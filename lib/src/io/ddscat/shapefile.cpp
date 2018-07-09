#include "Stdafx-ddscat.h"
#pragma warning( disable : 4996 ) // -D_SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/weak_ptr.hpp>

#include <Ryan_Debug/debug.h>
#include <Ryan_Debug/macros.h>
#include <Ryan_Debug/hash.h>
#include <Ryan_Debug/splitSet.h>
#include <Ryan_Debug/registry.h>
#include <Ryan_Debug/Serialization.h>
#include <Ryan_Debug/error.h>
#include "../rtmath/error/debug.h"
#include "../rtmath/Voronoi/Voronoi.h"
#include "../rtmath/ddscat/shapefile.h"
#include "shapestats_private.h"

namespace {
	std::set<std::string> mtypes;
	std::mutex mlock_shp;

	std::map<std::string, 
		boost::weak_ptr<const ::rtmath::ddscat::shapefile::shapefile> > loadedShapes;
	//std::map<rtmath::HASH_t, boost::shared_ptr<shapefile> > stubShapes;
}

namespace Ryan_Debug {
	namespace registry {
		template struct IO_class_registry_writer
			< ::rtmath::ddscat::shapefile::shapefile > ;

		template struct IO_class_registry_reader
			< ::rtmath::ddscat::shapefile::shapefile > ;

		template class usesDLLregistry <
			::rtmath::ddscat::shapefile::shapefile_IO_output_registry,
			IO_class_registry_writer<::rtmath::ddscat::shapefile::shapefile> > ;

		template class usesDLLregistry <
			::rtmath::ddscat::shapefile::shapefile_IO_input_registry,
			IO_class_registry_reader<::rtmath::ddscat::shapefile::shapefile> > ;

		template class usesDLLregistry <
			::rtmath::ddscat::shapefile::shapefile_query_registry,
			::rtmath::ddscat::shapefile::shapefile_db_registry > ;

	}

	namespace io {
		template <>
		boost::shared_ptr<::rtmath::ddscat::shapefile::shapefile> customGenerator()
		{
			boost::shared_ptr<::rtmath::ddscat::shapefile::shapefile> res
				(new ::rtmath::ddscat::shapefile::shapefile);
			return res;
		}
	}
}
namespace rtmath {
	namespace ddscat {
		namespace shapefile {

			implementsDDSHP::implementsDDSHP() :
				Ryan_Debug::io::implementsIObasic<shapefile, shapefile_IO_output_registry,
				shapefile_IO_input_registry, shapefile_Standard>(shapefile::writeDDSCAT, shapefile::readDDSCAT, known_formats())
			{}

			const std::set<std::string>& implementsDDSHP::known_formats()
			{
				// Moved to hidden file scope to avoid race condition
				//static std::set<std::string> mtypes;
				//static std::mutex mlock;
				// Prevent threading clashes
				{
					std::lock_guard<std::mutex> lck(mlock_shp);
					if (!mtypes.size())
					{
						mtypes.insert(".shp");
						mtypes.insert("shape.txt");
						mtypes.insert(".dat");
						mtypes.insert("shape.dat");
						mtypes.insert("target.out");
					}
					if (Ryan_Debug::io::TextFiles::serialization_handle::compressionEnabled())
					{
						std::string sctypes;
						std::set<std::string> ctypes;
						Ryan_Debug::serialization::known_compressions(sctypes, ".shp");
						Ryan_Debug::serialization::known_compressions(sctypes, ".dat");
						Ryan_Debug::serialization::known_compressions(sctypes, "shape.txt");
						Ryan_Debug::serialization::known_compressions(sctypes, "shape.dat");
						Ryan_Debug::serialization::known_compressions(sctypes, "target.out");
						Ryan_Debug::splitSet::splitSet(sctypes, ctypes);
						for (const auto & t : ctypes)
							mtypes.emplace(t);
					}
				}
				return mtypes;
			}

