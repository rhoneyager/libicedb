#pragma warning( disable : 4251 ) // DLL interface
#pragma warning( disable : 4661 ) // Exporting vector
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <complex>
#include <boost/filesystem.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cmath>
#include <ios>
#include <iomanip>
#include <thread>
#include <mutex>

//#include "../rtmath/zeros.h"
//#include "../rtmath/refract.h"
#include "ddOutput.h"
#include "ddpar.h"
#include "ddOriData.h"
//#include "ddweights.h"
#include "rotations.h"
#include "ddVersions.h"
#include "shapefile.h"
#include "icedb/units/units.hpp"
#include "dielTabFile.h"
#include "icedb/Errors/error.hpp"

namespace icedb {
	namespace registry {

		template struct IO_class_registry_writer
			< ::icedb::io::ddscat::ddOutput > ;

		template class usesDLLregistry <
			::icedb::io::ddscat::ddOutput_IO_output_registry,
			IO_class_registry_writer<::icedb::io::ddscat::ddOutput> > ;

		template struct IO_class_registry_reader
			< ::icedb::io::ddscat::ddOutput > ;

		template class usesDLLregistry <
			::icedb::io::ddscat::ddOutput_IO_input_registry,
			IO_class_registry_reader<::icedb::io::ddscat::ddOutput> > ;
	}
	namespace io {
		namespace ddscat {

			ddOutput::ddOutput() :
				freq(0), aeff(0), temp(0), numOriData(0)
			{
				resize(0, 0);
			}

			ddOutput::ddOutput(const ddOutput &src) :
				description(src.description),
				ingest_timestamp(src.ingest_timestamp),
				freq(src.freq),
				aeff(src.aeff),
				temp(src.temp),
				sources(src.sources),
				tags(src.tags),
				ddvertag(src.ddvertag),
				s(src.s),
				oridata_d(src.oridata_d),
				ms(src.ms),
				avgdata(src.avgdata),
				numOriData(src.numOriData),
				shape(src.shape)
			{
				fmldata = std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic, fmlColDefs::NUM_FMLCOLDEFS> >
					(new Eigen::Matrix<float, Eigen::Dynamic, fmlColDefs::NUM_FMLCOLDEFS>
					(*(src.fmldata)));
				parfile = ddPar::generate(src.parfile); //std::shared_ptr<ddPar>(new ddPar(*(src.parfile)));
			}

			ddOutput::shared_data::shared_data() : version(0), num_dipoles(0), navg(0) {
				mins.fill(0); maxs.fill(0), TA1TF.fill(0); TA2TF.fill(0); LFK.fill(0);
				IPV1LF.fill(std::complex<double>(0, 0));
				IPV2LF.fill(std::complex<double>(0, 0));
			}

			/** \brief Resize the orientation and fml tables
			*
			* The resizing operation is usually performed when data is loaded.
			* Resizing to a smaller size is typically quite destructive - ddOutputSingle
			* binds to a certain range of values for manipulations.
			*
			* \param numTotAngles is the total number of scattering angles considered, over
			* all orientations. Used to allow heterogeneous angle combinations, from combined
			* runs over the same particle.
			*
			* \param numOris is the number of orientation angles being considered.
			**/
			void ddOutput::resize(size_t numOris, size_t numTotAngles)
			{
				//std::cerr << "Resizing for hash " << ddOut->shapeHash.lower
				//	<< " numOris " << numOris << std::endl;
				oridata_d.conservativeResize(numOris, Eigen::NoChange);
				avgdata.avg.conservativeResize(1, Eigen::NoChange);
				//oridata_i.conservativeResize(numOris, Eigen::NoChange);
				//oridata_s.resize(1);
				ms.resize(numOris);
				numOriData = numOris;
				if (!fmldata) fmldata = std::shared_ptr
					<Eigen::Matrix<float, Eigen::Dynamic, fmlColDefs::NUM_FMLCOLDEFS> >
					(new Eigen::Matrix<float, Eigen::Dynamic, fmlColDefs::NUM_FMLCOLDEFS>());
				if (numTotAngles) resizeFML(numTotAngles);
			}

