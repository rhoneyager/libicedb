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
				struct InvalidHDF5FileHandle {
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

				/// \todo Extend with SFINAE and constexpr type detection for InvalidValueClass and CloseMethod.
				/// If these are not specified as template parameters, then they must be passed in the constructor.
				template <typename HandleType, class InvalidValueClass, class CloseMethod>
				struct ScopedHandle
				{
					HandleType h = NULL;
					bool valid() const { return !InvalidValueClass::isInvalid(h); }
					~ScopedHandle() {
						if (valid) CloseMethod::Close(h);
						h = NULL;
					}
					ScopedHandle(HandleType newh) : h(newh) {}
				};

				typedef ScopedHandle<hid_t, InvalidHDF5FileHandle, CloseHDF5File> H5F_handle;

			}
			/** \brief Reads a Penn State-style geometry file
			*
			* \todo Convert to using my "HDF for Humans" interface once it is ready.
			**/
			ShapeDataBasic readPSUfile(const std::string &filename)
			{
				using namespace std;
				ShapeDataBasic res;
				// The particle index is not specific enough. Let's use the filename for an id.
				boost::filesystem::path p(filename);
				auto pfile = p.filename();
				string id = pfile.string().c_str(); // Totally assuming a lack of non-Latin characters in the path.

				ScopedHandles::H5F_handle hFile(H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT));
				if (!hFile.valid()) ICEDB_throw(icedb::error::error_types::xBadInput)
					.add("Reason", "Cannot open a netCDF/HDF5 file.")
					.add("Filename", filename);

				// A valid PSU file has these tables: particle_index, sphere_index, r, x, y, z.
				// particle_index has one row, one column.
				// The rest have one column, and a number of rows that correspond to the number of spheres used to represent the particle.
				
				// Check that all of the datasets exist
				// The map has two components, the dataset name (string) and its HDF5 identifier (hid_t).
				map<string, hid_t> datasetNames;
				H5G_info_t info;
				herr_t err = H5Gget_info(hFile.h, &info);
				if (err < 0) ICEDB_throw(icedb::error::error_types::xBadInput)
					.add("Reason", "HDF5 error when reading a file.")
					.add("Filename", filename);

				for (hsize_t i = 0; i < info.nlinks; ++i) {
					//ssize_t sz = H5Lget_name_by_idx()
				}


				return res;
			}
		}
	}
}