			shapefile::shapefile() { _init(); }
			shapefile::~shapefile() { }

//#if _MSC_FULL_VER
			shapefile& shapefile::operator=(const shapefile& rhs)
			{
				if (this == &rhs) return *this;
#define cp(x) this->x = rhs.x;
				cp(_localhash);
				cp(filename);
				cp(ingest_timestamp);
				cp(ingest_hostname);
				cp(ingest_username);
				cp(ingest_rtmath_version);
				cp(latticePts);
				cp(latticePtsStd);
				cp(latticePtsNorm);
				cp(latticePtsRi);
				cp(latticeIndex);
				cp(tags);
				cp(latticeExtras);
				cp(numPoints);
				cp(standardD);
				cp(Dielectrics);
				cp(desc);
				cp(a1);
				cp(a2);
				cp(a3);
				cp(d);
				cp(x0);
				cp(xd);
				cp(mins);
				cp(maxs);
				cp(means);

#undef cp
				return *this;
			}
//#endif

			bool shapefile::isHashStored(const std::string &hash)
			{
				if (loadedShapes.count(hash)) return true;
				return false;
			}

			bool shapefile::isHashStored(const Ryan_Debug::hash::HASH_t &hash) { return isHashStored(hash.string()); }

			void shapefile::registerHash() const
			{
				boost::shared_ptr<const shapefile> ptr = this->shared_from_this();
				boost::weak_ptr<const shapefile> wshp(ptr);
				loadedShapes[this->_localhash.string()] = wshp;
				//loadedShapes.emplace(std::pair < std::string,
				//boost::weak_ptr<const ::rtmath::ddscat::shapefile::shapefile> > (
				//this->_localhash.string(), wshp));
			}

			boost::shared_ptr<shapefile> shapefile::generate(const std::string &filename)
			{
				boost::shared_ptr<shapefile> res(new shapefile);
				res->read(filename);
				res->filename = filename;
				return res;
			}

			boost::shared_ptr<shapefile> shapefile::generate(std::istream &in)
			{
				boost::shared_ptr<shapefile> res(new shapefile(in));
				return res;
			}

			boost::shared_ptr<shapefile> shapefile::generate()
			{
				boost::shared_ptr<shapefile> res(new shapefile);
				return res;
			}

			boost::shared_ptr<shapefile> shapefile::generate(boost::shared_ptr<const shapefile> p)
			{
				boost::shared_ptr<shapefile> res(new shapefile);
				*res = *p;
				return res;
			}

			/*
			shapefile::shapefile(const std::string &filename)
			{
				_init();
				read(filename);
				this->filename = filename;
			}
			*/

			shapefile::shapefile(std::istream &in)
			{
				_init();
				std::ostringstream so;
				boost::iostreams::copy(in, so);
				std::string s;
				s = so.str();
				this->_localhash = Ryan_Debug::hash::HASH(s.c_str(), (int)s.size());

				//std::istringstream ss_unc(s);
				readString(s);
			}

			void shapefile::_init()
			{
				numPoints = 0;
				standardD = 0;

				// Add the ingest tags. If using a copy constructor, then these get overridden.
				using namespace boost::posix_time;
				using namespace boost::gregorian;
				ptime now = second_clock::local_time();
				ingest_timestamp = to_iso_string(now);
				ingest_hostname = Ryan_Debug::getHostname();
				ingest_username = Ryan_Debug::getUsername();
				ingest_rtmath_version = rtmath::debug::rev();

				//filename = "";
				//::rtmath::io::Serialization::implementsSerialization<
				//	shapefile, shapefile_IO_output_registry, 
				//	shapefile_IO_input_registry, shapefile_serialization>::set_sname("rtmath::ddscat::shapefile::shapefile");
			}

			void shapefile::setHash(const Ryan_Debug::hash::HASH_t &h)
			{
				_localhash = h;
			}

