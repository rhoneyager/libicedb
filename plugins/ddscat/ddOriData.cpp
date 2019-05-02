#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <boost/filesystem.hpp>
#include <complex>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/iostreams/filter/newline.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/shared_ptr.hpp>

#include "ddOutput.h"
#include "ddOriData.h"
#include "ddpar.h"
#include "ddScattMatrix.h"
#include "ddVersions.h"
#include "rotations.h"
//#include "refract.h"
#include "icedb/units/units.hpp"
#include "icedb/Errors/error.hpp"
//#include <icedb/quadrature.hpp>

#include "ddOriDataParsers.h"

namespace {
	static std::mutex implementsDDRESmlock;
}

namespace icedb {

	namespace registry {
		template struct IO_class_registry_writer
			< ::icedb::io::ddscat::ddOriData >;

		template struct IO_class_registry_reader
			< ::icedb::io::ddscat::ddOriData >;

		template class usesDLLregistry <
			::icedb::io::ddscat::ddOriData_IO_output_registry,
			IO_class_registry_writer<::icedb::io::ddscat::ddOriData> >;

		template class usesDLLregistry <
			::icedb::io::ddscat::ddOriData_IO_input_registry,
			IO_class_registry_reader<::icedb::io::ddscat::ddOriData> >;
	}
}
namespace icedb {
	namespace io {
		namespace ddscat {

			implementsDDRES::implementsDDRES() :
				icedb::io::implementsIObasic<ddOriData, ddOriData_IO_output_registry,
				ddOriData_IO_input_registry, ddOriData_Standard>(ddOriData::writeDDSCAT, ddOriData::readDDSCAT, known_formats())
			{}

			const std::set<std::string>& implementsDDRES::known_formats()
			{
				static std::set<std::string> mtypes;
				// Prevent threading clashes
				{
					std::lock_guard<std::mutex> lck(implementsDDRESmlock);
					if (!mtypes.size())
					{
						mtypes.insert(".avg");
						mtypes.insert(".fml");
						mtypes.insert(".sca");
						mtypes.insert("avg_");
					}
				}
				return mtypes;
			}

			void ddOriData::readDDSCAT(std::shared_ptr<ddOriData> obj, std::istream&in, std::shared_ptr<icedb::registry::IO_options> opts)
			{
				std::string filename = opts->filename();
				std::string filetype = opts->filetype();
				std::string ext = opts->extension();
				std::string utype = filetype;
				if (!utype.size()) utype = filetype;
				if (!utype.size()) utype = boost::filesystem::path(filename).extension().string().c_str();
				if (utype == ".sca") {
					obj->readSCA(in);
				}
				else if (utype == ".fml") {
					obj->readFML(in);
				}
				else if (utype == ".avg") {
					obj->readAVG(in);
				}
				else if (filename.find("avg_") != std::string::npos) {
					obj->readAVG(in);
				}
				else {
					ICEDB_throw(icedb::error::error_types::xUnknownFileFormat)
						.add("Filename",filename);
				}
			}

			void ddOriData::writeDDSCAT(const ddOriData* obj, std::ostream &out, std::shared_ptr<icedb::registry::IO_options> opts)
			{
				//bool isFMLforced = false, writeFML = true;
				//ddOutput::isForcingFMLwrite(isFMLforced, writeFML);
				//if (isFMLforced) opts->setVal<bool>("writeFML", writeFML);

				std::string filename = opts->filename();
				std::string filetype = opts->filetype();
				std::string ext = opts->extension();
				std::string utype = filetype;
				if (!utype.size()) utype = filetype;
				if (!utype.size()) utype = boost::filesystem::path(filename).extension().string().c_str();
				if (utype == ".sca") {
					obj->writeSCA(out);
				}
				else if (utype == ".fml") {
					obj->writeFML(out);
				}
				else if (utype == ".avg") {
					obj->writeAVG(out);
				}
				else if (filename.find("avg_") != std::string::npos) {
					obj->writeAVG(out);
				}
				else {
					ICEDB_throw(icedb::error::error_types::xUnknownFileFormat)
						.add("Filename",filename);
				}
			}

			ddOriData::ddOriData(ddOutput &parent, size_t row, bool isAvg) :
				_parent(parent), _row(row), isAvg(isAvg)
			{
				_init();
			}

			std::shared_ptr<ddOriData> ddOriData::generate(ddOutput &parent, size_t row,
				const std::string &filenameSCA,
				const std::string &filenameFML)
			{
				std::shared_ptr<ddOriData> res(new ddOriData(parent, row, false));
				// Double read to capture fml data and cross-sections in one container.
				if (filenameSCA.size()) res->readFile(filenameSCA);
				if (filenameFML.size()) res->readFile(filenameFML);
				return res;
			}

			std::shared_ptr<ddOriData> ddOriData::generate(ddOutput &parent, const std::string &filenameAVG)
			{
				std::shared_ptr<ddOriData> res(new ddOriData(parent, 0, true));
				if (filenameAVG.size()) res->readFile(filenameAVG);
				return res;
			}

			void ddOriData::_init()
			{
				//_statTable_Size_ts.at(stat_entries_size_ts::VERSION)
				//	= icedb::io::ddscat::ddVersions::getDefaultVer();
			}



			void ddOriData::doExportFMLs(size_t startIndex) const
			{
				auto o = _parent.fmldata->block(startIndex, 0, numMat(), ddOutput::fmlColDefs::NUM_FMLCOLDEFS);
				size_t i = 0;
				for (auto it = _scattMatricesRaw.begin(); it != _scattMatricesRaw.end(); ++it)
				{
					if ((*it).id() != scattMatrixType::F) continue;
					//auto s = boost::dynamic_pointer_cast<const ddscat::ddScattMatrixF>(*it);

					auto f = it->getF();

					o(i, ddOutput::fmlColDefs::ORIINDEX) = static_cast<float>(_row);
					o(i, ddOutput::fmlColDefs::THETAB) = static_cast<float>((*it).theta());
					o(i, ddOutput::fmlColDefs::PHIB) = static_cast<float>((*it).phi());
					o(i, ddOutput::fmlColDefs::F00R) = static_cast<float>(f(0, 0).real());
					o(i, ddOutput::fmlColDefs::F00I) = static_cast<float>(f(0, 0).imag());
					o(i, ddOutput::fmlColDefs::F01R) = static_cast<float>(f(0, 1).real());
					o(i, ddOutput::fmlColDefs::F01I) = static_cast<float>(f(0, 1).imag());
					o(i, ddOutput::fmlColDefs::F10R) = static_cast<float>(f(1, 0).real());
					o(i, ddOutput::fmlColDefs::F10I) = static_cast<float>(f(1, 0).imag());
					o(i, ddOutput::fmlColDefs::F11R) = static_cast<float>(f(1, 1).real());
					o(i, ddOutput::fmlColDefs::F11I) = static_cast<float>(f(1, 1).imag());

					i++;
				}
			}

