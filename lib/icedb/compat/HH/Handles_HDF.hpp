#pragma once
#include <hdf5.h>

namespace HH {
	namespace Handles {
		/// Detect invalid HDF5 ids
		struct InvalidHDF5Handle {
			static inline bool isValid(hid_t h) {
				htri_t res = H5Iis_valid(h);
				if (res <= 0)
				{
					return false;
				}
				return true;
			}
			static inline bool isInvalid(hid_t h) {
				return !isValid(h);
			}
		};
		/// Automatically close out-of-scope HDF5 objects
		namespace Closers {
			struct CloseHDF5Attribute {
				static inline bool Close(hid_t h) {
					herr_t err = H5Aclose(h);
					if (err < 0) return false;
					return true;
				}
			};
			struct CloseHDF5File {
				static inline bool Close(hid_t h) {
					herr_t err = H5Fclose(h);
					if (err < 0) return false;
					return true;
				}
			};
			struct CloseHDF5Dataset {
				static inline bool Close(hid_t h) {
					herr_t err = H5Dclose(h);
					if (err < 0) return false;
					return true;
				}
			};
			struct CloseHDF5Dataspace {
				static inline bool Close(hid_t h) {
					herr_t err = H5Sclose(h);
					if (err < 0) return false;
					return true;
				}
			};
			struct CloseHDF5Datatype {
				static inline bool Close(hid_t h) {
					herr_t err = H5Tclose(h);
					if (err < 0) return false;
					return true;
				}
			};
			struct CloseHDF5Group {
				static inline bool Close(hid_t h) {
					herr_t err = H5Gclose(h);
					if (err < 0) return false;
					return true;
				}
			};
			struct CloseHDF5PropertyList {
				static inline bool Close(hid_t h) {
					herr_t err = H5Pclose(h);
					if (err < 0) return false;
					return true;
				}
			};
			struct DoNotClose {
				static inline bool Close(hid_t) { return true; }
			};
		}
	}
}