			/// \returns if row is found.
			bool ddOutput::getRow(double beta, double theta, double phi, size_t &row) const {
				auto inTol = [](double b, double br, double t, double tr,
					double p, double pr, double &res) -> bool {
					using namespace std;
					res = pow(b - br, 2.) + pow(t - tr, 2.) + pow(p - pr, 2.);
					res = pow(res, 0.5);
					if (res < 0.0001) return true;
					return false;
				};
				double resprev = 9999999, res = 9999999;
				for (size_t i = 0; i < (size_t)oridata_d.rows(); ++i) {
					auto o = oridata_d.block(i, stat_entries::BETA, 1, 3);
					bool r = inTol(o(0, 0), beta, o(0, 1), theta, o(0, 2), phi, res);
					if (res < resprev) {
						resprev = res;
						row = i;
					}
					else {
						//std::cerr << "Eval b " << o(0,0) << " t " << o(0,1) << " p " << o(0,2)
						//	<< " res " << res << " row " << i << std::endl;
					}
					if (r) return true;
				}
				return false;
			}

			void ddOutput::resizeFML(size_t numTotAngles)
			{
				fmldata->conservativeResize(numTotAngles, Eigen::NoChange);
			}

			ddOutput::Avgdata::Avgdata() : beta_min(0), beta_max(0), beta_n(0),
				theta_min(0), theta_max(0), theta_n(0),
				phi_min(0), phi_max(0), phi_n(0), hasAvg(0)
			{
				pdata = std::shared_ptr<Eigen::Matrix<float, Eigen::Dynamic,
					avgScaColDefs::NUM_AVGSCACOLDEFS> >(new Eigen::Matrix<float, Eigen::Dynamic,
						avgScaColDefs::NUM_AVGSCACOLDEFS>);
				pdata->resize(0, avgScaColDefs::NUM_AVGSCACOLDEFS);
			}

			/** \note This function will correct any bad wavelengths / effective
			 * radii in the parameter file read. It always assumes that the
			 * .avg file is correct. Needed when reading Holly's older data.
			 **/
			std::shared_ptr<ddOutput> ddOutput::generate(
				const std::string &avgfile, const std::string &parfile,
				const std::string &shpfile)
			{
				std::shared_ptr<ddOutput> res(new ddOutput());
				using namespace boost::filesystem;
				using std::shared_ptr;
				using std::vector;

				std::mutex m_shape, m_par, m_other, m_pathlist, m_filecheck;

				res->parfile = ddPar::generate(parfile);
				res->shape = ::icedb::io::ddscat::shapefile::shapefile::generate(shpfile);
				auto dat = ddOriData::generate(*res, avgfile);
				// Correct the par file with the correct wavelength
				// and effective radius.
				double wave = 0, aeff = 0;
				wave = dat->wave();
				aeff = dat->aeff();
				res->parfile->setWavelengths(wave, wave, 1, "LIN");
				res->parfile->setAeff(aeff, aeff, 1, "LIN");

				// Tag the source directory and ddscat version
				path pbavg = absolute(path(avgfile));
				res->sources.insert(pbavg.string());
				path pbpar = absolute(path(parfile));
				res->sources.insert(pbpar.string());
				path pbshp = absolute(path(shpfile));
				res->sources.insert(pbshp.string());

				// Apply consistent generation
				res->finalize();
				return res;
			}

