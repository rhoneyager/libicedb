#include "defs.hpp"
#include <icedb/error.hpp>
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
			namespace Shapes {
				template <typename T>
				void readDataset(HH::Dataset dset, std::vector<T> &outdata)
				{
					auto dims = dset.getDimensions();
					//Expects(dims.dimensionality == 2);
					outdata.resize(dims.numElements);
					if (dset.read<T>(outdata) < 0)
						ICEDB_throw(icedb::error::error_types::xBadInput)
						.add("Reason", "Cannot read a dataset.");
				}

				//------------------------------------------------------------------------------------------//
				/// Read a Penn State psuAydinetal-style GMM and DDA geometry files.
				//------------------------------------------------------------------------------------------//
				ShapeDataBasic readPSUfile(const std::string &filename)
				{
					using namespace std;
					ShapeDataBasic shpdata;
					//--------------------------------------------------------------------------------------//
					// The particle index is not specific enough. Let's use the filename for an id.
					//--------------------------------------------------------------------------------------//
					boost::filesystem::path p(filename);
					auto pfile = p.filename();
#if BOOST_VERSION < 104600
					string id = pfile.c_str(); // Needed for older RHEL machines
#else
					string id = pfile.string().c_str(); // Totally assuming a lack of non-Latin characters in the path.
#endif
					std::cout << "Reading id " << id << std::endl;

					//--------------------------------------------------------------------------------------//
					// Gain access to the data file.
					//--------------------------------------------------------------------------------------//

					auto hFile = HH::File::openFile(filename.c_str(), H5F_ACC_RDONLY);

					//--------------------------------------------------------------------------------------//
					// psuAydinetal geometry files are either for GMM or DDA calculations. Handle both.
					//--------------------------------------------------------------------------------------//

					// Declare variables that cover both in scope.

					size_t numPoints;
					vector<int32_t> particle_index;
					vector<int32_t> element_indices;
					vector<float>   x, y, z, rs;

					// Deal with the GMM and DDA file reads.
					if ((id.find("GMM")) != std::string::npos) {

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
							.add("Reason", "This file does not have the proper structure for a Penn State geometry file.")
							.add("Filename", filename);


						// No need to read particle_index. Not being used.
						readDataset<int32_t>(hFile.dsets["particle_index"], particle_index);
						readDataset<int32_t>(hFile.dsets["sphere_index"], element_indices);
						readDataset<float>(hFile.dsets["r"], rs);
						readDataset<float>(hFile.dsets["x"], x);
						readDataset<float>(hFile.dsets["y"], y);
						readDataset<float>(hFile.dsets["z"], z);

						// Check that the read arrays have matching sizes.

						numPoints = rs.size();

						if (numPoints != x.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add("Reason", "numpoints != x.size()");
						if (numPoints != y.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add("Reason", "numpoints != y.size()");
						if (numPoints != z.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add("Reason", "numpoints != z.size()");
						if (numPoints != element_indices.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add("Reason", "numpoints != element_indices.size()");

						// Done handling a GMM file.
					}
					else if ((id.find("DDA")) != std::string::npos) {

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
							.add("Reason", "This file does not have the proper structure for a Penn State geometry file.")
							.add("Filename", filename);

						// Open all of the datasets. Make sure that they have the correct dimensionality.
						// Read the data into vectors. Verify that the data have the appropriate sizes.

						std::vector<int32_t> xd, yd, zd;

						readDataset<int32_t>(hFile.dsets["particle_index"], particle_index);
						readDataset<int32_t>(hFile.dsets["dipole_index"], element_indices);
						readDataset<int32_t>(hFile.dsets["x"], xd);
						readDataset<int32_t>(hFile.dsets["y"], yd);
						readDataset<int32_t>(hFile.dsets["z"], zd);

						// Check that the read arrays have matching sizes.

						numPoints = xd.size();

						if (numPoints != yd.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add("Reason", "numpoints != yd.size()");
						if (numPoints != zd.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add("Reason", "numpoints != zd.size()");
						if (numPoints != element_indices.size()) ICEDB_throw(icedb::error::error_types::xBadInput).add("Reason", "numpoints != element_indices.size()");

						// Save xd, yd, zd coordinates into float x, y, z coordinates.

						x.resize(numPoints);
						y.resize(numPoints);
						z.resize(numPoints);

						for (size_t i = 0; i < numPoints; ++i) {
							x[i] = (float)xd[i];
							y[i] = (float)yd[i];
							z[i] = (float)zd[i];
						}

						// Done handling a DDA file.

					}

					//--------------------------------------------------------------------------------------//
					// Identify the particle type from the filename.
					//--------------------------------------------------------------------------------------//

					char particleType[32];

					if ((id.find("aggregate")) != std::string::npos) {
						sprintf(particleType, "Aggregate %05d", particle_index[0]);
					}
					else if ((id.find("branchedplanar")) != std::string::npos) {
						sprintf(particleType, "Branched Planar %05d", particle_index[0]);
					}
					else if ((id.find("column")) != std::string::npos) {
						sprintf(particleType, "Column %05d", particle_index[0]);
					}
					else if ((id.find("graupel")) != std::string::npos) {
						sprintf(particleType, "Conical Graupel %05d", particle_index[0]);
					}
					else if ((id.find("plate")) != std::string::npos) {
						sprintf(particleType, "Plate %05d", particle_index[0]);
					}

					//--------------------------------------------------------------------------------------//
					// Pack the data in the shpdata structure.
					//--------------------------------------------------------------------------------------//

					shpdata.required.number_of_particle_scattering_elements = static_cast<uint64_t>(numPoints);
					//shpdata.required.number_of_particle_constituents = 1;
					//shpdata.optional.particle_constituent_number.resize(1);
					//shpdata.optional.particle_constituent_number[0] = 1;
					shpdata.required.number_of_particle_constituents = 1;
					shpdata.optional.particle_constituent_number = { 1 };
					shpdata.optional.particle_constituent_name = { "ice" };


					shpdata.required.particle_id.resize(32);
					shpdata.required.particle_id = particleType; /// std::to_string(particle_index[0]);
					shpdata.required.particle_scattering_element_coordinates_are_integral = false;

					// Scattering element number with dimensions of
					// [number_of_particle_scattering_elements]

					shpdata.optional.particle_scattering_element_number.resize(1 * numPoints);

					// VARIABLE: Cartesian coordinates of the center of each scattering element
					// Written in form of x_1, y_1, z_1, x_2, y_2, z_2, ...
					// Dimensions of [number_of_particle_scattering_elements][axis]

					shpdata.required.particle_scattering_element_coordinates.resize(3 * numPoints);

					for (size_t i = 0; i < numPoints; ++i) {
						shpdata.required.particle_scattering_element_coordinates[(3 * i) + 0] = x[i];
						shpdata.required.particle_scattering_element_coordinates[(3 * i) + 1] = y[i];
						shpdata.required.particle_scattering_element_coordinates[(3 * i) + 2] = z[i];
						shpdata.optional.particle_scattering_element_number[i] = element_indices[i];
					}

					shpdata.optional.particle_constituent_name = { "ice" };

					if ((id.find("GMM")) != std::string::npos) {
						shpdata.optional.particle_scattering_element_radius = rs;
					}
					else if ((id.find("DDA")) != std::string::npos) {
						shpdata.optional.scattering_element_coordinates_scaling_factor = 0.001f; // 1 mm
						shpdata.optional.scattering_element_coordinates_units = "m";
					}

					//--------------------------------------------------------------------------------------//
					// Return the shpdata structure for writing to the new HDF5 output file.
					//--------------------------------------------------------------------------------------//

					return shpdata;

					//--------------------------------------------------------------------------------------//
					// Done.
					//--------------------------------------------------------------------------------------//

				}
			}
		}
	}
}