			Ryan_Debug::hash::HASH_t shapefile::hash() const
			{
				if (_localhash.lower) return _localhash;
				return rehash();
			}

			Ryan_Debug::hash::HASH_t shapefile::rehash() const
			{
				if (numPoints)
				{
					std::string res;
					std::ostringstream out;
					print(out);
					res = out.str();
					this->_localhash = Ryan_Debug::hash::HASH(res.c_str(), (int)res.size());
				}
				return this->_localhash;
			}

			void shapefile::info(std::ostream &out) const
			{
				// Print hash and all tags. Usually called after loading 
				// supplementary information from a database.
				out << "Shape: " << this->hash().lower << std::endl;
				out << "Description: " << desc << std::endl;
				out << "# points: " << numPoints << std::endl;
				out << "Filename: " << this->filename << std::endl;
				out << "Ingested on: " << ingest_timestamp << std::endl;
				out << "Ingested host: " << ingest_hostname << std::endl;
				out << "Ingested username: " << ingest_username << std::endl;
				out << "Ingested rtmath: " << ingest_rtmath_version << std::endl;
				out << "Dipole spacing: " << standardD << std::endl;
				out << "a1: " << a1.transpose() << std::endl
					<< "a2: " << a2.transpose() << std::endl
					<< "a3: " << a3.transpose() << std::endl
					<< "d: " << d.transpose() << std::endl
					<< "x0: " << x0.transpose() << std::endl
					<< "xd: " << xd.transpose() << std::endl
					<< "mins: " << mins.transpose() << std::endl
					<< "maxs: " << maxs.transpose() << std::endl
					<< "means: " << means.transpose() << std::endl;

				out << "Tags:" << std::endl;
				for (const auto &t : tags)
					out << "\t" << t.first << " - " << t.second << std::endl;
			}

			void shapefile::loadHashLocal()
			{
				if (latticePts.rows()) return; // Already loaded
				const auto origtags = tags;
				auto sD = standardD;

				// Tags are stored this way because incomplete entries usually come from databases.
				loadHashLocal(hash());
				
				if (sD) standardD = sD;
				for (const auto &tag : origtags)
					//if (!tags.count(tag.first)) 
					tags[tag.first] = tag.second;
			}

			boost::shared_ptr<const shapefile> shapefile::loadHash(
				const Ryan_Debug::hash::HASH_t &hash)
			{ return loadHash(boost::lexical_cast<std::string>(hash.lower)); }

			void shapefile::loadHashLocal(
				const Ryan_Debug::hash::HASH_t &hash)
			{
				loadHashLocal(boost::lexical_cast<std::string>(hash.lower));
			}

			
			void shapefile::readHeaderOnly(const std::string &str)
			{
				readString(str, true);
			}

			void shapefile::readString(const std::string &in, bool headerOnly)
			{
				size_t headerEnd = 0;
				readHeader(in.c_str(), headerEnd);
				if (!headerOnly)
					readContents(in.c_str(), headerEnd);
			}

			void shapefile::resize(size_t n)
			{

				latticePts.conservativeResize(n, 3);
				latticePtsRi.conservativeResize(n, 3);
				latticePtsStd.conservativeResize(n, 3);
				latticePtsNorm.conservativeResize(n, 3);
				latticeIndex.conservativeResize(n);
				// I want to keep the point indices if resizing to
				// something smaller.
				if (n > numPoints)
				{
					auto newinds = latticeIndex.block(numPoints, 0, n - numPoints, 1);
					newinds.setLinSpaced(n - numPoints,numPoints,(int) n);
				}
				numPoints = n;
				//for (auto &extra : latticeExtras)
				//	extra.second->conservativeResize(numPoints, 3);
			}

