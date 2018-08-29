#include "defs.hpp"
#include "plugin-psu.hpp"
#include <icedb/error.hpp>
#include <icedb/exv.hpp>
#include <icedb/registry.hpp>
#include <icedb/io.hpp>
#include <iostream>
#include <HH/Files.hpp>
#include <HH/Datasets.hpp>
#include <boost/filesystem.hpp> // Should switch to the boost-independent version
#include <memory>
#include <map>
#include <set>
#include <string>
#include <vector>
//#include "shapeIOtext.hpp"

namespace icedb {
	namespace plugins {
		namespace psu {
			namespace exv {
				template <typename T>
				void readDataset(const std::string dsetname, HH::Dataset dset, std::vector<T> &outdata)
				{
					auto dims = dset.getDimensions();
					//Expects(dims.dimensionality == 2);
					outdata.resize(dims.numElements);
					if (dset.read<T>(outdata) < 0)
						ICEDB_throw(icedb::error::error_types::xBadInput)
						.add("Reason", "HDF5 error when reading a dataset.")
						.add("Dataset", dsetname);
				}

				//------------------------------------------------------------------------------------------//
				/// Read a Penn State psuAydinetal-style GMM and DDA geometry files.
				//------------------------------------------------------------------------------------------//
				::icedb::exv::NewEXVrequiredProperties readPSUfile(std::shared_ptr<icedb::plugins::psu::psu_handle> h, std::shared_ptr<icedb::registry::IO_options> opts)
				{
					// error_info holds a stack of diagnostic error messages.
					::icedb::registry::options_ptr error_info = ::icedb::registry::options::generate();
					try {
						using namespace std;
						icedb::exv::NewEXVrequiredProperties exvdata; // We are reading the scattering matrices into this object

						//--------------------------------------------------------------------------------------//
						// The particle index is not specific enough. Let's use the filename for an id.
						//--------------------------------------------------------------------------------------//
						boost::filesystem::path p(opts->filename());
						auto pfile = p.filename();
						string id = pfile.string().c_str();
						std::cout << "Reading id " << id << std::endl;
						error_info->add("Filename", opts->filename());
						error_info->add("Particle-id", id);

						//--------------------------------------------------------------------------------------//
						// Gain access to the data file.
						//--------------------------------------------------------------------------------------//

						auto hFile = h->file;

						/*
						size_t numPoints;
						vector<int32_t> particle_index;
						vector<int32_t> element_indices;
						vector<float>   x, y, z, rs;
						vector<int32_t> xd, yd, zd;

						string PSUfileType;
						if ((id.find("GMM")) != std::string::npos) PSUfileType = "GMM";
						else if ((id.find("DDA")) != std::string::npos) PSUfileType = "DDA";
						else ICEDB_throw(icedb::error::error_types::xBadInput)
							.add<std::string>("Reason", "Cannot determine the type of PSU file (GMM/DDA) from the filename.");
						error_info->add("PSUfileType", PSUfileType);


						// Deal with the GMM and DDA file reads.
						if (PSUfileType == "GMM") {

							// A valid PSU GMM file has these tables: particle_index, sphere_index, r, x, y, z.
							// particle_index has one row, one column.
							// The rest have one column and a number of rows that correspond to the number
							// of spheres used to represent the particle.

							if (!hFile.dsets.exists("particle_index")
								|| !hFile.dsets.exists("sphere_index")
								|| !hFile.dsets.exists("r")
								|| !hFile.dsets.exists("x")
								|| !hFile.dsets.exists("y")
								|| !hFile.dsets.exists("z"))
								ICEDB_throw(icedb::error::error_types::xBadInput)
								.add<std::string>("Reason", "A necessary dataset (particle_index, sphere_index, r, x, y, z) does not exist.");


							// No need to read particle_index. Not being used.
							readDataset<int32_t>("particle_index", hFile.dsets["particle_index"], particle_index);
							readDataset<int32_t>("sphere_index", hFile.dsets["sphere_index"], element_indices);
							readDataset<float>("r", hFile.dsets["r"], rs);
							readDataset<float>("x", hFile.dsets["x"], x);
							readDataset<float>("y", hFile.dsets["y"], y);
							readDataset<float>("z", hFile.dsets["z"], z);

							// Check that the read arrays have matching sizes.

							numPoints = rs.size();

							if (numPoints != x.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add<std::string>("Reason", "numpoints != x.size()");
							if (numPoints != y.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add<std::string>("Reason", "numpoints != y.size()");
							if (numPoints != z.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add<std::string>("Reason", "numpoints != z.size()");
							if (numPoints != element_indices.size())
								ICEDB_throw(icedb::error::error_types::xBadInput)
								.add<std::string>("Reason", "numpoints != element_indices.size()");

							// Done handling a GMM file.
						}
						else if (PSUfileType == "DDA") {

							// A valid PSU GMM file has these tables: particle_index, sphere_index, r, x, y, z.
							// particle_index has one row, one column.
							// The rest have one column and a number of rows that correspond to the number
							// of spheres used to represent the particle.

							if (!hFile.dsets.exists("particle_index")
								|| !hFile.dsets.exists("dipole_index")
								|| !hFile.dsets.exists("x")
								|| !hFile.dsets.exists("y")
								|| !hFile.dsets.exists("z"))
								ICEDB_throw(icedb::error::error_types::xBadInput)
								.add<std::string>("Reason", "A necessary dataset (particle_index, dipole_index, x, y, z) does not exist.");

							// Open all of the datasets. Make sure that they have the correct dimensionality.
							// Read the data into vectors. Verify that the data have the appropriate sizes.


							readDataset<int32_t>("particle_index", hFile.dsets["particle_index"], particle_index);
							readDataset<int32_t>("dipole_index", hFile.dsets["dipole_index"], element_indices);
							readDataset<int32_t>("x", hFile.dsets["x"], xd);
							readDataset<int32_t>("y", hFile.dsets["y"], yd);
							readDataset<int32_t>("z", hFile.dsets["z"], zd);

							// Check that the read arrays have matching sizes.

							numPoints = xd.size();

							if (numPoints != yd.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add<std::string>("Reason", "numpoints != yd.size()");
							if (numPoints != zd.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add<std::string>("Reason", "numpoints != zd.size()");
							if (numPoints != element_indices.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add<std::string>("Reason", "numpoints != element_indices.size()");

							// Done handling a DDA file.

						}

						//--------------------------------------------------------------------------------------//
						// Identify the particle type from the filename.
						//--------------------------------------------------------------------------------------//

						const size_t max_ParticleType_Size = 32;
						char particleType[max_ParticleType_Size];
						{
							if ((id.find("aggregate")) != std::string::npos) {
								snprintf(particleType, max_ParticleType_Size, "Aggregate %05d", particle_index[0]);
							}
							else if ((id.find("branchedplanar")) != std::string::npos) {
								snprintf(particleType, max_ParticleType_Size, "Branched Planar %05d", particle_index[0]);
							}
							else if ((id.find("column")) != std::string::npos) {
								snprintf(particleType, max_ParticleType_Size, "Column %05d", particle_index[0]);
							}
							else if ((id.find("graupel")) != std::string::npos) {
								snprintf(particleType, max_ParticleType_Size, "Conical Graupel %05d", particle_index[0]);
							}
							else if ((id.find("plate")) != std::string::npos) {
								snprintf(particleType, max_ParticleType_Size, "Plate %05d", particle_index[0]);
							}
						}

						//--------------------------------------------------------------------------------------//
						// Pack the data in the shpdata structure.
						//--------------------------------------------------------------------------------------//

						shpdata.particle_id = string(particleType);
						shpdata.particle_constituents.push_back(std::make_pair(1, "ice"));
						shpdata.author = "Eugene Clothiaux (eec3@psu.edu)"; // TODO: Eugene: fill this in.
						shpdata.contact = "Eugene Clothiaux (eec3@psu.edu)";
						shpdata.scattering_method = PSUfileType;
						//shpdata.version = { 0,0,0 }; // Should be set by the importer program.
						shpdata.dataset_id = "TESTING---MUST-SET-IN-PRODUCTION"; // Should be set by the importer program.

						shpdata.particle_scattering_element_number = element_indices;
						shpdata.particle_scattering_element_composition_whole = decltype(shpdata.particle_scattering_element_composition_whole)(numPoints, 1);

						if (PSUfileType == "GMM") {
							shpdata.particle_scattering_element_radius = rs;
							shpdata.scattering_element_coordinates_scaling_factor = 1; // 1 m

							shpdata.particle_scattering_element_coordinates_as_floats.resize(3 * numPoints);
							for (size_t i = 0; i < numPoints; ++i) {
								shpdata.particle_scattering_element_coordinates_as_floats[(3 * i) + 0] = x[i];
								shpdata.particle_scattering_element_coordinates_as_floats[(3 * i) + 1] = y[i];
								shpdata.particle_scattering_element_coordinates_as_floats[(3 * i) + 2] = z[i];
							}
						}
						else if (PSUfileType == "DDA") {
							shpdata.scattering_element_coordinates_scaling_factor = 0.001f; // 1 mm

							shpdata.particle_scattering_element_coordinates_as_ints.resize(3 * numPoints);
							for (size_t i = 0; i < numPoints; ++i) {
								shpdata.particle_scattering_element_coordinates_as_ints[(3 * i) + 0] = xd[i];
								shpdata.particle_scattering_element_coordinates_as_ints[(3 * i) + 1] = yd[i];
								shpdata.particle_scattering_element_coordinates_as_ints[(3 * i) + 2] = zd[i];
							}
						}
						*/
						//--------------------------------------------------------------------------------------//
						// Return the shpdata structure for writing to the new HDF5 output file.
						//--------------------------------------------------------------------------------------//

						return exvdata;

						//--------------------------------------------------------------------------------------//
						// Done.
						//--------------------------------------------------------------------------------------//
					}
					// Error tagging
					catch (icedb::error::xError &err) {
						error_info->add<std::string>("Reason", "This file does not have the proper structure for a Penn State geometry file.");
						err.push(error_info);
						throw err;
					}
				}
			}
		}
	}

	using std::shared_ptr;
	using namespace icedb::plugins::psu::exv;

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
				std::shared_ptr<icedb::plugins::psu::psu_handle> h = registry::construct_handle
					<registry::IOhandler, icedb::plugins::psu::psu_handle>(
						sh, PLUGINID, [&]() {return std::shared_ptr<icedb::plugins::psu::psu_handle>(
							new icedb::plugins::psu::psu_handle(filename.c_str(), iotype)); });

				// Actually read the shape
				*s = readPSUfile(h, opts);

				// Return the opened "handle".
				return h;
			}
	}
}
