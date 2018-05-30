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

			/** \brief Reads a Penn State-style geometry file
			*
			* \todo Convert to using my "HDF for Humans" interface once it is ready.
			**/
			ShapeDataBasic readPSUfile(const std::string &filename)
			{
				using namespace std;
				ShapeDataBasic shpdata;
				// The particle index is not specific enough. Let's use the filename for an id.
				boost::filesystem::path p(filename);
				auto pfile = p.filename();
#if BOOST_VERSION < 104600
				string id = pfile.c_str(); // Needed for older RHEL machines
#else
				string id = pfile.string().c_str(); // Totally assuming a lack of non-Latin characters in the path.
#endif
				std::cout << "Reading id " << id << std::endl;

				ScopedHandles::H5F_handle hFile(H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT));
				if (!hFile.valid()) ICEDB_throw(icedb::error::error_types::xBadInput)
					.add("Reason", "Cannot open a netCDF/HDF5 file.")
					.add("Filename", filename);

				// A valid PSU file has these tables: particle_index, sphere_index, r, x, y, z.
				// particle_index has one row, one column.
				// The rest have one column, and a number of rows that correspond to the number of spheres used to represent the particle.

				auto verifyDatasetExists = [](hid_t file_id, const char* dataset_name) -> bool {
					if ((H5Lexists(file_id, dataset_name, H5P_DEFAULT) <= 0)) return false;
					H5O_info_t objinfo;
					if ((H5Oget_info_by_name(file_id, dataset_name, &objinfo, H5P_DEFAULT) < 0)) return false;
					if (objinfo.type != H5O_TYPE_DATASET) return false;
					return true;
				};

				if (!verifyDatasetExists(hFile.h, "/particle_index")
					|| !verifyDatasetExists(hFile.h, "/sphere_index")
					|| !verifyDatasetExists(hFile.h, "/r")
					|| !verifyDatasetExists(hFile.h, "/x")
					|| !verifyDatasetExists(hFile.h, "/y")
					|| !verifyDatasetExists(hFile.h, "/z"))
					ICEDB_throw(icedb::error::error_types::xBadInput)
					.add("Reason", "This file does not have the proper structure for a Penn State geometry file.")
					.add("Filename", filename);

				// Open all of the datasets. Make sure that they have the correct dimensionality.
				// Read the data into vectors. Verify that the data have the appropriate sizes.
				vector<float> xs, ys, zs, rs;
				vector<int32_t> sphere_indices;

				// No need to read particle_index. Not being used.
				readDataset<int32_t>(hFile.h, "/sphere_index", sphere_indices);
				readDataset<float>(hFile.h, "/r", rs);
				readDataset(hFile.h, "/x", xs);
				readDataset(hFile.h, "/y", ys);
				readDataset(hFile.h, "/z", zs);

				// Check that the read arrays have matching sizes.
				const size_t numPoints = rs.size();
				if (numPoints != xs.size()) ICEDB_throw(icedb::error::error_types::xAssert);
				if (numPoints != ys.size()) ICEDB_throw(icedb::error::error_types::xAssert);
				if (numPoints != zs.size()) ICEDB_throw(icedb::error::error_types::xAssert);
				if (numPoints != sphere_indices.size()) ICEDB_throw(icedb::error::error_types::xAssert);

				// Finally, pack the data in the shpdata structure.
				shpdata.required.number_of_particle_scattering_elements = static_cast<uint64_t>(numPoints);
				shpdata.required.number_of_particle_constituents = 1;
				shpdata.required.particle_id = id;
				shpdata.required.particle_scattering_element_coordinates_are_integral = false;

				/// VARIABLE: Cartesian coordinates of the center of each scattering element
				/// Written in form of x_1, y_1, z_1, x_2, y_2, z_2, ...
				/// Dimensions of [number_of_particle_scattering_elements][axis]
				shpdata.required.particle_scattering_element_coordinates.resize(3 * numPoints);
				for (size_t i = 0; i < numPoints; ++i) {
					shpdata.required.particle_scattering_element_coordinates[(3 * i) + 0] = xs[i];
					shpdata.required.particle_scattering_element_coordinates[(3 * i) + 1] = ys[i];
					shpdata.required.particle_scattering_element_coordinates[(3 * i) + 2] = zs[i];
				}

				shpdata.optional.particle_constituent_single_name = "ice";
				shpdata.required.particle_scattering_element_coordinates_scaling_factor = 0.001f; // 1 mm
				shpdata.required.particle_scattering_element_coordinates_units = "m";
				shpdata.optional.particle_scattering_element_radius = rs;


				return shpdata;
			}
		}
	}
}
