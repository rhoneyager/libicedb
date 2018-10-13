#include "defs.hpp"
#include "plugin-io-ddscat.hpp"
#include <icedb/error.hpp>
#include <icedb/exv.hpp>
#include <icedb/registry.hpp>
#include <icedb/io.hpp>
#include <icedb/units/units.hpp>
#include <iostream>
#include <HH/Files.hpp>
#include <HH/Datasets.hpp>
#include <boost/filesystem.hpp> // Should switch to the boost-independent version
#include <memory>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "ddOutput.h"
#include "ddpar.h"
#include "rotations.h"

namespace icedb {
	namespace plugins {
		namespace ddscat {
			namespace exv {
				::icedb::exv::NewEXVrequiredProperties readDDSCATdir(std::shared_ptr<icedb::plugins::ddscat::ddscat_text_handle> h, std::shared_ptr<icedb::registry::IO_options> opts)
				{
					// error_info holds a stack of diagnostic error messages.
					::icedb::registry::options_ptr error_info = ::icedb::registry::options::generate();
					try {
						using namespace std;
						// We are reading the scattering matrices into this object.
						// See lib/icedb/exv.hpp.
						icedb::exv::NewEXVrequiredProperties exvdata;

						auto ddrun = icedb::io::ddscat::ddOutput::generate(opts->filename());
						
						for (size_t i = 0; i < ddrun->ms.at(0).size(); ++i) {
							std::tuple<int, std::string, std::complex<double> > newM((int)(i + 1), std::string("unspecified"), ddrun->ms.at(0).at(i));
							exvdata.constituent_refractive_indices.push_back(newM);
						}
						exvdata.frequency_Hz = (float) ddrun->freq;
						exvdata.temperature_K = (float) ddrun->temp;
						// TODO: if temperature is unspecified, guess it from a refractive index
						// formula, assuming that one of the media is ice.

						
						icedb::io::ddscat::rotations rots;
						ddrun->parfile->getRots(rots);
						std::set<float> Betas, Thetas, Phis;
						rots.betas(Betas);
						rots.thetas(Thetas);
						rots.phis(Phis);
						const auto numRots = ddrun->fmldata->rows();
						std::set<float> incid_pol{ 0 }, incid_azi{ 0 };
						std::set<float> scatt_pol, scatt_azi;

						icedb::exv::NewEXVrequiredProperties::ScattProps p{
							icedb::exv::NewEXVrequiredProperties::ScattProps::Rotation_Scheme::DDSCAT,
							Thetas, Phis, Betas, incid_pol, incid_azi, scatt_pol, scatt_azi};

						// Get scattered pol and azi directions
						size_t numPlanes = ddrun->parfile->numPlanes();
						Expects(numPlanes);
						for (size_t i = 0; i < numPlanes; ++i) {
							double phi, thetan_min, thetan_max, dtheta;
							ddrun->parfile->getPlane(i, phi, thetan_min, thetan_max, dtheta);
							if (i == 0) {
								// Use the theta directions
								std::set<double> dscatt_azi;
								icedb::splitSet::splitSet(thetan_min, thetan_max, dtheta, "", dscatt_azi);
								for (const auto d : dscatt_azi) scatt_azi.emplace((float)d);
							}
							// In all cases, use the phi direction
							scatt_pol.emplace((float)phi);
						}

						//exvdata.incident_azimuth_angle.resize(numRots);

						for (int i = 0; i < numRots; ++i) {
							using icedb::io::ddscat::ddOutput;
							const auto &f = ddrun->fmldata->block<1, ddOutput::fmlColDefs::NUM_FMLCOLDEFS>(i, 0);
							const auto &o = ddrun->oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(
								(Eigen::Index) f(ddOutput::fmlColDefs::ORIINDEX), 0);

							double Beta = o(ddOutput::stat_entries::BETA);
							double Theta = o(ddOutput::stat_entries::THETA);
							double Phi = o(ddOutput::stat_entries::PHI);
							double scattering_azimuth_angle = f(ddOutput::fmlColDefs::THETAB);
							double scattering_polar_angle = f(ddOutput::fmlColDefs::PHIB);
							auto getIndex = [&](size_t i) {
								return p.get_unidimensional_index(
									(float)Theta, (float)Phi, (float)Beta,
									0, 0, (float)scattering_polar_angle, (float)scattering_azimuth_angle,
									i);
							};

							p.amplitude_scattering_matrix[getIndex(0)] 
								= std::complex<double>(f(ddOutput::fmlColDefs::F00R), f(ddOutput::fmlColDefs::F00I));
							p.amplitude_scattering_matrix[getIndex(1)]
								= std::complex<double>(f(ddOutput::fmlColDefs::F01R), f(ddOutput::fmlColDefs::F01I));
							p.amplitude_scattering_matrix[getIndex(2)]
								= std::complex<double>(f(ddOutput::fmlColDefs::F10R), f(ddOutput::fmlColDefs::F10I));
							p.amplitude_scattering_matrix[getIndex(3)]
								= std::complex<double>(f(ddOutput::fmlColDefs::F11R), f(ddOutput::fmlColDefs::F11I));
						}

						exvdata.scattering_properties.push_back(p);


						// Scattering matrices are read. Rotations are read. Temp and freq are read.

						// Let's use the folder name for an id.
						boost::filesystem::path pt(opts->filename());
						auto pfile = pt.filename();
						string fname = pfile.string().c_str();
						error_info->add("Filename", opts->filename());

						exvdata.particle_id = fname;

						exvdata.author = "PLACEHOLDER"; // Optional: can be set by importer program.
						exvdata.contact = "PLACEHOLDER"; // Optional: can be set by importer program.
						exvdata.dataset_id = "PLACEHOLDER"; // Optional: can be set by importer program.
						exvdata.scattMeth = "DDSCAT"; // Optional: can be set by importer program. TODO: Tag with detected DDSCAT version.
						exvdata.version = std::array<unsigned int, 3>{0, 0, 0}; // Optional: can be set by importer program.
						//exvdata.ingest_timestamp; // Always set by importer program

						return exvdata;
					}
					//-----------------------
					// Error tagging (if an error was detected, the code calls this)
					//-----------------------
					catch (icedb::error::xError &err) {
						error_info->add<std::string>("Reason", "Reading the directory failed.");
						err.push(error_info);
						throw err; // Pass the error up the stack.
					}
				}
			}
		}
	}

