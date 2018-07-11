#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <complex>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/filter/newline.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <cmath>
#include "ddpar.h"
#include "ddVersions.h"
//#include <Ryan_Debug/config.h>
//#include "../rtmath/config.h"
#include <icedb/splitSet.hpp>
#include "rotations.h"
#include <icedb/error.hpp>

namespace {
	
	boost::filesystem::path pDefaultPar;
	const std::string ddparDefaultInternal = 
		"' ========= Parameter file for v7.3 =================== '\n"
		"'**** Preliminaries ****'\n"
		"'NOTORQ' = CMTORQ * 6 (NOTORQ, DOTORQ) --either do or skip torque calculations\n"
		"'PBCGS2' = CMDSOL * 6 (PBCGS2, PBCGST, PETRKP) --select solution method\n"
		"'GPFAFT' = CMDFFT * 6 (GPFAFT, FFTMKL)-- - FFT method\n"
		"'GKDLDR' = CALPHA * 6 (GKDLDR, LATTDR)\n"
		"'NOTBIN' = CBINFLAG(NOTBIN, ORIBIN, ALLBIN)\n"
		"'**** Initial Memory Allocation ****'\n"
		"101 101 101 = dimension\n"
		"'**** Target Geometry and Composition ****'\n"
		"'FROM_FILE' = CSHAPE * 9 shape directive\n"
		"101 101 101 = shape parameters 1 - 3\n"
		"1 = NCOMP = number of dielectric materials\n"
		"'diel.tab' = file with refractive index 1\n"
		"'**** Additional Nearfield calculation? ****'\n"
		"0 = NRFLD(= 0 to skip nearfield calc., = 1 to calculate nearfield E)\n"
		"0 0 0 0 0 0 = (fract.extens.of calc.vol.in - x, +x, -y, +y, -z, +z)\n"
		"'**** Error Tolerance ****'\n"
		"1e-05 = TOL = MAX ALLOWED(NORM OF | G >= AC | E>-ACA | X>) / (NORM OF AC | E>)\n"
		"'**** Maximum number of iterations ****'\n"
		"300 = MXITER\n"
		"'**** Integration cutoff parameter for PBC calculations ****'\n"
		"0.005 = GAMMA(1e-2 is normal, 3e-3 for greater accuracy)\n"
		"'**** Angular resolution for calculation of <cos>, etc. ****'\n"
		"0.5 = ETASCA(number of angles is proportional to[(3 + x) / ETASCA] ^ 2)\n"
		"'**** Vacuum wavelengths (micron) ****'\n"
		"3189.28 3189.28 1 'LIN' = wavelengths\n"
		"'**** Refractive index of ambient medium'\n"
		"1 = NAMBIENT\n"
		"'**** Effective Radii (micron) **** '\n"
		"341.49 341.49 1 'LIN' = aeff\n"
		"'**** Define Incident Polarizations ****'\n"
		"(0, 0) (1, 0) (0, 0) = Polarization state e01(k along x axis)\n"
		"2 = IORTH(= 1 to do only pol.state e01; = 2 to also do orth.pol.state)\n"
		"'**** Specify which output files to write ****'\n"
		"1 = IWRKSC(= 0 to suppress, = 1 to write \".sca\" file for each target orient.\n"
		"'**** Specify Target Rotations ****'\n"
		"0. 360. 18 = BETAMI, BETAMX, NBETA(beta = rotation around a1)\n"
		"0. 180. 19 = THETMI, THETMX, NTHETA(theta = angle between a1 and k)\n"
		"0. 360. 18 = PHIMIN, PHIMAX, NPHI(phi = rotation angle of a1 around k)\n"
		"'**** Specify first IWAV, IRAD, IORI (normally 0 0 0) ****'\n"
		"0 0 0 = first IWAV, first IRAD, first IORI(0 0 0 to begin fresh)\n"
		"'**** Select Elements of S_ij Matrix to Print ****'\n"
		"6 = NSMELTS = number of elements of S_ij to print(not more than 9)\n"
		"11 12 21 22 31 41 = indices ij of elements to print\n"
		"'**** Specify Scattered Directions ****'\n"
		"'LFRAME' = CMDFRM(LFRAME, TFRAME for Lab Frame or Target Frame)\n"
		"2 = NPLANES = number of scattering planes\n"
		"0 0 180 10 = phi, thetan_min, thetan_max, dtheta(in deg) for plane 1\n"
		"90 0 180 10 = phi, thetan_min, thetan_max, dtheta(in deg) for plane 2\n"
		;