			void shapefile::readHeader(const char* in, size_t &headerEnd)
			{
				using namespace std;

				// Do header processing using istreams.
				// The previous method used strings, but this didn't work with compressed reads.
				//size_t &pend = headerEnd;
				const char* pend = in;
				const char* pstart = in;

				// The header is seven lines long
				for (size_t i = 0; i < 7; i++)
				{
					pstart = pend;
					pend = strchr(pend, '\n');
					pend++; // Get rid of the newline
					//pend = in.find_first_of("\n", pend+1);
					string lin(pstart, pend - pstart - 1);
					if (*(lin.rbegin()) == '\r') lin.pop_back();
					//std::getline(in,lin);

					size_t posa = 0, posb = 0;
					Eigen::Array3f *v = nullptr;
					switch (i)
					{
					case 0: // Title line
						desc = lin;
						break;
					case 1: // Number of dipoles
					{
								// Seek to first nonspace character
								posa = lin.find_first_not_of(" \t\n", posb);
								// Find first space after this position
								posb = lin.find_first_of(" \t\n", posa);
								size_t len = posb - posa;
								numPoints = Ryan_Debug::macros::m_atoi<size_t>(&(lin.data()[posa]), len);
					}
						break;
					case 6: // Junk line
					default:
						break;
					case 2: // a1
					case 3: // a2
					case 4: // d
					case 5: // x0
						// These all have the same structure. Read in three doubles, then assign.
					{
								if (2 == i) v = &a1;
								if (3 == i) v = &a2;
								if (4 == i) v = &d;
								if (5 == i) v = &x0;
								for (size_t j = 0; j < 3; j++)
								{
									// Seek to first nonspace character
									posa = lin.find_first_not_of(" \t\n,", posb);
									// Find first space after this position
									posb = lin.find_first_of(" \t\n,", posa);
									size_t len = posb - posa;
									(*v)(j) = Ryan_Debug::macros::m_atof<float>(&(lin.data()[posa]), len);
								}
					}
						break;
					}
				}

				headerEnd = (pend - in) / sizeof(char);
				resize(numPoints);
			}

			void shapefile::writeDDSCAT(const boost::shared_ptr<const shapefile> s, std::ostream &out, std::shared_ptr<Ryan_Debug::registry::IO_options>)
			{ s->print(out); }

			void shapefile::readDDSCAT(boost::shared_ptr<shapefile> s, std::istream &in, std::shared_ptr<Ryan_Debug::registry::IO_options> opts)
			{
				std::ostringstream so;
				boost::iostreams::copy(in, so);
				std::string str = so.str();
				bool headerOnly = opts->getVal<bool>("headerOnly", false);
				s->readString(str, headerOnly);
				
				// This is a standard ddscat file, so add the ingest tags
				s->filename = opts->filename();
				using namespace boost::posix_time;
				using namespace boost::gregorian;
				ptime now = second_clock::local_time();
				s->ingest_timestamp = to_iso_string(now);
				s->ingest_hostname = Ryan_Debug::getHostname();
				s->ingest_username = Ryan_Debug::getUsername();
				s->ingest_rtmath_version = rtmath::debug::rev();

			}

			/*
			void shapefile::writeVTK(const std::string &fname) const
			{
				rtmath::ddscat::convexHull hull(latticePtsStd);
				hull.writeVTKraw(fname);
			}
			*/

			convolutionCellInfo::convolutionCellInfo() :
				initDiel(0), numFilled(0), numTotal(0), index(0)
			{
				s.setZero(); crd.setZero();
			}

