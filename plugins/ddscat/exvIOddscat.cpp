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
							std::tuple<int, std::string, std::complex<double> > newM((int)(i + 1), std::string("unspecified"), s->ms.at(0).at(i));
							exvdata.constituent_refractive_indices.push_back(newM);
						}
						exvdata.frequency_Hz = ddrun->freq;
						exvdata.temperature_K = ddrun->temp;
						exvdata.scattMeth = "DDSCAT";
						
						/*
						exvdata.author = ddrun->author;
						exvdata.contact = ddrun->contact;
						exvdata.version = ddrun->version;
						exvdata.dataset_id = ddrun->datasetID;
						exvdata.scattMeth = ddrun->scattMeth;
						exvdata.ingest_timestamp = ddrun->ingest_timestamp;
						*/

						const auto numRots = ddrun->fmldata->rows();
						exvdata.rotation.resize(numRots);
						/* Assuming that:
						- Each rotation has the same number of scattering angles for the calculation.
						*/
						// First, set the incident and scattering azimuth and polar angles.
						exvdata.incident_azimuth_angle;
						exvdata.incident_polar_angle;
						exvdata.scattering_azimuth_angle;
						exvdata.scattering_polar_angle;

						//exvdata.incident_azimuth_angle.resize(numRots);

						for (int i = 0; i < numRots; ++i) {
							auto &a = exvdata.rotation[i];
							using icedb::io::ddscat::ddOutput;
							const auto &f = ddrun->fmldata->block<1, ddOutput::fmlColDefs::NUM_FMLCOLDEFS>(i, 0);
							const auto &o = ddrun->oridata_d.block<1, ddOutput::stat_entries::NUM_STAT_ENTRIES_DOUBLES>(
								f(ddOutput::fmlColDefs::ORIINDEX), 0);
							a.alpha = o(ddOutput::stat_entries::BETA);
							a.beta = o(ddOutput::stat_entries::THETA);
							a.gamma = o(ddOutput::stat_entries::PHI);
							a.incident_azimuth_angle = 0;
							a.incident_polar_angle = 0;
							a.scattering_azimuth_angle = f(ddOutput::fmlColDefs::THETAB);
							a.scattering_polar_angle = f(ddOutput::fmlColDefs::PHIB);
							a.amplitude_scattering_matrix[0] = std::complex<double>(f(ddOutput::fmlColDefs::F00R), f(ddOutput::fmlColDefs::F00I));
							a.amplitude_scattering_matrix[1] = std::complex<double>(f(ddOutput::fmlColDefs::F01R), f(ddOutput::fmlColDefs::F01I));
							a.amplitude_scattering_matrix[2] = std::complex<double>(f(ddOutput::fmlColDefs::F10R), f(ddOutput::fmlColDefs::F10I));
							a.amplitude_scattering_matrix[3] = std::complex<double>(f(ddOutput::fmlColDefs::F11R), f(ddOutput::fmlColDefs::F11I));
						}

						//--------------------------------------------------------------------------------------//
						// The particle index is not specific enough. Let's use the filename for an id.
						//--------------------------------------------------------------------------------------//
						boost::filesystem::path p(opts->filename());
						auto pfile = p.filename();
						string fname = pfile.string().c_str();
						// Parse the filename.
						// Example name is psuaydinetal_aggregate_00001_HD-P1d_0.42_01_f9GHz_01_GMM.nc.
						// We can get the particle class, id, frequency, interdipole spacing, etc.
						error_info->add("Filename", opts->filename());

						//--------------------------------------------------------------------------------------//
						// Gain access to the data file.
						//--------------------------------------------------------------------------------------//

						// hFile is an HH::File object.
						auto &hFile = h->file;
						// See lib/deps/HH for the source code of the HDFforHumans library.
						// See the docs/ directory for documentation and basic examples for this library.
						// Groups.md, Datasets.md and Attributes.md will be particularly useful.

						// To access the datasets stored under hFile, use
						// hFile.dsets;
						// To see if a dataset exists, use (hFile.dsets.exists("name") > 0);
						// To list all datasets: std::vector<std::string> names = hFile.dsets.list();
						// To open a dataset: HH::DataSet dName = hFile.dsets.open("name");

						//-----------------------
						// Parse the filename.
						//-----------------------
						// TODO!

						// Set particle_id. It's a std::string.
						particle_info pi = getParticleInfo(opts->filename()); // Gives information about the particle id
						exvdata.particle_id = pi.particle_id;
						// constituent_refractive_indices is a vector of (int, string, complex<double>) tuples.
						// - The int is the substance id number. (1)
						// - The string is the substance name (ice)
						// - The complex<double> is the refractive index that you used for the scattering data calculations.

						// NOTE: Refractive index currently has to use the sign convention for DDSCAT (N=n-ik), n,k>0.
						// This goes against the B&H convention, where N=n+ik, where n,k>0. 
						// Potential area for confusion, particularly since we write the amplitude matrices in B&H conventions.
						// May change in the future.
						exvdata.constituent_refractive_indices.push_back(std::make_tuple(
							1, "ice", std::complex<double>(0, 0))); // Set the refractive index (the 0,0 part).

						// See pi.freq_number_as_string; pi.freq_units;
						// The icedb::units library handles the conversion.
						exvdata.frequency_Hz = icedb::units::conv_spec(pi.freq_units, "Hz")
							.convert(boost::lexical_cast<float>(pi.freq_number_as_string)); //It's a float.

						exvdata.temperature_K = -1; // Set this. It's a float.
						exvdata.author = "PLACEHOLDER"; // Optional: can be set by importer program.
						exvdata.contact = "PLACEHOLDER"; // Optional: can be set by importer program.
						exvdata.dataset_id = "PLACEHOLDER"; // Optional: can be set by importer program.
						exvdata.scattMeth = "PLACEHOLDER"; // Optional: can be set by importer program.
						exvdata.version = std::array<unsigned int, 3>{0, 0, 0}; // Optional: can be set by importer program.
						//exvdata.ingest_timestamp; // Always set by importer program

						// NOTE: At present, the EXV files do not record the PARTICLE ORIENTATION! This must be fixed later.
						// These are in the ESSENTIAL scattering variables files, and have to be moved here.
						// TODO: We have to work out the conventions for these rotations. Presently using Euler angles to 
						// agree with the Mishchenko books. This may conflict with DDSCAT. Can these conventions be uniquely interconverted?
						// We have to document the convention of the rotations. What exactly does (0,0,0) mean relative to the incident light angles?
						exvdata.alpha = -1; // First rotation (degrees).
						exvdata.beta = -1; // Second rotation (degrees).
						exvdata.gamma = -1; // Third rotation (degrees).

						//-----------------------
						// Check that the appropriate datasets exist
						//-----------------------
						// These should all exist, and should all have 32-bit float data.
						const std::set<std::string> requiredDatasets = {
							"S1_imag", "S1_real", "S2_imag", "S2_real", "S3_imag", "S3_real",
							"S4_imag", "S4_real", "incident_azimuth_angle", "incident_polar_angle",
							"scattering_azimuth_angle", "scattering_polar_angle"
						};
						for (const auto &d : requiredDatasets) {
							if (!(hFile.dsets.exists(d.c_str()) > 0))
								ICEDB_throw(icedb::error::error_types::xBadInput)
								.add<std::string>("Reason", "A required dataset is missing.")
								.add<std::string>("Missing-dataset-name", d);
							if (!hFile.dsets[d.c_str()].isOfType<float>())
								ICEDB_throw(icedb::error::error_types::xBadInput)
								.add<std::string>("Reason", "A dataset has the wrong data type. It should be a 32-bit floating point type.")
								.add<std::string>("Dataset-name", d);
						}

						//-----------------------
						// Read the data
						//-----------------------

						// S have dims of incid_polar_angle, incid_azimuth_angle, scatt_polar_angle, scatt_azimuth_angle
						// sizes around 19 x 18 x 181 x 72.
						vector<float> S1i, S1r, S2i, S2r, S3i, S3r, S4i, S4r,
							incid_azi, incid_polar, scatt_azi, scatt_polar;
						readDataset<float>("S1_imag", hFile.dsets["S1_imag"], S1i);
						readDataset<float>("S1_real", hFile.dsets["S1_real"], S1r);
						readDataset<float>("S2_imag", hFile.dsets["S2_imag"], S2i);
						readDataset<float>("S2_real", hFile.dsets["S2_real"], S2r);
						readDataset<float>("S3_imag", hFile.dsets["S3_imag"], S3i);
						readDataset<float>("S3_real", hFile.dsets["S3_real"], S3r);
						readDataset<float>("S4_imag", hFile.dsets["S4_imag"], S4i);
						readDataset<float>("S4_real", hFile.dsets["S4_real"], S4r);
						readDataset<float>("incident_azimuth_angle", hFile.dsets["incident_azimuth_angle"], exvdata.incident_azimuth_angle);
						readDataset<float>("incident_polar_angle", hFile.dsets["incident_polar_angle"], exvdata.incident_polar_angle);
						readDataset<float>("scattering_azimuth_angle", hFile.dsets["scattering_azimuth_angle"], exvdata.scattering_azimuth_angle);
						readDataset<float>("scattering_polar_angle", hFile.dsets["scattering_polar_angle"], exvdata.scattering_polar_angle);

						//-----------------------
						// Check that all of the read data have the correct sizes.
						//-----------------------
						const size_t numScattMatrices = exvdata.incident_azimuth_angle.size() * exvdata.incident_polar_angle.size()
							* exvdata.scattering_azimuth_angle.size() * exvdata.scattering_polar_angle.size();
						{
							bool badSize = false;
							if (S1i.size() != S1r.size()) badSize = true;
							if (S1i.size() != S1r.size()) badSize = true;
							if (S1i.size() != S2i.size()) badSize = true;
							if (S1i.size() != S2r.size()) badSize = true;
							if (S1i.size() != S3i.size()) badSize = true;
							if (S1i.size() != S3r.size()) badSize = true;
							if (S1i.size() != S4i.size()) badSize = true;
							if (S1i.size() != S4r.size()) badSize = true;
							if (numScattMatrices != S1i.size()) badSize = true;
							if (badSize)ICEDB_throw(icedb::error::error_types::xBadInput)
								.add<std::string>("Reason", "Datasets have inconsistent sizes.");
						}

						//-----------------------
						// Convert the data to the correct convention.
						//-----------------------
						// TODO! Strongly depends on any changes that could be made to the exv and esv table structures.
						// Definitions must match Bohren and Huffman.

						//-----------------------
						// Populate the exvdata structure
						//-----------------------
						exvdata.amplitude_scattering_matrix.resize(numScattMatrices);
						for (size_t i = 0; i < S1i.size(); ++i) {
							// NOTE: The convention of S1,S2,S3,S4 matters here.
							// A few books have the matrix written as S2, S3, S4, S1. Some have S2, S1, S3, S4, and others have S1, S2, S3, S4!!!
							// To preserve sanity, let's write these as S11, S12, S21, S22, where [ij] is row/column.

							// TODO: You may have to adjust the ordering of these. Really depends on your convention.
							// NOTE: The imaginary components get stored as positive numbers.
							exvdata.amplitude_scattering_matrix[i] = std::array<std::complex<double>, 4> {
								std::complex<double>(S1r[i], std::abs(S1i[i])),
									std::complex<double>(S2r[i], std::abs(S2i[i])),
									std::complex<double>(S3r[i], std::abs(S3i[i])),
									std::complex<double>(S4r[i], std::abs(S4i[i]))
							};
						}

						//-----------------------
						// Finished
						//-----------------------
						return exvdata;
					}
					//-----------------------
					// Error tagging (if an error was detected, the code calls this)
					//-----------------------
					catch (icedb::error::xError &err) {
						error_info->add<std::string>("Reason", "This file does not have the proper structure for a Penn State geometry file.");
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