			std::shared_ptr<ddOutput> ddOutput::generate(const std::string &dir, bool noLoadRots)
			{
				std::shared_ptr<ddOutput> res(new ddOutput());
				using namespace boost::filesystem;
				using std::shared_ptr;
				using std::vector;

				path pBase(dir);
				if (!exists(pBase)) ICEDB_throw(icedb::error::error_types::xMissingFile)
					.add("folder-name",dir);

				if (!is_directory(pBase))
				{
					std::vector<std::shared_ptr<ddOutput> > runs;
					icedb::io::readObjs(runs, dir);
					if (!runs.size()) ICEDB_throw(icedb::error::error_types::xEmptyInputFile)
						.add("filename", dir);
					res = runs[0];
				}
				else {

					// Perform a single-level iteration through the path tree
					vector<path> cands;
					cands.reserve(50000);
					copy(directory_iterator(pBase), directory_iterator(), back_inserter(cands));

					// Iterate over and load each file. Loading files in parallel because each read operation is 
					// rather slow. The ddscat file parsers could use much improvement.

					//shared_ptr<ddPar> parfile;
					//shared_ptr<shapefile::shapefile> shape;
					std::mutex m_fmls, m_fmlmap, m_shape, m_par, m_other, m_pathlist, m_filecheck;

					// Pair up matching sca and fml files for a combined read.
					// Keys with only an sca entry are avg files.
					std::map<path, std::pair<path, path> > orisources;

					const size_t numThreads = 8; // rtmath::debug::getConcurrentThreadsSupported();
					std::vector<std::thread> pool;

					auto loadShape = [&](const path &p)
					{
						std::lock_guard<std::mutex> lock(m_shape);
						if (res->shape) return; // Only needs to be loaded once
						//if (noLoadRots) return;
						// Note: the hashed object is the fundamental thing here that needs to be loaded
						// The other stuff is only loaded for processing, and is not serialized directly.
						res->shape = ::icedb::io::ddscat::shapefile::shapefile::generate(p.string());
						// Get the hash and load the stats
						//shapeHash = res->shape->hash();
						//if (dostats)
						//	res->stats = stats::shapeFileStats::genStats(res->shape);
					};
					auto loadPar = [&](const path &p)
					{
						std::lock_guard<std::mutex> lock(m_par);
						res->parfile = ddPar::generate(p.string());
					};
					auto loadAvg = [&](const path &p)
					{
						std::lock_guard<std::mutex> lock(m_other);
						auto dat = ddOriData::generate(*res, p.string());
						//ddOriData dat(*res, p.string());
					};

					for (const auto &praw : cands)
					{
						// Extract entension of files in ._ form
						// Note: some files (like mtable) have no extension. I don't use these.
						if (!praw.has_extension()) continue;
						path pext = praw.extension();
						path pfileid = praw;
						pfileid.replace_extension();

						if ((pext.string() == ".sca" || pext.string() == ".fml") && noLoadRots) continue;
						if (pext.string() == ".sca" || pext.string() == ".fml")
						{
							if (!orisources.count(pfileid)) orisources[pfileid] = std::pair<path, path>(path(), path());
							if (pext.string() == ".sca")
							{
								orisources[pfileid].second = praw;
							}
							else {
								orisources[pfileid].first = praw;
							}
						}
						else if (pext.string() == ".avg") {
							std::thread t(loadAvg, praw);
							pool.push_back(std::move(t));
						}
						else if (praw.filename().string() == "ddscat.par") { // Match full name
							std::thread t(loadPar, praw);
							pool.push_back(std::move(t));
						}
						else if (praw.filename().string() == "shape.dat") { // Match full name
							std::thread t(loadShape, praw);
							pool.push_back(std::move(t));
						}
						else if (praw.filename().string() == "target.out") { // Match full name
							// Backup shape load. The points should be exactly the same. One of these gets hit first.
							std::thread t(loadShape, praw);
							pool.push_back(std::move(t));
						}
					}

					std::vector<std::pair<path, path> > oris;
					oris.reserve(orisources.size());
					for (const auto &s : orisources)
						oris.push_back(s.second);
					res->resize(oris.size(), 0); // fml size is not yet known. These entries will be imported later.
					vector<std::shared_ptr<ddOriData> > fmls;
					fmls.reserve(orisources.size());
					size_t count = 0;

					auto process_path = [&](const std::pair<path, path> &p)
					{
						size_t mycount = count;
						{
							std::lock_guard<std::mutex> lock(m_fmls);
							++count;
						}
						auto dat = ddOriData::generate(*res, mycount, p.second.string(), p.first.string());
						{
							std::lock_guard<std::mutex> lock(m_fmlmap);
							fmls.push_back(std::move(dat));
						}
					};

					auto process_paths = [&]()
					{
						try {
							std::pair<path, path> p;
							for (;;)
							{
								{
									std::lock_guard<std::mutex> lock(m_pathlist);

									if (!oris.size()) return;
									p = oris.back();
									oris.pop_back();
								}

								process_path(p);
							}
						}
						catch (std::exception &e)
						{
							std::cerr << e.what() << std::endl;
							return;
						}
					};



					for (size_t i = 0; i < numThreads; i++)
					{
						std::thread t(process_paths);
						pool.push_back(std::move(t));
					}
					for (auto &t : pool)
						t.join();

					/// \todo Do table resorting for better memory access for both the orientation and fml tables.
					size_t numAngles = 0;
					for (const auto &f : fmls) // Avg files have no fml entries - only cross-sections are stored
						numAngles += f->numMat();
					res->resizeFML(numAngles);
					size_t i = 0;
					for (const auto &f : fmls)
					{
						f->doExportFMLs(i);
						i += f->numMat();
					}


					// Tag the source directory and ddscat version
					path pdir(dir);
					path pbdir = absolute(pdir);
					res->sources.insert(pbdir.string());

					// Apply consistent generation
					res->finalize();
				}
				return res;
			}