			boost::shared_ptr<Voronoi::VoronoiDiagram> shapefile::generateVoronoi(
				const std::string &name,
				std::function < boost::shared_ptr<Voronoi::VoronoiDiagram>(
				const Eigen::Array3f&, const Eigen::Array3f&,
				const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>&, const char*)> f) const
			{
				//if (voronoi_diagrams.count(name))
				//	return voronoi_diagrams[name];
				// Attempt to load the voronoi diagram from the hash database.
				boost::shared_ptr<Voronoi::VoronoiDiagram> res;
				std::cerr << "Requesting voronoi diagrams for " << hash().string() << std::endl;
				res = Voronoi::VoronoiDiagram::loadHash(hash());
				if (!res) {
					if (stats::doVoronoi) {
						std::cerr << " Diagrams not found. Generating." << std::endl;
						res = f(mins, maxs, latticePts, ""); // note that no particular plugin is specified here.
						res->setHash(this->hash());
					} else {
						std::cerr << " Diagrams not found, and calculation prohibited." << std::endl;
					}
				}

				//voronoi_diagrams[name] = res;
				return res;
			}

			boost::shared_ptr<const shapefile> shapefile::slice(
				int axis, float intercept,
				float tolerance
				) const
			{
				boost::shared_ptr<shapefile> res(new shapefile);

				res->a1 = a1;
				res->a2 = a2;
				res->a3 = a3;
				res->d = d;
				res->desc = desc;
				res->Dielectrics = Dielectrics;
				res->filename = filename;
				res->xd = xd;
				res->resize(numPoints);
				if (axis < 0 || axis >= 3)
					RDthrow(Ryan_Debug::error::xBadInput())
						<< Ryan_Debug::error::otherErrorText("axis must be 0, 1 or 2");
				if (tolerance <= 0)
					RDthrow(Ryan_Debug::error::xBadInput())
						<< Ryan_Debug::error::otherErrorText("tolerance must be > 0");

				size_t point = 0;
				for (size_t i = 0; i < numPoints; ++i)
				{
					auto basem = latticePts.block<1, 3>(i, 0);
					auto basei = latticePtsRi.block<1, 3>(i, 0);
					auto bases = latticePtsStd.block<1, 3>(i, 0);
					auto basen = latticePtsNorm.block<1, 3>(i, 0);
					float planedist = basem(axis) - intercept;
					if (std::abs(planedist) > tolerance) continue;
					auto crdsm = res->latticePts.block<1, 3>(point, 0);
					auto crdsi = res->latticePtsRi.block<1, 3>(point, 0);
					auto crdss = res->latticePtsStd.block<1, 3>(point, 0);
					auto crdsn = res->latticePtsNorm.block<1, 3>(point, 0);
					crdsm = basem;
					crdsi = basei;
					crdss = bases;
					crdsn = basen;
					res->latticeIndex(point) = latticeIndex(i);
					point++;
				}
				
				// Shrink the data store
				res->resize(point);

				res->recalcStats();
				// Rescale x0 to point to the new center
				//res->x0 = x0 / Eigen::Array3f((float)dx, (float)dy, (float)dz);
				// Cannot just rescale because of negative coordinates.
				res->x0 = res->means;

				return res;
			}

			boost::shared_ptr<const Eigen::Array<float, Eigen::Dynamic, 4> >
			shapefile::sliceAll(int axis, int numbins) const {
				if (axis < 0 || axis >= 3)
					RDthrow(Ryan_Debug::error::xBadInput())
						<< Ryan_Debug::error::otherErrorText("axis must be 0, 1 or 2");

				boost::shared_ptr<Eigen::Array<float, Eigen::Dynamic, 4> > res(
					new Eigen::Array<float, Eigen::Dynamic, 4>);
				//std::cerr << mins << "\n" << maxs << std::endl;
				auto minBase = mins(axis), maxBase = maxs(axis);
				int rows = maxBase - minBase + 1;
				if (numbins > 0) rows = numbins;
				res->resize(rows, 4);
				res->setZero();
				Eigen::Array<float,  Eigen::Dynamic, 1> ls;
				ls.resize(rows,1);
				ls.setLinSpaced(rows,minBase,maxBase);
				res->matrix().block(0,0,rows,1) = ls;
				ls.setLinSpaced(rows,-0.5,0.5);
				res->matrix().block(0,1,rows,1) = ls;

				for (size_t i = 0; i < numPoints; ++i)
				{
					auto basem = latticePts.block<1, 3>(i, 0);
					int bin = rows * (basem(axis) - minBase) / (maxBase - minBase);
					if (bin < 0) bin = 0;
					if (bin >= rows) bin = rows - 1;
					auto outbin = res->matrix().block(bin, 0, 1, 4);
					outbin(0,2) = outbin(0,2) + 1;
					outbin(0,3) = outbin(0,3) + (float) ( (float) rows / (float) numPoints);
				}

				return res;
			}