	/// \todo This function should contain much of ddPar::defaultInstance
	void initPaths()
	{
		static bool loaded = false;
		if (loaded) return;
		using std::string;
		using namespace icedb;
		using boost::filesystem::path;

		loaded = true;
	}

	std::set<std::string> mtypes;
	std::mutex mlock;
}


namespace icedb {
	namespace registry {
		template struct IO_class_registry_writer
			< ::icedb::io::ddscat::ddPar > ;
		template class usesDLLregistry <
			::icedb::io::ddscat::ddPar_IO_output_registry,
			IO_class_registry_writer<::icedb::io::ddscat::ddPar> > ;
		template struct IO_class_registry_reader
			< ::icedb::io::ddscat::ddPar > ;
		template class usesDLLregistry <
			::icedb::io::ddscat::ddPar_IO_input_registry,
			IO_class_registry_reader<::icedb::io::ddscat::ddPar> > ;
	}
	namespace io {
		template <>
		std::shared_ptr<::icedb::io::ddscat::ddPar> customGenerator()
		{
			std::shared_ptr<::icedb::io::ddscat::ddPar> res
				= ::icedb::io::ddscat::ddPar::generate();
			//	(new ::icedb::io::ddscat::ddPar);
			return res;
		}
	}
}
namespace icedb {
	namespace io {
		namespace ddscat {

			implementsDDPAR::implementsDDPAR() :
				icedb::io::implementsIObasic<ddPar, ddPar_IO_output_registry,
				ddPar_IO_input_registry, ddPar_Standard>(ddPar::writeDDSCAT, ddPar::readDDSCATdef, known_formats())
			{}

			const std::set<std::string>& implementsDDPAR::known_formats()
			{
				// Moved to hidden file scope to avoid race condition
				//static std::set<std::string> mtypes;
				//static std::mutex mlock;
				// Prevent threading clashes
				{
					std::lock_guard<std::mutex> lck(mlock);
					if (!mtypes.size())
						mtypes.insert(".par");
				}
				return mtypes;
			}

			ddPar::ddPar()
			{
				_init();
				// Fill in the blanks from the default file. Needed here to avoid
				// crashed associated with accessing missing keys.
				//_populateDefaults(false);
			}
			/*
			ddPar::ddPar()
			{
				_init();
				_populateDefaults(false);
			}
			*/

			std::shared_ptr<ddPar> ddPar::generate(const std::shared_ptr<const ddPar> src)
			{
				std::shared_ptr<ddPar> res(new ddPar(*(src.get())));
				return res;
			}

			std::shared_ptr<ddPar> ddPar::generate()
			{
				std::shared_ptr<ddPar> res(new ddPar);
				return res;
			}

			std::shared_ptr<ddPar> ddPar::generate(const std::string &filename, bool popDefaults)
			{
				std::shared_ptr<ddPar> res(new ddPar);
				res->readFile(filename);
				if (popDefaults)
					res->populateDefaults(false);
				return res;
			}

			ddPar::~ddPar()
			{
			}

			bool ddPar::operator==(const ddPar &rhs) const
			{
				std::string sThis, sRhs;
				std::ostringstream oThis, oRhs;
				writeDDSCAT(this->shared_from_this(), oThis, nullptr);
				writeDDSCAT(rhs.shared_from_this(), oRhs, nullptr);
				sThis = oThis.str();
				sRhs = oRhs.str();
				return (sThis == sRhs);
			}

