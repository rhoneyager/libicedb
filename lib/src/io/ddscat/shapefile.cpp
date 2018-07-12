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
#include <boost/iostreams/copy.hpp>
#include <boost/lexical_cast.hpp>
#include <memory>

#include <icedb/splitSet.hpp>
#include <icedb/registry.hpp>
#include <icedb/error.hpp>
#include "shapefile.h"

namespace {
	std::set<std::string> mtypes;
	std::mutex mlock_shp;

	//std::map<rtmath::HASH_t, std::shared_ptr<shapefile> > stubShapes;
}

namespace icedb {
	namespace registry {
		template struct IO_class_registry_writer
			< ::icedb::io::ddscat::shapefile::shapefile > ;

		template struct IO_class_registry_reader
			< ::icedb::io::ddscat::shapefile::shapefile > ;

		template class usesDLLregistry <
			::icedb::io::ddscat::shapefile::shapefile_IO_output_registry,
			IO_class_registry_writer<::icedb::io::ddscat::shapefile::shapefile> > ;

		template class usesDLLregistry <
			::icedb::io::ddscat::shapefile::shapefile_IO_input_registry,
			IO_class_registry_reader<::icedb::io::ddscat::shapefile::shapefile> > ;


	}

	namespace io {
		template <>
		std::shared_ptr<::icedb::io::ddscat::shapefile::shapefile> customGenerator()
		{
			std::shared_ptr<::icedb::io::ddscat::shapefile::shapefile> res
				(new ::icedb::io::ddscat::shapefile::shapefile);
			return res;
		}
	}
}
namespace icedb {
	namespace io {
		namespace ddscat {
			namespace shapefile {

				implementsDDSHP::implementsDDSHP() :
					icedb::io::implementsIObasic<shapefile, shapefile_IO_output_registry,
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
					cp(filename);
					cp(ingest_timestamp);
					cp(ingest_hostname);
					//cp(ingest_username);
					//cp(ingest_rtmath_version);
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

				std::shared_ptr<shapefile> shapefile::generate(const std::string &filename)
				{
					std::shared_ptr<shapefile> res(new shapefile);
					res->read(filename);
					res->filename = filename;
					return res;
				}

				std::shared_ptr<shapefile> shapefile::generate(std::istream &in)
				{
					std::shared_ptr<shapefile> res(new shapefile(in));
					return res;
				}

				std::shared_ptr<shapefile> shapefile::generate()
				{
					std::shared_ptr<shapefile> res(new shapefile);
					return res;
				}

				std::shared_ptr<shapefile> shapefile::generate(std::shared_ptr<const shapefile> p)
				{
					std::shared_ptr<shapefile> res(new shapefile);
					*res = *p;
					return res;
				}

				shapefile::shapefile(std::istream &in)
				{
					_init();
					std::ostringstream so;
					boost::iostreams::copy(in, so);
					std::string s;
					s = so.str();
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
					//ingest_hostname = icedb::getHostname();
					//ingest_username = icedb::getUsername();
					//ingest_rtmath_version = rtmath::debug::rev();

					//filename = "";
					//::rtmath::io::Serialization::implementsSerialization<
					//	shapefile, shapefile_IO_output_registry, 
					//	shapefile_IO_input_registry, shapefile_serialization>::set_sname("icedb::io::ddscat::shapefile::shapefile");
				}

				void shapefile::info(std::ostream &out) const
				{
					// Print hash and all tags. Usually called after loading 
					// supplementary information from a database.
					//out << "Shape: " << this->hash().lower << std::endl;
					out << "Description: " << desc << std::endl;
					out << "# points: " << numPoints << std::endl;
					out << "Filename: " << this->filename << std::endl;
					out << "Ingested on: " << ingest_timestamp << std::endl;
					out << "Ingested host: " << ingest_hostname << std::endl;
					//out << "Ingested username: " << ingest_username << std::endl;
					//out << "Ingested rtmath: " << ingest_rtmath_version << std::endl;
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
						newinds.setLinSpaced((int)(n - numPoints), (int)numPoints, (int)n);
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
							std::string sNP = lin.substr(posa, len);
							numPoints = boost::lexical_cast<size_t>(sNP);
							//icedb::macros::m_atoi<size_t>(&(lin.data()[posa]), len);
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
								(*v)(j) = boost::lexical_cast<float>(lin.substr(posa,len));
								// icedb::macros::m_atof<float>(&(lin.data()[posa]), len);
							}
						}
						break;
						}
					}

					headerEnd = (pend - in) / sizeof(char);
					resize(numPoints);
				}

				void shapefile::writeDDSCAT(const std::shared_ptr<const shapefile> s, std::ostream &out, std::shared_ptr<icedb::registry::IO_options>)
				{
					s->print(out);
				}

				void shapefile::readDDSCAT(std::shared_ptr<shapefile> s, std::istream &in, std::shared_ptr<icedb::registry::IO_options> opts)
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
					//s->ingest_hostname = icedb::getHostname();
					//s->ingest_username = icedb::getUsername();
					//s->ingest_rtmath_version = rtmath::debug::rev();

				}

				void shapefile::fixStats()
				{
					x0 = means;
					xd = x0 * d;
				}

			}
		}
	}
}



std::ostream & operator<<(std::ostream &stream, const icedb::io::ddscat::shapefile::shapefile &ob)
{
	ob.print(stream);
	return stream;
}