			void ddOriData::doImportFMLs()
			{
				size_t startRow = 0;
				size_t stopRow = 0;
				bool startFound = false;
				bool endFound = false;
				for (size_t i = 0; i < (size_t)_parent.fmldata->rows(); ++i)
				{
					size_t oriindex = static_cast<size_t>((*(_parent.fmldata))(i, ddOutput::fmlColDefs::ORIINDEX));
					if (!startFound && oriindex == _row)
					{
						startFound = true;
						startRow = oriindex;
					}
					if (startFound && oriindex != _row)
					{
						endFound = true;
						stopRow = oriindex;
						break;
					}
				}
				if (!startFound) return;
				if (!endFound) stopRow = _parent.fmldata->rows();
				if (stopRow <= startRow) ICEDB_throw(icedb::error::error_types::xArrayOutOfBounds);

				doImportFMLs(startRow, stopRow - startRow);
			}

			void ddOriData::doImportFMLs(size_t startIndex, size_t n)
			{
				auto o = _parent.fmldata->block(startIndex, 0, n, ddOutput::fmlColDefs::NUM_FMLCOLDEFS);
				size_t i = 0;
				_scattMatricesRaw.clear();
				_scattMatricesRaw.reserve(n);
				using std::complex;
				for (i = 0; i < n; ++i)
				{
					ddScattMatrixF mat(freq(), o(i, ddOutput::fmlColDefs::THETAB), o(i, ddOutput::fmlColDefs::PHIB),
						0, 0, getConnector());
					ddScattMatrix::FType fs;
					fs(0, 0) = complex<double>(o(i, ddOutput::fmlColDefs::F00R), o(i, ddOutput::fmlColDefs::F00I));
					fs(1, 0) = complex<double>(o(i, ddOutput::fmlColDefs::F01R), o(i, ddOutput::fmlColDefs::F01I));
					fs(0, 1) = complex<double>(o(i, ddOutput::fmlColDefs::F10R), o(i, ddOutput::fmlColDefs::F10I));
					fs(1, 1) = complex<double>(o(i, ddOutput::fmlColDefs::F11R), o(i, ddOutput::fmlColDefs::F11I));
					mat.setF(fs);

					//std::shared_ptr<const ddScattMatrix> matC =
					//	boost::dynamic_pointer_cast<const ddScattMatrix>(mat);

					//_scattMatricesRaw.push_back(matC);
					_scattMatricesRaw.push_back(mat);
				}
			}

			size_t ddOriData::version() const
			{
				return _parent.s.version;
			}

			size_t ddOriData::numDipoles() const
			{
				return _parent.s.num_dipoles;
			}

			Eigen::Block<ddOutput::doubleType, 1,
				ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES,
				false> //, true>
				ddOriData::selectData() const
			{
				if (isAvg)
					return _parent.avgdata.avg.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(0, 0);
				return _parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(_row, 0);
			}

			double ddOriData::freq() const
			{
				auto od = selectData();
				//std::cerr << _row << " freq() " << od(ddOutput::stat_entries::WAVE) << std::endl;
				return units::conv_spec("um", "GHz").convert(od(ddOutput::stat_entries::WAVE));
			}

			ddOriData::~ddOriData() {}