			boost::shared_ptr<const shapefile> shapefile::decimate(
				decimationFunction dFunc,
				size_t dx, size_t dy, size_t dz
				) const
			{
				boost::shared_ptr<shapefile> res(new shapefile);

				auto span = (maxs - mins).cast<int>();
				// Note: shapefile also has an element (d), which is masked here.
				Eigen::Array3i d; d << dx, dy, dz;
				Eigen::Array3i rs = (span / d) + 1;

				auto getIndex = [&](const Eigen::Array3f p) -> size_t {
					size_t index = 0;
					Eigen::Array3i s = (p - mins).cast<int>() / d;
					index = (s(0) * (rs(1) * rs(2))) + (s(1) * rs(2)) + s(2);
					return index;
				};
				auto getCrds = [&](size_t index) -> Eigen::Array3i {
					Eigen::Array3i res;
					res(0) = index / (rs(1)*rs(2));
					index -= res(0)*rs(1)*rs(2);
					res(1) = index / rs(2);
					index -= res(1)*rs(2);
					res(2) = index;
					return res;
				};

				std::vector<convolutionCellInfo> vals(rs.prod());
				for (auto &v : vals)
				{
					v.initDiel = 1;
					v.numTotal = d.prod();
					v.s = d;
				}

				// Iterate over all points and bin into the appropriate set
				for (size_t i = 0; i < numPoints; ++i)
				{
					auto crdsm = latticePts.block<1, 3>(i, 0);
					Eigen::Array3f pt = crdsm.array();
					size_t index = getIndex(pt);
					auto &v = vals.at(index);
					v.numFilled++;
					v.index = index;
					v.crd = pt;
				}

				// Count the decimated points with nonzero values
				//size_t num = vals.size() - std::count_if(vals.begin(), vals.end(), [&]
				//	(const convolutionCellInfo& c){if (c.numFilled == 0) return true; return false; });
				//	NOTE: replaced with this. Resize at end.
				size_t num = vals.size();

				res->a1 = a1;
				res->a2 = a2;
				res->a3 = a3;
				res->d = d.cast<float>();
				res->desc = desc;
				res->Dielectrics = Dielectrics;
				res->filename = filename;
				res->xd = xd;

				res->resize(num);

				size_t dielMax = 0;
				// Set the decimated values
				size_t point = 0;
				for (size_t i = 0; i < vals.size(); ++i)
				{
					size_t diel = dFunc(vals.at(i),shared_from_this());
					if (diel == 0) continue;
					auto t = getCrds(i);
					auto crdsm = res->latticePts.block<1, 3>(point, 0);
					auto crdsi = res->latticePtsRi.block<1, 3>(point, 0);
					crdsm = t.cast<float>();
					crdsi.setConstant((float)diel);
					if (dielMax < diel) dielMax = diel;
					point++;
				}
				
				// Shrink the data store
				if (point < num)
					res->resize(point);

				// Set the dielectrics
				res->Dielectrics.clear();
				for (size_t i = 1; i <= dielMax; ++i)
					res->Dielectrics.insert(i);

				res->recalcStats();
				// Rescale x0 to point to the new center
				//res->x0 = x0 / Eigen::Array3f((float)dx, (float)dy, (float)dz);
				// Cannot just rescale because of negative coordinates.
				res->x0 = res->means;


				return res;
			}