			bool ddPar::operator!=(const ddPar &rhs) const
			{
				return !(operator==(rhs));
			}

			ddPar & ddPar::operator=(const ddPar &rhs)
			{
				if (this != &rhs)
				{
					_version = rhs._version;
					std::ostringstream out;
					writeDDSCAT(rhs.shared_from_this(), out, nullptr);
					std::string data = out.str();
					std::istringstream in(data);
					read(in);
				}
				return *this;
			}

			ddPar::ddPar(const ddPar &src)
			{
				// Expensive copy constructor. Implements cloning to avoid screwups.
				_version = src._version;
				std::ostringstream out;
				writeDDSCAT(src.shared_from_this(), out, nullptr);
				std::string data = out.str();
				std::istringstream in(data);
				read(in);
			}

			std::shared_ptr<ddPar> ddPar::clone() const
			{
				std::shared_ptr<ddPar> lhs(new ddPar);

				lhs->_version = _version;

				std::ostringstream out;
				writeDDSCAT(this->shared_from_this(), out, nullptr);
				std::string data = out.str();
				std::istringstream in(data);

				lhs->read(in);

				return lhs;
			}

			/*
			void ddPar::readFile(const std::string &filename, bool overlay)
			{
				// Check file existence
				using namespace std;
				using namespace boost::filesystem;
				using namespace Ryan_Serialization;
				std::string cmeth, target, uncompressed;
				// Combination of detection of compressed file, file type and existence.
				if (!detect_compressed(filename, cmeth, target))
					throw rtmath::debug::xMissingFile(filename.c_str());
				uncompressed_name(target, uncompressed, cmeth);

				boost::filesystem::path p(uncompressed);
				boost::filesystem::path pext = p.extension(); // Uncompressed extension

				// Serialization gets its own override
				if (Ryan_Serialization::known_format(pext))
				{
					// This is a serialized file. Verify that it has the correct identifier, and
					// load the serialized object directly
					Ryan_Serialization::read<ddPar>(*this, filename, "icedb::io::ddscat::ddPar");
					return;
				}

				this->_filename = filename;

				std::ifstream in(filename.c_str(), std::ios_base::binary | std::ios_base::in);
				// Consutuct an filtering_iostream that matches the type of compression used.
				using namespace boost::iostreams;
				filtering_istream sin;
				if (cmeth.size())
					prep_decompression(cmeth, sin);
				sin.push(boost::iostreams::newline_filter(boost::iostreams::newline::posix));
				sin.push(in);

				read(sin, overlay);
			}


			void ddPar::writeFile(const std::string &filename, const std::string &type) const
			{
				populateDefaults();
				//std::ofstream out(filename.c_str());
				//write(out);


				using namespace Ryan_Serialization;
				std::string cmeth, uncompressed;
				uncompressed_name(filename, uncompressed, cmeth);
				boost::filesystem::path p(uncompressed);
				boost::filesystem::path pext = p.extension(); // Uncompressed extension

				std::string utype = type;
				if (!utype.size()) utype = pext.string();

				// Serialization gets its own override
				if (Ryan_Serialization::known_format(utype))
				{
					Ryan_Serialization::write<ddPar>(*this, filename, "icedb::io::ddscat::ddPar");
					return;
				}

				std::ofstream out(filename.c_str(), std::ios_base::out | std::ios_base::binary);
				using namespace boost::iostreams;
				filtering_ostream sout;
				if (cmeth.size())
					prep_compression(cmeth, sout);

				sout.push(boost::iostreams::newline_filter(boost::iostreams::newline::posix));
				sout.push(out);
				write(sout);
			}
			*/