			/// Input in avg format
			void ddOriData::readAVG(std::istream &in)
			{
				using namespace std;
				using namespace ddOriDataParsers;
				_parent.avgdata.hasAvg = true;
				auto od = _parent.avgdata.avg.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(0, 0);
				//auto od = _parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(_row, 0);
				ddOutput::shared_data s;
				{
					std::lock_guard<std::mutex> lock(_parent.mtxUpdate);
					s = _parent.s;
				}
				//auto &os = _parent.oridata_s.at(_row);
				//auto &oi = _parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(_row, 0);
				// Zero any unread entries (occurs only with an avg file, not with sca+fml pairs)
				od.setZero();
				//oi.setZero();

				std::string junk;

				s.version = version::read(in, this->version());
				simpleString::read(in, s.target);
				std::getline(in, junk); // simpleStringRev::read(in, _parent.ddameth);
				std::getline(in, junk); // simpleStringRev::read(in, _parent.ccgmeth);
				std::getline(in, junk); // simpleStringRev::read(in, _parent.hdr_shape);
				simpleNumRev<size_t>::read(in, s.num_dipoles);
				std::getline(in, junk); // d/aeff
				simpleNumRev<double>::read(in, od(ddOutput::stat_entries::D));

				simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::AEFF));
				simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::WAVE));

				//od(ddOutput::stat_entries::FREQ) = units::conv_spec("um", "GHz").convert(od(ddOutput::stat_entries::WAVE));

				std::getline(in, junk); // k*aeff
				if (icedb::io::ddscat::ddVersions::isVerWithin(s.version, 72, 0))
					std::getline(in, junk);
				//simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::NAMBIENT));

			// Read refractive indices (plural)
				std::string lin; // Used for peeking ahead
				for (size_t i = 0;; ++i)
				{
					std::getline(in, lin);
					if (lin.at(0) != 'n') break; // No more refractive indices
					size_t subst = 0;
					std::complex<double> m;
					refractive::read(lin, subst, m);
					_parent.avgdata.avg_ms.push_back(m);
				}

				//simpleNumCompound<double>::read(lin, od(ddOutput::stat_entries::TOL)); // lin from refractive index read

				std::vector<double> a(3);
				size_t axisnum = 0;
				frameType frm = frameType::TF;
				ddAxisVec::read(in, a, axisnum, frm);
				s.TA1TF[0] = a[0]; s.TA1TF[1] = a[1]; s.TA1TF[2] = a[2];
				ddAxisVec::read(in, a, axisnum, frm);
				s.TA2TF[0] = a[0]; s.TA2TF[1] = a[1]; s.TA2TF[2] = a[2];

				simpleNumCompound<size_t>::read(in, s.navg);
				//od(ddOutput::stat_entries::DOWEIGHT) = 0;

				ddAxisVec::read(in, a, axisnum, frm);
				s.LFK[0] = a[0]; s.LFK[1] = a[1]; s.LFK[2] = a[2];


				std::vector<std::complex<double> > iv(3);
				size_t vecnum = 0;
				ddPolVec::read(in, iv, vecnum, frm);
				s.IPV1LF[0] = iv[0];
				s.IPV1LF[1] = iv[1];
				s.IPV1LF[2] = iv[2];
				ddPolVec::read(in, iv, vecnum, frm);
				s.IPV2LF[0] = iv[0];
				s.IPV2LF[1] = iv[1];
				s.IPV2LF[2] = iv[2];

				ddRot1d::read(in, junk, _parent.avgdata.beta_min, _parent.avgdata.beta_max, _parent.avgdata.beta_n);
				ddRot1d::read(in, junk, _parent.avgdata.theta_min, _parent.avgdata.theta_max, _parent.avgdata.theta_n);
				ddRot1d::read(in, junk, _parent.avgdata.phi_min, _parent.avgdata.phi_max, _parent.avgdata.phi_n);
				std::getline(in, junk); // empty line

				std::getline(in, junk); // simpleNumRev<double>::read(in, od(ddOutput::stat_entries::ETASCA));
				// num orientations
				ddNumOris::read(in, _parent.avgdata.num_avg_entries);
				//std::getline(in, junk); // num orientations

				std::getline(in, junk); // num polarizations

				std::getline(in, junk); //"          Qext       Qabs       Qsca      g(1)=<cos>  <cos^2>     Qbk       Qpha" << endl;

				readStatTable(in);
				{
					std::lock_guard<std::mutex> lock(_parent.mtxUpdate);
					_parent.s = s;
				}
				readMuellerDDSCAT(in);
			}

			/// Input in sca format
			void ddOriData::readSCA(std::istream &in)
			{
				using namespace std;
				using namespace ddOriDataParsers;

				auto od = _parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(_row, 0);
				ddOutput::shared_data s;
				{
					std::lock_guard<std::mutex> lock(_parent.mtxUpdate);
					s = _parent.s;
				}
				//auto &os = _parent.oridata_s.at(_row);
				//auto &oi = _parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(_row, 0);
				std::string junk;


				s.version = version::read(in, this->version());
				simpleString::read(in, s.target);
				std::getline(in, junk); // simpleStringRev::read(in, _parent.ddameth);
				std::getline(in, junk); // simpleStringRev::read(in, _parent.ccgmeth);
				std::getline(in, junk); // simpleStringRev::read(in, _parent.hdr_shape);
				simpleNumRev<size_t>::read(in, s.num_dipoles);

				std::getline(in, junk); // d/aeff
				simpleNumRev<double>::read(in, od(ddOutput::stat_entries::D));
				std::getline(in, junk); // physical extent
				ddPhysExtent::read(in, s.mins[0], s.maxs[0], junk[0]);
				ddPhysExtent::read(in, s.mins[1], s.maxs[1], junk[0]);
				ddPhysExtent::read(in, s.mins[2], s.maxs[2], junk[0]);

				simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::AEFF));
				simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::WAVE));
				//od(ddOutput::stat_entries::FREQ) = units::conv_spec("um", "GHz").convert(od(ddOutput::stat_entries::WAVE));

				std::getline(in, junk); // k*aeff
				if (icedb::io::ddscat::ddVersions::isVerWithin(s.version, 72, 0))
					std::getline(in, junk);
				//simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::NAMBIENT));

				// Read refractive indices (plural)
				std::string lin; // Used for peeking ahead
				for (size_t i = 0;; ++i)
				{
					std::getline(in, lin);
					if (lin.at(0) != 'n') break; // No more refractive indices
					size_t subst = 0;
					std::complex<double> m;
					refractive::read(lin, subst, m);
					//_parent.ms[_row].push_back(m); // FML read instead does this - AVG read still does it.
				}

				//simpleNumCompound<double>::read(lin, od(ddOutput::stat_entries::TOL)); // lin from refractive index read

				std::vector<double> a(3);
				size_t axisnum = 0;
				frameType frm = frameType::TF;
				ddAxisVec::read(in, a, axisnum, frm);
				s.TA1TF[0] = a[0]; s.TA1TF[1] = a[1]; s.TA1TF[2] = a[2];
				ddAxisVec::read(in, a, axisnum, frm);
				s.TA2TF[0] = a[0]; s.TA2TF[1] = a[1]; s.TA2TF[2] = a[2];

				simpleNumCompound<size_t>::read(in, s.navg);
				//od(ddOutput::stat_entries::DOWEIGHT) = 1;

				ddAxisVec::read(in, a, axisnum, frm);
				od(ddOutput::stat_entries::TFKX) = a[0]; od(ddOutput::stat_entries::TFKY) = a[1]; od(ddOutput::stat_entries::TFKZ) = a[2];


				std::vector<std::complex<double> > iv(3);
				size_t vecnum = 0;
				ddPolVec::read(in, iv, vecnum, frm);
				od(ddOutput::stat_entries::IPV1TFXR) = iv[0].real(); od(ddOutput::stat_entries::IPV1TFXI) = iv[0].imag();
				od(ddOutput::stat_entries::IPV1TFYR) = iv[1].real(); od(ddOutput::stat_entries::IPV1TFYI) = iv[1].imag();
				od(ddOutput::stat_entries::IPV1TFZR) = iv[2].real(); od(ddOutput::stat_entries::IPV1TFZI) = iv[2].imag();
				ddPolVec::read(in, iv, vecnum, frm);
				od(ddOutput::stat_entries::IPV2TFXR) = iv[0].real(); od(ddOutput::stat_entries::IPV2TFXI) = iv[0].imag();
				od(ddOutput::stat_entries::IPV2TFYR) = iv[1].real(); od(ddOutput::stat_entries::IPV2TFYI) = iv[1].imag();
				od(ddOutput::stat_entries::IPV2TFZR) = iv[2].real(); od(ddOutput::stat_entries::IPV2TFZI) = iv[2].imag();

				ddAxisVec::read(in, a, axisnum, frm);
				od(ddOutput::stat_entries::TA1LFX) = a[0]; od(ddOutput::stat_entries::TA1LFY) = a[1]; od(ddOutput::stat_entries::TA1LFZ) = a[2];
				ddAxisVec::read(in, a, axisnum, frm);
				od(ddOutput::stat_entries::TA2LFX) = a[0]; od(ddOutput::stat_entries::TA2LFY) = a[1]; od(ddOutput::stat_entries::TA2LFZ) = a[2];


				ddAxisVec::read(in, a, axisnum, frm);
				s.LFK[0] = a[0]; s.LFK[1] = a[1]; s.LFK[2] = a[2];

				ddPolVec::read(in, iv, vecnum, frm);
				s.IPV1LF[0] = iv[0];
				s.IPV1LF[1] = iv[1];
				s.IPV1LF[2] = iv[2];
				ddPolVec::read(in, iv, vecnum, frm);
				s.IPV2LF[0] = iv[0];
				s.IPV2LF[1] = iv[1];
				s.IPV2LF[2] = iv[2];

				simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::BETA));
				simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::THETA));
				simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::PHI));

				std::getline(in, junk); // simpleNumRev<double>::read(in, od(ddOutput::stat_entries::ETASCA));

				std::getline(in, junk); //"          Qext       Qabs       Qsca      g(1)=<cos>  <cos^2>     Qbk       Qpha" << endl;

				readStatTable(in);
				{
					std::lock_guard<std::mutex> lock(_parent.mtxUpdate);
					_parent.s = s;
				}
				//readMuellerDDSCAT(in);
			}

			/// Input in fml format
			void ddOriData::readFML(std::istream &in)
			{
				using namespace std;
				using namespace ddOriDataParsers;

				auto od = _parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(_row, 0);
				ddOutput::shared_data s;
				{
					std::lock_guard<std::mutex> lock(_parent.mtxUpdate);
					s = _parent.s;
				}
				//auto &os = _parent.oridata_s.at(_row);
				//auto &oi = _parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(_row, 0);
				std::string junk;

				s.version = version::read(in, this->version());
				simpleString::read(in, s.target);
				std::getline(in, junk); // simpleStringRev::read(in, _parent.ccgmeth);
				std::getline(in, junk); // simpleStringRev::read(in, _parent.ddameth);
				std::getline(in, junk); // simpleStringRev::read(in, _parent.hdr_shape);
				simpleNumRev<size_t>::read(in, s.num_dipoles);
				//std::getline(in, junk); // d/aeff
				//simpleNumRev<double>::read(in, od(ddOutput::stat_entries::D));
				simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::AEFF));
				std::getline(in, junk); // (Bad FREQ lines in FML!) simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::WAVE));
				//od(ddOutput::stat_entries::FREQ) = units::conv_spec("um", "GHz").convert(od(ddOutput::stat_entries::WAVE));

				std::getline(in, junk); // k*aeff
				if (icedb::io::ddscat::ddVersions::isVerWithin(s.version, 72, 0))
					std::getline(in, junk);
				//simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::NAMBIENT));

				// Read refractive indices (plural)
				std::string lin; // Used for peeking ahead
				for (size_t i = 0;; ++i)
				{
					std::getline(in, lin);
					if (lin.at(0) != 'n') break; // No more refractive indices
					size_t subst = 0;
					std::complex<double> m;
					refractive::read(lin, subst, m);
					_parent.ms[_row].push_back(m);
				}

				//simpleNumCompound<double>::read(lin, od(ddOutput::stat_entries::TOL)); // lin from refractive index read
				simpleNumCompound<size_t>::read(in, s.navg);
				//od(ddOutput::stat_entries::DOWEIGHT) = 1;

				std::vector<double> a(3);
				size_t axisnum = 0;
				frameType frm = frameType::TF;
				ddAxisVec::read(in, a, axisnum, frm);
				s.TA1TF[0] = a[0]; s.TA1TF[1] = a[1]; s.TA1TF[2] = a[2];
				ddAxisVec::read(in, a, axisnum, frm);
				s.TA2TF[0] = a[0]; s.TA2TF[1] = a[1]; s.TA2TF[2] = a[2];
				ddAxisVec::read(in, a, axisnum, frm);
				od(ddOutput::stat_entries::TFKX) = a[0]; od(ddOutput::stat_entries::TFKY) = a[1]; od(ddOutput::stat_entries::TFKZ) = a[2];


				std::vector<std::complex<double> > iv(3);
				size_t vecnum = 0;
				ddPolVec::read(in, iv, vecnum, frm);
				od(ddOutput::stat_entries::IPV1TFXR) = iv[0].real(); od(ddOutput::stat_entries::IPV1TFXI) = iv[0].imag();
				od(ddOutput::stat_entries::IPV1TFYR) = iv[1].real(); od(ddOutput::stat_entries::IPV1TFYI) = iv[1].imag();
				od(ddOutput::stat_entries::IPV1TFZR) = iv[2].real(); od(ddOutput::stat_entries::IPV1TFZI) = iv[2].imag();
				ddPolVec::read(in, iv, vecnum, frm);
				od(ddOutput::stat_entries::IPV2TFXR) = iv[0].real(); od(ddOutput::stat_entries::IPV2TFXI) = iv[0].imag();
				od(ddOutput::stat_entries::IPV2TFYR) = iv[1].real(); od(ddOutput::stat_entries::IPV2TFYI) = iv[1].imag();
				od(ddOutput::stat_entries::IPV2TFZR) = iv[2].real(); od(ddOutput::stat_entries::IPV2TFZI) = iv[2].imag();

				ddAxisVec::read(in, a, axisnum, frm);
				s.LFK[0] = a[0]; s.LFK[1] = a[1]; s.LFK[2] = a[2];

				ddPolVec::read(in, iv, vecnum, frm);
				s.IPV1LF[0] = iv[0];
				s.IPV1LF[1] = iv[1];
				s.IPV1LF[2] = iv[2];
				ddPolVec::read(in, iv, vecnum, frm);
				s.IPV2LF[0] = iv[0];
				s.IPV2LF[1] = iv[1];
				s.IPV2LF[2] = iv[2];

				simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::BETA));
				simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::THETA));
				simpleNumCompound<double>::read(in, od(ddOutput::stat_entries::PHI));


				std::getline(in, junk); // "     Finite target:\n"
				std::getline(in, junk); // "     e_m dot E(r) = i*exp(ikr)*f_ml*E_inc(0)/(kr)\n"
				std::getline(in, junk); // "     m=1 in scatt. plane, m=2 perp to scatt. plane\n";
				std::getline(in, junk); //out << endl;

				std::getline(in, junk); //readHeader(in, "Re(f_11)");
				// Get e1 and e2 in lab frame from the header data
				auto cn = getConnector();
				_scattMatricesRaw.clear();
				_scattMatricesRaw.reserve(40);
				{
					std::lock_guard<std::mutex> lock(_parent.mtxUpdate);
					_parent.s = s;
				}
				readF_DDSCAT(in, cn);
			}


			void ddOriData::writeAVG(std::ostream &out) const
			{
				// Write the file in the appropriate order
				using namespace std;
				using namespace ddOriDataParsers;
				//using namespace ddOutput::stat_entries;
				const auto od = _parent.avgdata.avg.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(0, 0);
				const auto &s = _parent.s;
				//const auto &os = _parent.oridata_s.at(_row);
				//const auto &oi = _parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(_row, 0);

				version::write(out, _parent.s.version);
				simpleString::write(out, this->version(), _parent.s.target, "TARGET");
				simpleStringRev::write(out, this->version(), _parent.parfile->getCalpha(), "DDA method");
				simpleStringRev::write(out, this->version(), _parent.parfile->getSolnMeth(), "CCG method");
				simpleStringRev::write(out, this->version(), _parent.parfile->getShape(), "shape");
				simpleNumRev<size_t>::write(out, this->version(), _parent.s.num_dipoles, "NAT0 = number of dipoles");
				double daeff = od(ddOutput::stat_entries::D) / od(ddOutput::stat_entries::AEFF);
				simpleNumRev<double>::write(out, this->version(), daeff, "d/aeff for this target [d=dipole spacing]");
				simpleNumRev<double>::write(out, this->version(), od(ddOutput::stat_entries::D), "d (physical units)");

				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::AEFF), 12, "  AEFF=  ", "effective radius (physical units)");
				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::WAVE), 12, "  WAVE=  ", "wavelength (in vacuo, physical units)");
				double k = 2. * boost::math::constants::pi<double>() / od(ddOutput::stat_entries::WAVE);
				double kaeff = 2. * boost::math::constants::pi<double>() * od(ddOutput::stat_entries::AEFF) / od(ddOutput::stat_entries::WAVE);
				simpleNumCompound<double>::write(out, this->version(), kaeff, 12, "K*AEFF=  ", "2*pi*aeff/lambda");

				if (icedb::io::ddscat::ddVersions::isVerWithin(version(), 72, 0))
					simpleNumCompound<double>::write(out, this->version(), _parent.parfile->nAmbient(), 8, "NAMBIENT=    ", "refractive index of ambient medium");
				//simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::NAMBIENT), 8, "NAMBIENT=    ", "refractive index of ambient medium");

			// Write refractive indices (plural)
				for (size_t i = 0; i < _parent.avgdata.avg_ms.size(); ++i)
					refractive::write(out, this->version(), i + 1, _parent.avgdata.avg_ms[i], k, od(ddOutput::stat_entries::D));

				simpleNumCompound<double>::write(out, this->version(), _parent.parfile->maxTol(), 9, "   TOL= ", " error tolerance for CCG method");

				std::vector<double> a(3);
				a[0] = s.TA1TF[0]; a[1] = s.TA1TF[1]; a[2] = s.TA1TF[2];
				ddAxisVec::write(out, this->version(), a, 1, frameType::TF);
				a[0] = s.TA2TF[0]; a[1] = s.TA2TF[1]; a[2] = s.TA2TF[2];
				ddAxisVec::write(out, this->version(), a, 2, frameType::TF);


				simpleNumCompound<size_t>::write(out, this->version(), _parent.s.navg, 5, "  NAVG= ", "(theta,phi) values used in comp. of Qsca,g");

				a[0] = s.LFK[0]; a[1] = s.LFK[1]; a[2] = s.LFK[2];
				ddAxisVec::write(out, this->version(), a, 0, frameType::LF);

				std::vector<std::complex<double> > iv(3);
				iv[0] = s.IPV1LF[0];
				iv[1] = s.IPV1LF[1];
				iv[2] = s.IPV1LF[2];
				ddPolVec::write(out, this->version(), iv, 1, frameType::LF);
				iv[0] = s.IPV2LF[0];
				iv[1] = s.IPV2LF[1];
				iv[2] = s.IPV2LF[2];
				ddPolVec::write(out, this->version(), iv, 2, frameType::LF);

				ddRot1d::write(out, this->version(), "beta", _parent.avgdata.beta_min, _parent.avgdata.beta_max, _parent.avgdata.beta_n, "NBETA");
				ddRot1d::write(out, this->version(), "theta", _parent.avgdata.theta_min, _parent.avgdata.theta_max, _parent.avgdata.theta_n, "NTHETA");
				ddRot1d::write(out, this->version(), "phi", _parent.avgdata.phi_min, _parent.avgdata.phi_max, _parent.avgdata.phi_n, "NPHI");

				out << endl;

				simpleNumRev<double>::write(out, this->version(), _parent.parfile->etasca(),
					"ETASCA = param. controlling # of scatt. dirs used to calculate <cos> etc.", 1, 6);

				ddNumOris::write(out, this->version(), _parent.avgdata.num_avg_entries);
				out << "                   and    2 incident polarizations\n";


				// Write the odd table of Qsca and the others
				writeStatTable(out);

				// Write the P matrix (not supported for avg file here)
				// TODO: fix this!
				writeMuellerDDSCAT(out);
			}

			void ddOriData::writeSCA(std::ostream &out) const
			{
				// Write the file in the appropriate order
				using namespace std;
				using namespace ddOriDataParsers;
				//using namespace ddOutput::stat_entries;
				const auto od = _parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(_row, 0);
				const auto &s = _parent.s;
				//const auto &os = _parent.oridata_s.at(_row);
				//const auto &oi = _parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(_row, 0);

				version::write(out, _parent.s.version);
				simpleString::write(out, this->version(), _parent.s.target, "TARGET");
				simpleStringRev::write(out, this->version(), _parent.parfile->getCalpha(), "DDA method");
				simpleStringRev::write(out, this->version(), _parent.parfile->getSolnMeth(), "CCG method");
				simpleStringRev::write(out, this->version(), _parent.parfile->getShape(), "shape");
				simpleNumRev<size_t>::write(out, this->version(), _parent.s.num_dipoles, "NAT0 = number of dipoles");
				double daeff = od(ddOutput::stat_entries::D) / od(ddOutput::stat_entries::AEFF);
				simpleNumRev<double>::write(out, this->version(), daeff, "d/aeff for this target [d=dipole spacing]");
				simpleNumRev<double>::write(out, this->version(), od(ddOutput::stat_entries::D), "d (physical units)");

				out << "----- physical extent of target volume in Target Frame ------\n";
				ddPhysExtent::write(out, this->version(), s.mins[0], s.maxs[0], 'x');
				ddPhysExtent::write(out, this->version(), s.mins[1], s.maxs[1], 'y');
				ddPhysExtent::write(out, this->version(), s.mins[2], s.maxs[2], 'z');

				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::AEFF), 12, "  AEFF=  ", "effective radius (physical units)");
				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::WAVE), 12, "  WAVE=  ", "wavelength (in vacuo, physical units)");
				double k = 2. * boost::math::constants::pi<double>() / od(ddOutput::stat_entries::WAVE);
				double kaeff = 2. * boost::math::constants::pi<double>() * od(ddOutput::stat_entries::AEFF) / od(ddOutput::stat_entries::WAVE);
				simpleNumCompound<double>::write(out, this->version(), kaeff, 12, "K*AEFF=  ", "2*pi*aeff/lambda");

				if (icedb::io::ddscat::ddVersions::isVerWithin(version(), 72, 0))
					simpleNumCompound<double>::write(out, this->version(), _parent.parfile->nAmbient(), 8, "NAMBIENT=    ", "refractive index of ambient medium");
				//simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::NAMBIENT), 8, "NAMBIENT=    ", "refractive index of ambient medium");

				// Write refractive indices (plural)
				for (size_t i = 0; i < _parent.ms[_row].size(); ++i)
					refractive::write(out, this->version(), i + 1, _parent.ms[_row][i], k, od(ddOutput::stat_entries::D));

				simpleNumCompound<double>::write(out, this->version(), _parent.parfile->maxTol(), 9, "   TOL= ", " error tolerance for CCG method");

				std::vector<double> a(3);
				a[0] = s.TA1TF[0]; a[1] = s.TA1TF[1]; a[2] = s.TA1TF[2];
				ddAxisVec::write(out, this->version(), a, 1, frameType::TF);
				a[0] = s.TA2TF[0]; a[1] = s.TA2TF[1]; a[2] = s.TA2TF[2];
				ddAxisVec::write(out, this->version(), a, 2, frameType::TF);


				simpleNumCompound<size_t>::write(out, this->version(), _parent.s.navg, 5, "  NAVG= ", "(theta,phi) values used in comp. of Qsca,g");


				a[0] = od(ddOutput::stat_entries::TFKX); a[1] = od(ddOutput::stat_entries::TFKY); a[2] = od(ddOutput::stat_entries::TFKZ);
				ddAxisVec::write(out, this->version(), a, 0, frameType::TF);

				std::vector<std::complex<double> > iv(3);
				iv[0] = std::complex<double>(od(ddOutput::stat_entries::IPV1TFXR), od(ddOutput::stat_entries::IPV1TFXI));
				iv[1] = std::complex<double>(od(ddOutput::stat_entries::IPV1TFYR), od(ddOutput::stat_entries::IPV1TFYI));
				iv[2] = std::complex<double>(od(ddOutput::stat_entries::IPV1TFZR), od(ddOutput::stat_entries::IPV1TFZI));
				ddPolVec::write(out, this->version(), iv, 1, frameType::TF);
				iv[0] = std::complex<double>(od(ddOutput::stat_entries::IPV2TFXR), od(ddOutput::stat_entries::IPV2TFXI));
				iv[1] = std::complex<double>(od(ddOutput::stat_entries::IPV2TFYR), od(ddOutput::stat_entries::IPV2TFYI));
				iv[2] = std::complex<double>(od(ddOutput::stat_entries::IPV2TFZR), od(ddOutput::stat_entries::IPV2TFZI));
				ddPolVec::write(out, this->version(), iv, 2, frameType::TF);


				a[0] = od(ddOutput::stat_entries::TA1LFX); a[1] = od(ddOutput::stat_entries::TA1LFY); a[2] = od(ddOutput::stat_entries::TA1LFZ);
				ddAxisVec::write(out, this->version(), a, 1, frameType::LF);
				a[0] = od(ddOutput::stat_entries::TA2LFX); a[1] = od(ddOutput::stat_entries::TA2LFY); a[2] = od(ddOutput::stat_entries::TA2LFZ);
				ddAxisVec::write(out, this->version(), a, 2, frameType::LF);


				a[0] = s.LFK[0]; a[1] = s.LFK[1]; a[2] = s.LFK[2];
				ddAxisVec::write(out, this->version(), a, 0, frameType::LF);
				iv[0] = s.IPV1LF[0];
				iv[1] = s.IPV1LF[1];
				iv[2] = s.IPV1LF[2];
				ddPolVec::write(out, this->version(), iv, 1, frameType::LF);
				iv[0] = s.IPV2LF[0];
				iv[1] = s.IPV2LF[1];
				iv[2] = s.IPV2LF[2];
				ddPolVec::write(out, this->version(), iv, 2, frameType::LF);

				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::BETA), 7, " BETA =", "rotation of target around A1");
				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::THETA), 7, " THETA=", "angle between A1 and k");
				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::PHI), 7, "  PHI =", "rotation of A1 around k");

				out << endl;

				simpleNumRev<double>::write(out, this->version(), _parent.parfile->etasca(),
					"ETASCA = param. controlling # of scatt. dirs used to calculate <cos> etc.", 1, 6);

				//out << " Results averaged over " << 0 << " target orientations\n"
				//	<< "                   and    2 incident polarizations\n";


				// Write the odd table of Qsca and the others
				writeStatTable(out);

				// Write the P matrix
				writeMuellerDDSCAT(out);
			}

			void ddOriData::writeFML(std::ostream &out) const
			{
				// Write the file in the appropriate order
				using namespace std;
				using namespace ddOriDataParsers;
				//using namespace ddOutput::stat_entries;
				const auto od = _parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(_row, 0);
				const auto &s = _parent.s;
				//const auto &os = _parent.oridata_s.at(_row);
				//const auto &oi = _parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(_row, 0);

				version::write(out, _parent.s.version);
				simpleString::write(out, this->version(), _parent.s.target, "TARGET");
				simpleStringRev::write(out, this->version(), _parent.parfile->getSolnMeth(), "CCG method");
				simpleStringRev::write(out, this->version(), _parent.parfile->getCalpha(), "DDA method");
				simpleStringRev::write(out, this->version(), _parent.parfile->getShape(), "shape");
				simpleNumRev<size_t>::write(out, this->version(), _parent.s.num_dipoles, "NAT0 = number of dipoles");
				//double daeff = od(ddOutput::stat_entries::D) / od(ddOutput::stat_entries::AEFF);
				//simpleNumRev<double>::write(out, this->version(), daeff, "d/aeff for this target [d=dipole spacing]");
				//simpleNumRev<double>::write(out, this->version(), od(ddOutput::stat_entries::D), "d (physical units)");

				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::AEFF), 12, "  AEFF=  ", "effective radius (physical units)");
				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::WAVE), 12, "  WAVE=  ", "wavelength (in vacuo, physical units)");
				double k = 2. * boost::math::constants::pi<double>() / od(ddOutput::stat_entries::WAVE);
				double kaeff = 2. * boost::math::constants::pi<double>() * od(ddOutput::stat_entries::AEFF) / od(ddOutput::stat_entries::WAVE);
				simpleNumCompound<double>::write(out, this->version(), kaeff, 12, "K*AEFF=  ", "2*pi*aeff/lambda");

				if (icedb::io::ddscat::ddVersions::isVerWithin(version(), 72, 0))
					simpleNumCompound<double>::write(out, this->version(), _parent.parfile->nAmbient(), 8, "NAMBIENT=    ", "refractive index of ambient medium");
				//simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::NAMBIENT), 8, "NAMBIENT=    ", "refractive index of ambient medium");

				// Write refractive indices (plural)
				for (size_t i = 0; i < _parent.ms[_row].size(); ++i)
					refractive::write(out, this->version(), i + 1, _parent.ms[_row][i], k, od(ddOutput::stat_entries::D));

				simpleNumCompound<double>::write(out, this->version(), _parent.parfile->maxTol(), 9, "   TOL= ", " error tolerance for CCG method");


				simpleNumCompound<size_t>::write(out, this->version(), _parent.s.navg, 5, "  NAVG= ", "(theta,phi) values used in comp. of Qsca,g");


				std::vector<double> a(3);
				a[0] = s.TA1TF[0]; a[1] = s.TA1TF[1]; a[2] = s.TA1TF[2];
				ddAxisVec::write(out, this->version(), a, 1, frameType::TF);
				a[0] = s.TA2TF[0]; a[1] = s.TA2TF[1]; a[2] = s.TA2TF[2];
				ddAxisVec::write(out, this->version(), a, 2, frameType::TF);

				a[0] = od(ddOutput::stat_entries::TFKX); a[1] = od(ddOutput::stat_entries::TFKY); a[2] = od(ddOutput::stat_entries::TFKZ);
				ddAxisVec::write(out, this->version(), a, 0, frameType::TF);

				std::vector<std::complex<double> > iv(3);
				iv[0] = std::complex<double>(od(ddOutput::stat_entries::IPV1TFXR), od(ddOutput::stat_entries::IPV1TFXI));
				iv[1] = std::complex<double>(od(ddOutput::stat_entries::IPV1TFYR), od(ddOutput::stat_entries::IPV1TFYI));
				iv[2] = std::complex<double>(od(ddOutput::stat_entries::IPV1TFZR), od(ddOutput::stat_entries::IPV1TFZI));
				ddPolVec::write(out, this->version(), iv, 1, frameType::TF);
				iv[0] = std::complex<double>(od(ddOutput::stat_entries::IPV2TFXR), od(ddOutput::stat_entries::IPV2TFXI));
				iv[1] = std::complex<double>(od(ddOutput::stat_entries::IPV2TFYR), od(ddOutput::stat_entries::IPV2TFYI));
				iv[2] = std::complex<double>(od(ddOutput::stat_entries::IPV2TFZR), od(ddOutput::stat_entries::IPV2TFZI));
				ddPolVec::write(out, this->version(), iv, 2, frameType::TF);


				a[0] = s.LFK[0]; a[1] = s.LFK[1]; a[2] = s.LFK[2];
				ddAxisVec::write(out, this->version(), a, 0, frameType::LF);
				iv[0] = s.IPV1LF[0];
				iv[1] = s.IPV1LF[1];
				iv[2] = s.IPV1LF[2];
				ddPolVec::write(out, this->version(), iv, 1, frameType::LF);
				iv[0] = s.IPV2LF[0];
				iv[1] = s.IPV2LF[1];
				iv[2] = s.IPV2LF[2];
				ddPolVec::write(out, this->version(), iv, 2, frameType::LF);


				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::BETA), 7, " BETA =", "rotation of target around A1");
				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::THETA), 7, " THETA=", "angle between A1 and k");
				simpleNumCompound<double>::write(out, this->version(), od(ddOutput::stat_entries::PHI), 7, "  PHI =", "rotation of A1 around k");


				out << "     Finite target:\n"
					"     e_m dot E(r) = i*exp(ikr)*f_ml*E_inc(0)/(kr)\n"
					"     m=1 in scatt. plane, m=2 perp to scatt. plane\n";
				out << endl;

				// Write the f matrix
				writeF_DDSCAT(out);
			}

			std::shared_ptr<const ddScattMatrixConnector> ddOriData::getConnector() const
			{
				if (!_connector)
				{
					//const auto od = _parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(_row, 0);
					std::vector<std::complex<double> > v;
					v.push_back(_parent.s.IPV1LF[0]);
					v.push_back(_parent.s.IPV1LF[1]);
					v.push_back(_parent.s.IPV1LF[2]);
					v.push_back(_parent.s.IPV2LF[0]);
					v.push_back(_parent.s.IPV2LF[1]);
					v.push_back(_parent.s.IPV2LF[2]);
					_connector = ddScattMatrixConnector::fromVector(v);
				}
				return _connector;
			}

			void ddOriData::setConnector(std::shared_ptr<const ddScattMatrixConnector> cn)
			{
				_connector = cn;
			}


			void ddOriData::writeStatTable(std::ostream &out) const
			{
				// cannot be const as it changes if it is an avg file
				auto od = selectData();

				//const auto &os = _parent.oridata_s.at(_row);
				//const auto &oi = _parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(_row, 0);
				using namespace std;
				out << "          Qext       Qabs       Qsca      g(1)=<cos>  <cos^2>     Qbk       Qpha" << endl;
				out << " JO=1: ";
				out.width(11);
				for (size_t i = (size_t)ddOutput::stat_entries::QEXT1; i < (size_t)ddOutput::stat_entries::QEXT2; i++)
					out << "\t" << od(i);
				out << endl;
				out.width(0);
				out << " JO=2: ";
				out.width(11);
				for (size_t i = (size_t)ddOutput::stat_entries::QEXT2; i < (size_t)ddOutput::stat_entries::QEXTM; i++)
					out << "\t" << od(i);
				out << endl;
				out.width(0);
				out << " mean: ";
				out.width(11);
				for (size_t i = (size_t)ddOutput::stat_entries::QEXTM; i < (size_t)ddOutput::stat_entries::QPOL; i++)
					out << "\t" << od(i);
				out << endl;
				out.width(0);
				out << " Qpol= " << od(ddOutput::stat_entries::QPOL) <<
					"                                                  " <<
					"dQpha= ";
				out.width(11);
				out << od(ddOutput::stat_entries::DQPHA) << endl;

				out << "         Qsca*g(1)   Qsca*g(2)   Qsca*g(3)   iter  mxiter  Nsca\n";
				out << " JO=1: ";
				out.width(11);
				for (size_t i = (size_t)ddOutput::stat_entries::QSCAG11; i < (size_t)ddOutput::stat_entries::QSCAG12; i++)
					out << "\t" << od(i);
				out << endl;
				out.width(0);
				out << " JO=2: ";
				out.width(11);
				for (size_t i = (size_t)ddOutput::stat_entries::QSCAG12; i < (size_t)ddOutput::stat_entries::QSCAG1M; i++)
					out << "\t" << od(i);
				out << endl;
				out.width(0);
				out << " mean: ";
				for (size_t i = (size_t)ddOutput::stat_entries::QSCAG1M; i < (size_t)ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES; i++)
					out << "\t" << od(i);
				out << endl;
				out.width(0);
			}

			void ddOriData::readStatTable(std::istream &in)
			{
				auto od = selectData();
				//auto &os = _parent.oridata_s.at(_row);
				//auto &oi = _parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(_row, 0);

				using namespace std;
				string line;
				// First line was detected by the calling function, so it is outside of the istream now.
				//std::getline(in,line); // "          Qext       Qabs       Qsca      g(1)=<cos>  <cos^2>     Qbk       Qpha" << endl;
				in >> line; // " JO=1: ";
				for (size_t i = ddOutput::stat_entries::QEXT1; i < (size_t)ddOutput::stat_entries::QEXT2; i++)
					in >> od(i);
				in >> line; // " JO=2: ";
				for (size_t i = (size_t)ddOutput::stat_entries::QEXT2; i < (size_t)ddOutput::stat_entries::QEXTM; i++)
					in >> od(i);
				in >> line; // " mean: ";
				for (size_t i = (size_t)ddOutput::stat_entries::QEXTM; i < (size_t)ddOutput::stat_entries::QPOL; i++)
					in >> od(i);
				in >> line // " Qpol= " 
					>> od(ddOutput::stat_entries::QPOL) >> line; // "dQpha=";
				in >> od(ddOutput::stat_entries::DQPHA);

				std::getline(in, line); // "         Qsca*g(1)   Qsca*g(2)   Qsca*g(3)   iter  mxiter  Nsca";
				std::getline(in, line);
				in >> line; // " JO=1: ";
				for (size_t i = (size_t)ddOutput::stat_entries::QSCAG11; i < (size_t)ddOutput::stat_entries::QSCAG12; i++)
					in >> od(i);
				in >> line; // " JO=2: ";
				for (size_t i = (size_t)ddOutput::stat_entries::QSCAG12; i < (size_t)ddOutput::stat_entries::QSCAG1M; i++)
					in >> od(i);
				in >> line; // " mean: ";
				for (size_t i = (size_t)ddOutput::stat_entries::QSCAG1M; i < (size_t)ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES; i++)
					in >> od(i);
				std::getline(in, line);
			}

			double ddOriData::guessTemp(size_t dielIndex) const
			{
				const auto od = selectData();
				return -999;
				//return rtmath::refract::guessTemp(freq(), M(dielIndex));
			}

			void ddOriData::writeF_DDSCAT(std::ostream &out) const
			{
				using namespace std;
				out << " theta   phi  Re(f_11)   Im(f_11)   Re(f_21)   Im(f_21)   Re(f_12)   Im(f_12)   Re(f_22)   Im(f_22)";
				for (auto it = _scattMatricesRaw.begin(); it != _scattMatricesRaw.end(); ++it)
				{
					if ((*it).id() != scattMatrixType::F) continue;
					//std::shared_ptr<const ddscat::ddScattMatrixF> sf(
					//	boost::dynamic_pointer_cast<const ddscat::ddScattMatrixF>(*it));
					out << endl;
					out.width(6);
					// it->first crds ordering is freq, phi, theta
					out << (*it).theta() << "\t";
					out << (*it).phi();
					out.width(11);
					ddScattMatrix::FType f = it->getF();

					for (size_t j = 0; j < 2; j++)
						for (size_t i = 0; i < 2; i++)
						{
							// Note the reversed coordinates. This matches ddscat.
							out << "\t" << f(i, j).real();
							out << "\t" << f(i, j).imag();
						}
				}
			}

			void ddOriData::writeS(std::ostream &out) const
			{
				using namespace std;
				out << " theta   phi  Re(S_11)   Im(S_11)   Re(S_21)   Im(S_21)   Re(S_12)   Im(S_12)   Re(f_22)   Im(f_22)";
				for (auto it = _scattMatricesRaw.begin(); it != _scattMatricesRaw.end(); ++it)
				{
					if ((*it).id() != scattMatrixType::F) continue;
					//std::shared_ptr<const ddscat::ddScattMatrixF> sf(
					//	boost::dynamic_pointer_cast<const ddscat::ddScattMatrixF>(*it));
					out << endl;
					out.width(6);
					// it->first crds ordering is freq, phi, theta
					out << (*it).theta() << "\t";
					out << (*it).phi();
					out.width(11);
					ddScattMatrix::FType s = it->getS();
					for (size_t j = 0; j < 2; j++)
						for (size_t i = 0; i < 2; i++)
						{
							// Note the reversed coordinates. This matches ddscat.
							out << "\t" << s(i, j).real();
							out << "\t" << s(i, j).imag();
						}
				}
			}

			/// Provides default Mueller matrix entries to write.
			const ddOriData::mMuellerIndices& ddOriData::mMuellerIndicesDefault()
			{
				static mMuellerIndices mi;
				/// \todo Make threadable
				if (!mi.size())
				{
					mi.push_back(std::pair<size_t, size_t>(0, 0));
					mi.push_back(std::pair<size_t, size_t>(0, 1));
					mi.push_back(std::pair<size_t, size_t>(1, 0));
					mi.push_back(std::pair<size_t, size_t>(1, 1));
					mi.push_back(std::pair<size_t, size_t>(2, 0));
					mi.push_back(std::pair<size_t, size_t>(3, 0));
				}
				return mi;
			}

			void ddOriData::writeMuellerDDSCAT(std::ostream &out, const mMuellerIndices &mi) const
			{
				using namespace std;
				out << "            Mueller matrix elements for selected scattering directions in Lab Frame" << endl;
				out << " theta    phi    Pol.    "; // "S_11        S_12        S_21       S_22       S_31       S_41\n";
				for (auto it = mi.begin(); it != mi.end(); ++it)
				{
					out << "S_" << (it->first + 1) << (it->second + 1);
					auto ot = it;
					ot++;
					if (ot != mi.end()) out << "        ";
				}
				out << "\n";

				for (auto it = _scattMatricesRaw.begin(); it != _scattMatricesRaw.end(); ++it)
				{
					//std::shared_ptr<const ddscat::ddScattMatrix> sf(*it);
					out << endl;
					//out.width(6);
					using namespace std;
					//out.width(6);
					out << fixed << right << showpoint << setprecision(2) << setw(6) << (*it).theta() << " ";
					out << setw(6) << (*it).phi() << " ";
					//out.width(8);
					out << setprecision(5) << setw(8) << it->polLin() << " ";
					//out.width(10);
					ddScattMatrix::PnnType p = it->mueller();
					for (auto ot = mi.begin(); ot != mi.end(); ++ot)
					{
						out << " " << scientific << setprecision(4) << setw(10) << p(ot->first, ot->second);
					}
				}
			}

			size_t ddOriData::numMat() const { return _scattMatricesRaw.size(); }

			bool ddOriData::operator<(const ddOriData &rhs) const
			{
				const auto od = _parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(_row, 0);
				//const auto &os = _parent.oridata_s.at(_row);
				//const auto &oi = _parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(_row, 0);

				const auto &rod = rhs._parent.oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(rhs._row, 0);
				//const auto &ros = rhs._parent.oridata_s.at(rhs._row);
				//const auto &roi = rhs._parent.oridata_i.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_INTS>(rhs._row, 0);


#define CHECKD(x) if( od(x) != rod(x)) return od(x) < rod(x);
#define CHECKI(x) if( oi(x) != roi(x)) return oi(x) < roi(x);
			//CHECKD(ddOutput::stat_entries::DOWEIGHT);
				CHECKD(ddOutput::stat_entries::WAVE);
				CHECKD(ddOutput::stat_entries::AEFF);
				if (_parent.s.num_dipoles != rhs._parent.s.num_dipoles) return _parent.s.num_dipoles < rhs._parent.s.num_dipoles;
				//CHECKI(ddOutput::stat_entries::NUM_DIPOLES);
				CHECKD(ddOutput::stat_entries::BETA);
				CHECKD(ddOutput::stat_entries::THETA);
				CHECKD(ddOutput::stat_entries::PHI);
#undef CHECKD
#undef CHECKI

				return false;
			}

			std::complex<double> ddOriData::M(size_t dielIndex) const
			{
				if (!isAvg)
				{
					if (_parent.ms[_row].size() > dielIndex) return _parent.ms[_row][dielIndex];
				}
				else {
					if (_parent.avgdata.avg_ms.size() > dielIndex) return _parent.avgdata.avg_ms[dielIndex];
				}
				ICEDB_throw(icedb::error::error_types::xArrayOutOfBounds);
				return std::complex<double>(0, 0); // needed to suppress _parent.ms[_row]vc warning
			}
			void ddOriData::M(const std::complex<double>& m, size_t dielIndex)
			{
				if (!isAvg)
				{
					if (_parent.ms[_row].size() < dielIndex) _parent.ms[_row].resize(dielIndex + 1);
					_parent.ms[_row][dielIndex] = m;
				}
				else {
					if (_parent.avgdata.avg_ms.size() < dielIndex) _parent.avgdata.avg_ms.resize(dielIndex + 1);
					_parent.avgdata.avg_ms[dielIndex] = m;
				}
			}

			size_t ddOriData::numM() const
			{
				if (!isAvg) return _parent.ms[_row].size();
				else return _parent.avgdata.avg_ms.size();
			}



		}
	}
}

