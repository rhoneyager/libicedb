#include <icedb/defs.h>
#include <icedb/error.hpp>
#include <iostream>
#include <hdf5.h>
#include <hdf5_hl.h>
#include <boost/filesystem.hpp>
#include <memory>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "shapeIOtext.hpp"

namespace icedb {
	namespace Examples {
		namespace Shapes {

			/// This makes it much easier to automatically close HDF5 handles that are no longer used.

			namespace ScopedHandles {
				struct InvalidHDF5Handle {
					static inline bool isInvalid(hid_t h) {
						if (h < 0) return true;
						return false;
					}
				};
				struct CloseHDF5File {
					static inline void Close(hid_t h) {
						herr_t err = H5Fclose(h);
						if (err < 0) ICEDB_throw(icedb::error::error_types::xOtherError)
							.add("Reason", "Cannot close an open HDF5 file.");
					}
				};
				struct CloseHDF5Dataset {
					static inline void Close(hid_t h) {
						herr_t err = H5Dclose(h);
						if (err < 0) ICEDB_throw(icedb::error::error_types::xOtherError)
							.add("Reason", "Cannot close an HDF5 dataset.");
					}
				};
				struct CloseHDF5Dataspace {
					static inline void Close(hid_t h) {
						herr_t err = H5Sclose(h);
						if (err < 0) ICEDB_throw(icedb::error::error_types::xOtherError)
							.add("Reason", "Cannot close an HDF5 dataspace.");
					}
				};
				struct CloseHDF5Datatype {
					static inline void Close(hid_t h) {
						herr_t err = H5Tclose(h);
						if (err < 0) ICEDB_throw(icedb::error::error_types::xOtherError)
							.add("Reason", "Cannot close an HDF5 datatype.");
					}
				};

				/// \todo Extend with SFINAE and constexpr type detection for InvalidValueClass and CloseMethod.
				/// If these are not specified as template parameters, then they must be passed in the constructor.

				template <typename HandleType, class InvalidValueClass, class CloseMethod>
				struct ScopedHandle
				{
					HandleType h = 0;
					bool valid() const { return !InvalidValueClass::isInvalid(h); }
					~ScopedHandle() {
						if (valid()) CloseMethod::Close(h);
						h = 0;
					}
					ScopedHandle(HandleType newh) : h(newh) {}
				};

				typedef ScopedHandle<hid_t, InvalidHDF5Handle, CloseHDF5File> H5F_handle;
				typedef ScopedHandle<hid_t, InvalidHDF5Handle, CloseHDF5Dataset> H5D_handle;
				typedef ScopedHandle<hid_t, InvalidHDF5Handle, CloseHDF5Dataspace> H5DS_handle;
				typedef ScopedHandle<hid_t, InvalidHDF5Handle, CloseHDF5Datatype> H5T_handle;

			}

			typedef hid_t MatchType_t;
			template <class DataType>
			MatchType_t MatchType()
			{
				ICEDB_throw(icedb::error::error_types::xUnimplementedFunction)
					.add("Reason", "Unsupported type during conversion.");
			}
			template<> MatchType_t MatchType<int32_t>() {
				return H5T_NATIVE_INT32;
			}
			template<> MatchType_t MatchType<float>() {
				return H5T_NATIVE_FLOAT;
			}