			void ddPar::writeDDSCAT(const std::shared_ptr<const ddPar> p, std::ostream &out, std::shared_ptr<registry::IO_options> opts)
			{
				// Writing is much easier than reading!
				using namespace std;

				// Ensute that all necessary keys exist. If not, create them!!!
				//populateDefaults(); // User's responsibility

				if (!p) ICEDB_throw(error::error_types::xNullPointer)
					.add("Reason","ddPar::writeDDSCAT::p is null");
				// Write file version
				string ver;
				ver = icedb::io::ddscat::ddVersions::getVerString(p->_version);
				out << "' ========= Parameter file for v" << ver << " =================== '" << endl;

				// Loop through and write parameters and comments
				for (auto it = p->_parsedData.begin(); it != p->_parsedData.end(); it++)
				{
					// If key is valid for this output version, write it
					if (it->second->versionValid(p->_version))
						it->second->write(out, p->_version);

					// Check here for dielectric write. Always goes after NCOMP.
					if (it->first == ddParParsers::NCOMP)
					{
						int i = 1;
						for (auto ot = p->_diels.begin(); ot != p->_diels.end(); ++ot, ++i)
						{
							ostringstream o;
							// "...file with refractive index" + " #"
							o << " " << i;
							string plid = o.str();
							(*ot)->write(out, p->_version, plid);
						}
					}
				}
				for (auto ot = p->_scaPlanes.begin(); ot != p->_scaPlanes.end(); ++ot)
				{
					// If key is valid for this output version, write it
					if (ot->second->versionValid(p->_version))
					{
						ostringstream o;
						// "...for plane" + " #"
						o << " " << boost::lexical_cast<std::string>(ot->first);
						string plid = o.str();
						ot->second->write(out, p->_version, plid);
					}
				}
			}

			void ddPar::_init()
			{
				_version = 730;
			}

			void ddPar::readDDSCAT(std::shared_ptr<ddPar> src, std::istream &in, bool overlay)
			{
				src->read(in, overlay);
			}

			void ddPar::readDDSCATdef(std::shared_ptr<ddPar> src, std::istream &in, std::shared_ptr<icedb::registry::IO_options>)
			{
				readDDSCAT(src, in, false);
			}

			void ddPar::write(std::ostream& out) const
			{
				writeDDSCAT(this->shared_from_this(), out, nullptr);
			}