			void ddOutput::finalize()
			{
				using namespace boost::filesystem;
				using std::vector;

				tags.insert(std::pair<std::string, std::string>("target", s.target));

				/*
				auto selectData = [&]() -> Eigen::Block<ddOutput::doubleType, 1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES, false, true>
				{
				if (avg(0)))
				return avg.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(0, 0);
				return oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(0, 0);
				};
				*/
				if (numOriData || avgdata.hasAvg)
				{
					//auto od = selectData();
					std::shared_ptr<ddOriData> data;
					if (avgdata.hasAvg) data = ddOriData::generate(*this, "");
					else data = ddOriData::generate(*this, 0);

					ddvertag = s.target; /// \todo allow better ddscat version tagging

					freq = data->freq();
					aeff = data->aeff();
					temp = data->guessTemp();
				}
				else if (parfile) {
					// select first wavelength and effective radius
					double min, max;
					size_t n;
					std::string spacing;
					parfile->getWavelengths(min, max, n, spacing);
					freq = units::conv_spec("um", "GHz").convert(min);
					parfile->getAeff(min, max, n, spacing);
					aeff = min;
					// attempt to load the dielectric file
					using namespace boost::filesystem;
					std::vector<std::string> diels;
					parfile->getDiels(diels);
					/*
					if (diels.size() && sources.size()) {
						std::string pardir = *(sources.begin());
						path ppar = path(pardir);
						//path ppar = path(filename).remove_filename();
						path pval(diels[0]);
						path prel = boost::filesystem::absolute(pval, ppar);
						if (boost::filesystem::exists(prel))
						{
							dielTab dt(prel.string());
							std::complex<double> diel;
							diel = dt.interpolate(freq);
							temp = refract::guessTemp(freq, diel);
						}
						else temp = 0;
					}
					else */
						temp = 0;
				}
				else {
					aeff = 0;
					freq = 0;
					temp = 0;
				}

				if (shape) {
					// Save the shape in the hash location, if necessary
					//shape->writeToHash();
					// Resave the stats in the hash location
					//stats->writeToHash();
				}

				if (!ddvertag.size()) ddvertag = "unknown";
				//ingest_hostname = Ryan_Debug::getHostname();
				//ingest_username = Ryan_Debug::getUsername();
				using namespace boost::posix_time;
				using namespace boost::gregorian;
				ptime now = second_clock::local_time();
				ingest_timestamp = to_iso_string(now);
				//ingest_rtmath_version = rtmath::debug::rev();

			}

			void ddOutput::expand(const std::string &outdir, bool writeShape) //const
			{
				using namespace boost::filesystem;
				path pOut(outdir);
				if (exists(pOut))
				{
					ICEDB_throw(icedb::error::error_types::xFileExists)
					.add("Dir-name",outdir);
				}
				else {
					create_directory(pOut);
				}

				// Write par file
				if (parfile)
					this->parfile->writeFile((path(pOut) / "ddscat.par").string());

				// Write shape file
				if (writeShape)
				{
					if (this->shape)
						shape->writeFile((path(pOut) / "target.out").string());
					else 
						ICEDB_throw(icedb::error::error_types::xCannotFindReference)
						.add("Reason", "Shape cannot be written - it is not in memory");
				}

				auto onameb = [](const boost::filesystem::path &base,
					size_t i, size_t ni) -> std::string
				{
					using namespace std;
					ostringstream n;
					n << "w000r000"; //k";
					if (ni) {
						n << "k";
						n.width(ni);
						n.fill('0');
						n << i;
					}
					return (base / path(n.str())).string();
				};

				// Write fmls
				for (size_t i = 0; i < (size_t)oridata_d.rows(); ++i)
				{
					//if (oridata_d(i, stat_entries::DOWEIGHT)) {
					std::string basename = onameb(pOut, i, 4);
					std::string fmlname = basename;
					std::string scaname = basename;
					fmlname.append(".fml");
					scaname.append(".sca");

					auto obj = ddOriData::generate(*this, i);
					obj->doImportFMLs();
					obj->writeFile(fmlname);
					obj->writeFile(scaname);
					/*
					} else {
					// File is an avg file of some sort
					std::string basename = onameb(pOut, i, 0);
					basename.append(".avg");

					ddOriData obj(*this, i);
					//obj.doImportFMLs();
					obj.writeFile(basename);
					}
					*/
				}

				// Write out avg files
				auto avgdat = ddOriData::generate(*this);
				std::string oavg = onameb(pOut, 0, 0);
				oavg.append(".avg");
				avgdat->writeFile(oavg);

				// Write diel.tab files

				/*
				// Write out the weighting table
				{
				std::ofstream owt( (path(pOut)/"weights.tsv").string() );
				owt << "Theta\tPhi\tBeta\tWeight\n";
				for (auto w : weights)
				{
				owt << w.first->theta() << "\t" << w.first->phi() << "\t"
				<< w.first->beta() << "\t" << w.second << "\n";
				}
				}
				*/
			}