			template <typename T>
			void readDataset(hid_t file_id, const char* dataset_name, std::vector<T> &outdata)
			{
				try {

					ScopedHandles::H5D_handle hData(H5Dopen(file_id, dataset_name, H5P_DEFAULT));
					if (!hData.valid()) ICEDB_throw(icedb::error::error_types::xBadInput);

					// Get dimensionality

					ScopedHandles::H5DS_handle hSpace(H5Dget_space(hData.h));
					if (!hSpace.valid()) ICEDB_throw(icedb::error::error_types::xBadInput);
					int rank = H5Sget_simple_extent_ndims(hSpace.h);
					if (rank == 0 || rank > 2) ICEDB_throw(icedb::error::error_types::xBadInput);
					hssize_t numElems = H5Sget_simple_extent_npoints(hSpace.h);

					// Verify that the dataset has the expected type

					ScopedHandles::H5T_handle hDatatype(H5Dget_type(hData.h));
					MatchType_t funcDataType = MatchType<T>();
					if (H5Tequal(funcDataType, hDatatype.h) <= 0) ICEDB_throw(icedb::error::error_types::xBadInput);

					// If this conversion fails, then either we are trying to open a dataset with
					// the wrong data type (i.e. float vs int32 vs int16), or the architecture
					// between the systems used to store and read the data are _very_ different,
					// which is highly unlikely.

					// Resize output vector

					outdata.resize(numElems);

					// Read the data

					if (H5Dread(
						hData.h, // dataset
						hDatatype.h, // in-memory data type
						H5S_ALL, // memory data space; reading the entire dataset
						H5S_ALL, // file data space; reading the entire dataset
						H5P_DEFAULT, // default transfer properties
						(void*)outdata.data()) // the output buffer (pre-sized)
						< 0) ICEDB_throw(icedb::error::error_types::xBadInput);

				}
				catch (icedb::error::xError &err) {
					err
						.add("Reason", "Cannot open an HDF5 dataspace.")
						.add("Dataset", dataset_name);
					throw err; // rethrow
				}
			}

            //------------------------------------------------------------------------------------------//
			// Read a Penn State psuAydinetal-style GMM and DDA geometry files.
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

				ScopedHandles::H5F_handle hFile(H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT));

				if (!hFile.valid()) ICEDB_throw(icedb::error::error_types::xBadInput)
					.add("Reason", "Cannot open a netCDF/HDF5 file.")
					.add("Filename", filename);

				auto verifyDatasetExists = [](hid_t file_id, const char* dataset_name) -> bool {
					if ((H5Lexists(file_id, dataset_name, H5P_DEFAULT) <= 0)) return false;
					H5O_info_t objinfo;
					if ((H5Oget_info_by_name(file_id, dataset_name, &objinfo, H5P_DEFAULT) < 0)) return false;
					if (objinfo.type != H5O_TYPE_DATASET) return false;
					return true;
				};

                //--------------------------------------------------------------------------------------//
				// psuAydinetal geometry files are either for GMM or DDA calculations. Handle both.
                //--------------------------------------------------------------------------------------//

                // Declare variables that cover both in scope.

                int  found;

                int32_t numPoints;
                vector<int32_t> particle_index;
				vector<int32_t> element_indices;
				vector<float>   x, y, z, rs;

                // Deal with the GMM and DDA file reads.