			void ddPar::read(std::istream &stream, bool overlay)
			{
				// Parse until end of stream, line-by-line
				// Split string based on equal sign, and do parsing
				// based on keys
				using namespace std;
				// _keys are mostly useless. Just used for loading.
				std::map<std::string, std::string> _keys;
				if (!overlay)
				{
					_parsedData.clear();
					_scaPlanes.clear();
					_diels.clear();
				}

				size_t line = 1;
				{
					// First line in file provides version information
					std::string vertag;
					std::getline(stream, vertag);

					_version = ddVersions::getVerId(vertag);
				}
				size_t nScaPlane = 0;
				string comment;
				while (stream.good())
				{
					string lin;
					std::getline(stream, lin);
					line++;
					// Check if this is a comment line (ends with ')
					// Need extra logic if line ends in whitespace
					{
						bool skip = false;
						for (auto it = lin.rbegin(); it != lin.rend(); ++it)
						{
							if (*it == ' ') continue;
							if (*it == '\'')
							{
								// End line parsing
								skip = true;
								break;
							}
							// If we make it here, the line is valid for 
							// key-value pair parsing
							skip = false;
							break;
						} // Awkward. TODO: redo.
						if (skip) continue;
					}

					// Split lin based on '='
					// Prepare tokenizer
					typedef boost::tokenizer<boost::char_separator<char> >
						tokenizer;
					boost::char_separator<char> sep("=");
					tokenizer tcom(lin, sep);
					vector<string> vals;
					for (auto it = tcom.begin(); it != tcom.end(); ++it)
						vals.push_back(*it);
					if (vals.size() < 2)
					{
						continue;
						//ostringstream errmsg;
						//errmsg << "This is not a valid ddscat.par file (error on file line " << line << ").";
						//throw rtmath::debug::xUnknownFileFormat(errmsg.str().c_str());
					}

					// Populate map
					//_keys[vals[1]] = vals[0];
					using namespace icedb::io::ddscat::ddParParsers;
					{
						std::shared_ptr<ddParLine> ptr = mapKeys(vals[1]);
						// Strip trailing whitespace at the end of vals[0].
						// It confuses some of the parsing functions, 
						// like ddParSimple<string>
						std::string vz = boost::algorithm::trim_right_copy(vals[0]);
						ptr->read(vz);
						// Individual dielectric files go into a separate structure
						// Also, if the dielectric files can be found, calculate their hashes
						if (ptr->id() == ddParParsers::IREFR)
						{
							auto p = std::dynamic_pointer_cast<ddParParsers::ddParLineSimple<std::string>>(ptr);
							_diels.push_back(p);
							std::string dval;
							p->get(dval);
							using namespace boost::filesystem;
							path ppar = path(_filename).remove_filename();
							path pval(dval);
							path prel = boost::filesystem::absolute(pval, ppar);

							//_dielHashes.push_back(HASHfile(dval));
						}
						// Everything but diels and scattering plane go here
						else if (ptr->id() < ddParParsers::PLANE1)
						{
							if (_parsedData.count(ptr->id()))
							{
								if (overlay)
								{
									_parsedData.erase(ptr->id());
								}
								else {
									ostringstream ostr;
									ostr << "Duplicate ddscat.par key: ";
									ostr << vals[1];
									ICEDB_throw(error::error_types::xBadInput)
										.add("Reason", ostr.str());
								}
							}
							_parsedData[ptr->id()] = ptr;
						}
						else if (ptr->id() == ddParParsers::PLANE1)
						{
							// Scattering plane info
							nScaPlane++;
							_scaPlanes[nScaPlane] =
								std::dynamic_pointer_cast<ddParParsers::ddParLineSimplePlural<double>>(ptr);
						}
						else {
							// Unknown key
							ostringstream ostr;
							ostr << "Unknown ddscat.par key: ";
							ostr << vals[1];
							ICEDB_throw(error::error_types::xBadInput)
								.add("Reason", ostr.str());
						}
					}

				}
			}

			std::shared_ptr<const ddPar> ddPar::defaultInstance()
			{
				using namespace std;
				using namespace boost::filesystem;
				static std::shared_ptr<ddPar> s_inst(new ddPar);
				static bool loaded = false;
				if (!loaded)
				{
					initPaths();
					if (pDefaultPar.string().size() && boost::filesystem::exists(path(pDefaultPar)))
					{
						s_inst = ddPar::generate(pDefaultPar.string(), false);
						// = std::shared_ptr<ddPar>(new ddPar(pDefaultPar.string(), false));
					}
					else {
						// Attempt to load the internal instance
						try {
							std::istringstream in(ddparDefaultInternal);
							//s_inst = new ddPar;
							readDDSCAT(s_inst, in, false);
							//Ryan_Serialization::readString(s_inst, ddparDefaultInternal, "icedb::io::ddscat::ddPar");

						}
						catch (std::exception&)
						{
							// Cannot get default instance.....
							if (pDefaultPar.string().size()) {
								ICEDB_throw(error::error_types::xMissingFile)
									.add("Filename", pDefaultPar.string());
							} else {
								ICEDB_throw(error::error_types::xOtherError)
									.add("Reason", "Cannot get default instance. Reason unknown.");
							}
						}
					}

					//rtmath::debug::instances::registerInstance( "ddPar::defaultInstance", reinterpret_cast<void*>(s_inst));
					loaded = true;
				}
				return s_inst;
			}

		} // end namespace ddscat
	}
}


