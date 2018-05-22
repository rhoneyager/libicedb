#pragma once

#include <gsl/multi_span>
#include <gsl/pointers>
#include <tuple>
#include <hdf5.h>
#include <hdf5_hl.h>

#include "Handles.hpp"
//#include "Tags.hpp"
#include "Types.hpp"
//#include "Groups.hpp"

namespace HH {
	namespace Files {
		using namespace Handles;
		using namespace gsl;


		[[nodiscard]] inline H5F_ScopedHandle&& open(
			not_null<const char*> filename,
			unsigned int FileOpenFlags,
			not_invalid<HH_hid_t> FileAccessPlist = H5P_DEFAULT)
		{
			H5F_ScopedHandle res(H5Fopen(filename, FileOpenFlags, FileAccessPlist.get().h));
			return std::move(res);
		}

		[[nodiscard]] inline H5F_ScopedHandle&& create(
			not_null<const char*> filename,
			unsigned int FileCreateFlags,
			not_invalid<HH_hid_t> FileCreationPlist = H5P_DEFAULT,
			not_invalid<HH_hid_t> FileAccessPlist = H5P_DEFAULT)
		{
			H5F_ScopedHandle res(H5Fcreate(filename, FileCreateFlags,
				FileCreationPlist.get().h, FileAccessPlist.get().h));
			return std::move(res);
		}

		/// \brief Mount a file into a group
		/// \returns >=0 on success, negative on failure
		[[nodiscard]] inline herr_t mount(
			not_invalid<HH_hid_t> destination_base,
			not_null<const char*> destination_groupname,
			not_invalid<HH_hid_t> source_file,
			not_invalid<HH_hid_t> FileMountPlist = H5P_DEFAULT)
		{
			return H5Fmount(destination_base.get().h, destination_groupname.get(),
				source_file().h, FileMountPlist.get().h);
		}

		/// \brief Unmount a file from a group
		/// \returns >=0 success, negative on failure
		[[nodiscard]] inline herr_t unmount(
			not_invalid<HH_hid_t> base_location,
			const char* mountpoint)
		{
			return H5Funmount(base_location.get().h, mountpoint);
		}

		/// \brief Load an image of an already-opened HDF5 file into system memory
		[[nodiscard]] inline ssize_t get_file_image(
			not_invalid<HH_hid_t> file_id,
			void* buf_ptr,
			size_t buf_len)
		{
			return H5Fget_file_image(file_id.get().h, buf_ptr, buf_len);
		}

		/// \brief Open a file image that is loaded into system memory as a regular HDF5 file
		[[nodiscard]] inline H5F_ScopedHandle&& open_file_image(
			not_null<void*> buf_ptr,
			size_t buf_size,
			unsigned int flags)
		{
			return std::move(H5F_ScopedHandle(H5LTopen_file_image(buf_ptr.get(), buf_size, flags)));
		}

		/// \brief Creates a new file image (i.e. a file that exists purely in memory)
		/// \param buffer_ptr is a pointer to the buffer to be manipulated. If null, then *buf_len is set to the minimum size, in bytes, needed to create this image.
		/// \param WritePhysicalFileOnClose determines whether a physical file is written upon close.
		[[nodiscard]] inline H5F_ScopedHandle&& create_file_image(
			not_null<const char*> filename,
			void* buffer_ptr,
			size_t buffer_len,
			bool backing_store_in_file = false,
			not_invalid<HH_hid_t> ImageCreationPlist = H5P_FILE_ACCESS)
		{
			const auto h5Result = H5Pset_fapl_core(ImageCreationPlist.get().h, buffer_len, backing_store_in_file);
			Expects(h5Result >= 0 && "H5Pset_fapl_core failed");
			// This new memory-only dataset needs to always be writable. The flags parameter
			// has little meaning in this context.
			/// \todo Check if truncation actually removes the file on the disk!!!!!
			return std::move(H5F_ScopedHandle(H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, ImageCreationPlist.get().h)));
		}

		/// \brief Get name of the file to which an object belongs
		/// \brief Is this path an HDF5 file?
		/// \breif Is this file opened for read/write or read/only access?
		/// \brief Reopen an already-open file (i.e. to strip mountpoints)


		class File
		{
			H5F_ScopedHandle::thisSharedHandle_t h;
		public:
			File() {}
			virtual ~File() {}
			static inline File open(
				not_null<const char*> filename,
				unsigned int FileOpenFlags,
				not_invalid<HH_hid_t> FileAccessPlist = H5P_DEFAULT
			) {
				auto hh = HH::Files::open(filename, FileOpenFlags, FileAccessPlist.get());
				File f;
				f.h = hh.make_shared();
				return f;
			}
			static inline File create(
				not_null<const char*> filename,
				unsigned int FileCreateFlags,
				not_invalid<HH_hid_t> FileCreationPlist = H5P_DEFAULT,
				not_invalid<HH_hid_t> FileAccessPlist = H5P_DEFAULT
			) {
				auto hh = HH::Files::create(filename, FileCreateFlags, FileCreationPlist.get(), FileAccessPlist.get());
				File f;
				f.h = hh.make_shared();
				return f;
			}
		};
	}

}