			boost::shared_ptr<const shapefile> shapefile::
				enhance(size_t dx, size_t dy, size_t dz) const
			{
				boost::shared_ptr<shapefile> res(new shapefile);
				auto span = (maxs - mins).cast<int>();
				Eigen::Array3i d; d << dx, dy, dz;
				auto rs = (span / d) + 1;

				// Resize the resultant shapefile based on the new scale
				size_t nd = d.prod();
				res->resize(nd * numPoints);

				res->a1 = a1;
				res->a2 = a2;
				res->a3 = a3;
				res->d = d.cast<float>();
				res->desc = desc;
				res->Dielectrics = Dielectrics;
				res->filename = filename;
				res->xd = xd;
				// Rescale x0 to point to the new center
				res->x0 = x0 * d.cast<float>();

				// Iterate over all points and bin into the appropriate set
				for (size_t i = 0; i < numPoints; ++i)
				{
					auto crdsm = latticePts.block<1, 3>(i, 0);
					auto crdsi = latticePtsRi.block<1, 3>(i, 0);

					for (size_t x = 0; x < d(0); ++x)
					for (size_t y = 0; y < d(1); ++y)
					for (size_t z = 0; z < d(2); ++z)
					{
						size_t j = (nd * i) + z + (d(2)*y) + (d(2)*d(1)*x);
						auto rdsm = res->latticePts.block<1, 3>(j, 0);
						auto rdsi = res->latticePtsRi.block<1, 3>(j, 0);
						rdsm(0) = crdsm(0) + x;
						rdsm(1) = crdsm(1) + y;
						rdsm(2) = crdsm(2) + z;
						rdsi = crdsi;
					}
				}

				res->recalcStats();

				return res;
			}

			void shapefile::fixStats()
			{
				x0 = means;
				xd = x0 * d;
			}



			boost::shared_ptr<const shapefile> shapefile::loadHash(
				const std::string &hash)
			{
				if (loadedShapes.count(hash))
				{
					auto wptr = loadedShapes.at(hash);
					if (!wptr.expired()) return wptr.lock();
				}

				boost::shared_ptr<shapefile> res(new shapefile);

				using boost::filesystem::path;
				using boost::filesystem::exists;

				std::shared_ptr<Ryan_Debug::registry::IOhandler> sh;
				std::shared_ptr<Ryan_Debug::registry::IO_options> opts; // No need to set - it gets reset by findHashObj

				if (Ryan_Debug::hash::hashStore::findHashObj(hash, "shape.hdf5", sh, opts, "rtmath"))
				{
					opts->setVal<std::string>("key", hash);
					res = boost::shared_ptr<shapefile>(new shapefile);
					res->readMulti(sh, opts);
				}

				res->registerHash();
				return res;
			}

			void shapefile::loadHashLocal(
				const std::string &hash)
			{
				if (loadedShapes.count(hash))
				{
					auto wptr = loadedShapes.at(hash);
					if (!wptr.expired())
					{
						*this = *(wptr.lock());
						return;
					}
				}

				using boost::filesystem::path;
				using boost::filesystem::exists;

				std::shared_ptr<Ryan_Debug::registry::IOhandler> sh;
				std::shared_ptr<Ryan_Debug::registry::IO_options> opts; // No need to set - it gets reset by findHashObj

				if (Ryan_Debug::hash::hashStore::findHashObj(hash, "shape.hdf5", sh, opts, "rtmath"))
				{
					opts->setVal<std::string>("key", hash);
					readMulti(sh, opts);
				}
				else {
					RDthrow(Ryan_Debug::error::xMissingHash())
						<< Ryan_Debug::error::hash(hash)
						<< Ryan_Debug::error::hashType("shapefile");
				}

				this->registerHash();
			}

		}
	}
}



std::ostream & operator<<(std::ostream &stream, const rtmath::ddscat::shapefile::shapefile &ob)
{
	ob.print(stream);
	return stream;
}