	using std::shared_ptr;
	using namespace icedb::plugins::ddscat::exv;

	namespace registry {
		template<> shared_ptr<IOhandler>
			read_file_type_multi<::icedb::exv::NewEXVrequiredProperties>
				(shared_ptr<IOhandler> sh, shared_ptr<IO_options> opts,
					shared_ptr<::icedb::exv::NewEXVrequiredProperties > s,
					shared_ptr<const icedb::registry::collectionTyped<::icedb::exv::NewEXVrequiredProperties> > filter)
			{
				// Prepare to read the shape - open a "handle" to the file if it is not already open.
				std::string filename = opts->filename();
				IOhandler::IOtype iotype = opts->getVal<IOhandler::IOtype>("iotype", IOhandler::IOtype::READONLY);
				using std::shared_ptr;
				std::shared_ptr<icedb::plugins::ddscat::ddscat_text_handle> h = registry::construct_handle
					<registry::IOhandler, icedb::plugins::ddscat::ddscat_text_handle>(
						sh, PLUGINID, [&]() {return std::shared_ptr<icedb::plugins::ddscat::ddscat_text_handle>(
							new icedb::plugins::ddscat::ddscat_text_handle(filename.c_str(), iotype)); });

				/// \todo Check that the passed object is actually a directory.
				*s = readDDSCATdir(h, opts);

				// Return the opened "handle".
				return h;
			}
	}
}