			std::string ddOutput::stat_entries::stringify(int val)
			{
#define _tostr(a) #a
#define tostr(a) _tostr(a)
#define check(a) if (val == a) return std::string( tostr(a) );
				check(D); //check(XMIN); check(XMAX); check(YMIN);
				//check(YMAX); check(ZMIN); check(ZMAX); 
				check(AEFF);
				check(WAVE); //check(FREQ); //check(NAMBIENT); check(TOL);
				//check(TA1TFX); check(TA1TFY); check(TA1TFZ); check(TA2TFX);
				//check(TA2TFY); check(TA2TFZ); 
				check(TFKX); check(TFKY);
				check(TFKZ); check(IPV1TFXR); check(IPV1TFXI); check(IPV1TFYR);
				check(IPV1TFYI); check(IPV1TFZR); check(IPV1TFZI); check(IPV2TFXR);
				check(IPV2TFXI); check(IPV2TFYR); check(IPV2TFYI); check(IPV2TFZR);
				check(IPV2TFZI); check(TA1LFX); check(TA1LFY); check(TA1LFZ);
				check(TA2LFX); check(TA2LFY); check(TA2LFZ);
				//check(LFKX);
				//check(LFKY); check(LFKZ); check(IPV1LFXR); check(IPV1LFXI);
				//check(IPV1LFYR); check(IPV1LFYI); check(IPV1LFZR); check(IPV1LFZI);
				//check(IPV2LFXR); check(IPV2LFXI); check(IPV2LFYR); check(IPV2LFYI);
				//check(IPV2LFZR); check(IPV2LFZI); 
				check(BETA); check(THETA);
				check(PHI); //check(ETASCA); 
				check(QEXT1); check(QABS1);
				check(QSCA1); check(G11); check(G21); check(QBK1);
				check(QPHA1); check(QEXT2); check(QABS2); check(QSCA2);
				check(G12); check(G22); check(QBK2); check(QPHA2);
				check(QEXTM); check(QABSM); check(QSCAM); check(G1M);
				check(G2M); check(QBKM); check(QPHAM); check(QPOL);
				check(DQPHA); check(QSCAG11); check(QSCAG21); check(QSCAG31);
				check(ITER1); check(MXITER1); check(NSCA1);
				check(QSCAG12);
				check(QSCAG22); check(QSCAG32);
				check(ITER2); check(MXITER2); check(NSCA2);
				check(QSCAG1M); check(QSCAG2M); check(QSCAG3M);
				//check(DOWEIGHT);
				return std::string("");
#undef _tostr
#undef tostr
#undef check
			}

			std::string ddOutput::fmlColDefs::stringify(int val)
			{
#define _tostr(a) #a
#define tostr(a) _tostr(a)
#define check(a) if (val == a) return std::string( tostr(a) );
				check(ORIINDEX); check(THETAB); check(PHIB);
				check(F00R); check(F00I); check(F01R); check(F01I);
				check(F10R); check(F10I); check(F11R); check(F11I);
				return std::string("");
#undef _tostr
#undef tostr
#undef check
			}

		}
	}
}