                if ((found = id.find("GMM")) != std::string::npos) {

				    // A valid PSU GMM file has these tables: particle_index, sphere_index, r, x, y, z.
				    // particle_index has one row, one column.
				    // The rest have one column and a number of rows that correspond to the number
                    // of spheres used to represent the particle.

				    if (   !verifyDatasetExists(hFile.h, "/particle_index")
				    	|| !verifyDatasetExists(hFile.h, "/sphere_index")
					    || !verifyDatasetExists(hFile.h, "/r")
					    || !verifyDatasetExists(hFile.h, "/x")
					    || !verifyDatasetExists(hFile.h, "/y")
					    || !verifyDatasetExists(hFile.h, "/z"))
					    ICEDB_throw(icedb::error::error_types::xBadInput)
					    .add("Reason", "This file does not have the proper structure for a Penn State geometry file.")
					    .add("Filename", filename);

				    // No need to read particle_index. Not being used.

				    readDataset<int32_t>(hFile.h, "/particle_index", particle_index);
				    readDataset<int32_t>(hFile.h, "/sphere_index"  , element_indices);
				    readDataset<float>(hFile.h, "/r", rs);
				    readDataset<float>(hFile.h, "/x",  x);
				    readDataset<float>(hFile.h, "/y",  y);
				    readDataset<float>(hFile.h, "/z",  z);

				    // Check that the read arrays have matching sizes.

				    numPoints = rs.size();

				    if (numPoints != x.size()              ) ICEDB_throw(icedb::error::error_types::xAssert);
				    if (numPoints != y.size()              ) ICEDB_throw(icedb::error::error_types::xAssert);
				    if (numPoints != z.size()              ) ICEDB_throw(icedb::error::error_types::xAssert);
				    if (numPoints != element_indices.size()) ICEDB_throw(icedb::error::error_types::xAssert);

                    // Done handling a GMM file.

                }
                else if ((found = id.find("DDA")) != std::string::npos) {

				    // A valid PSU GMM file has these tables: particle_index, sphere_index, r, x, y, z.
				    // particle_index has one row, one column.
				    // The rest have one column and a number of rows that correspond to the number
                    // of spheres used to represent the particle.

				    if (   !verifyDatasetExists(hFile.h, "/particle_index")
				    	|| !verifyDatasetExists(hFile.h, "/dipole_index")
					    || !verifyDatasetExists(hFile.h, "/x")
					    || !verifyDatasetExists(hFile.h, "/y")
					    || !verifyDatasetExists(hFile.h, "/z"))
					    ICEDB_throw(icedb::error::error_types::xBadInput)
					    .add("Reason", "This file does not have the proper structure for a Penn State geometry file.")
					    .add("Filename", filename);

				    // Open all of the datasets. Make sure that they have the correct dimensionality.
				    // Read the data into vectors. Verify that the data have the appropriate sizes.

				    vector<int32_t> xd, yd, zd;

				    // No need to read particle_index. Not being used.

				    readDataset<int32_t>(hFile.h, "/particle_index", particle_index);
				    readDataset<int32_t>(hFile.h, "/dipole_index"  , element_indices);
				    readDataset<int32_t>(hFile.h, "/x", xd);
				    readDataset<int32_t>(hFile.h, "/y", yd);
				    readDataset<int32_t>(hFile.h, "/z", zd);

				    // Check that the read arrays have matching sizes.

				    numPoints = xd.size();

				    if (numPoints != yd.size()             ) ICEDB_throw(icedb::error::error_types::xAssert);
				    if (numPoints != zd.size()             ) ICEDB_throw(icedb::error::error_types::xAssert);
				    if (numPoints != element_indices.size()) ICEDB_throw(icedb::error::error_types::xAssert);

                    // Save xd, yd, zd coordinates into float x, y, z coordinates.

                    x.resize(numPoints);
                    y.resize(numPoints);
                    z.resize(numPoints);

				    for (size_t i = 0; i < numPoints; ++i) {
                        x[i] = xd[i];
                        y[i] = yd[i];
                        z[i] = zd[i];
                    }

                    // Done handling a DDA file.

                }


                //--------------------------------------------------------------------------------------//
                // Identify the particle type from the filename.
                //--------------------------------------------------------------------------------------//

                char particleType[32];

                if      ((found = id.find("aggregate"))      != std::string::npos) {
                  sprintf(particleType, "Aggregate %05d",       particle_index[0]);
                }
                else if ((found = id.find("branchedplanar")) != std::string::npos) {
                  sprintf(particleType, "Branched Planar %05d", particle_index[0]);
                }
                else if ((found = id.find("column"))         != std::string::npos) {
                  sprintf(particleType, "Column %05d",          particle_index[0]);
                }
                else if ((found = id.find("graupel"))        != std::string::npos) {
                  sprintf(particleType, "Conical Graupel %05d", particle_index[0]);
                }
                else if ((found = id.find("plate"))          != std::string::npos) {
                  sprintf(particleType, "Plate %05d",           particle_index[0]);
                }

                //--------------------------------------------------------------------------------------//
				// Pack the data in the shpdata structure.
                //--------------------------------------------------------------------------------------//

				shpdata.required.number_of_particle_scattering_elements = static_cast<uint64_t>(numPoints);
				shpdata.required.number_of_particle_constituents = 1;
                shpdata.optional.particle_constituent_number.resize(1);
                shpdata.optional.particle_constituent_number[0] = 1;
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
                    shpdata.optional.particle_scattering_element_number[i]                = element_indices[i];
				}
                
				shpdata.optional.particle_constituent_single_name = "ice";

                if      ((found = id.find("GMM")) != std::string::npos) {
			        shpdata.optional.particle_scattering_element_radius = rs;
                }
                else if ((found = id.find("DDA")) != std::string::npos) {
			        shpdata.optional.particle_scattering_element_spacing = 0.001f; // 1 mm
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
