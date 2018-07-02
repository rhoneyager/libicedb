#pragma once
#include <cstdio>
#include <gsl/multi_span>
#include <gsl/pointers>
#include <tuple>
#include <hdf5.h>
#include <hdf5_hl.h>

#include "Handles.hpp"
//#include "Tags.hpp"
#include "Types.hpp"
#include "Groups.hpp"
#include "Attributes.hpp"
#include "Datasets.hpp"

namespace HH {
	using namespace Handles;
	using namespace gsl;
	using namespace HH::Handles;
	using namespace HH::Types;
	using namespace gsl;
	using std::initializer_list;
	using std::tuple;

	struct File : public Group {
	private:
		HH_hid_t base;
	public:
		File(HH_hid_t hnd) : base(hnd), atts(hnd), Group(hnd), dsets(hnd) {}
		virtual ~File() {}
		HH_hid_t get() const { return base; }

		Has_Attributes atts;
		//Has_Groups grps;
		Has_Datasets dsets;

		[[nodiscard]] herr_t get_info(H5F_info_t &info) const {
			herr_t err = H5Fget_info(base(), &info);
			if (err < 0) return err;
			return 1;
		}

		/// \note Should ideally be open, but the Group::open functions get masked.
		[[nodiscard]] static File openFile(
			not_null<const char*> filename,
			unsigned int FileOpenFlags,
			HH_hid_t FileAccessPlist = H5P_DEFAULT)
		{
			hid_t res = H5Fopen(filename, FileOpenFlags, FileAccessPlist());
			Expects(res >= 0);
			return File(HH_hid_t(res, Closers::CloseHDF5File::CloseP));
		}

		/// \note Should ideally be create, but the Group::create functions get masked.
		[[nodiscard]] static File createFile(
			not_null<const char*> filename,
			unsigned int FileCreateFlags,
			HH_hid_t FileCreationPlist = H5P_DEFAULT,
			HH_hid_t FileAccessPlist = H5P_DEFAULT)
		{
			hid_t res = H5Fcreate(filename, FileCreateFlags,
				FileCreationPlist(), FileAccessPlist());
			Expects(res >= 0);
			return File(HH_hid_t(res, Closers::CloseHDF5File::CloseP));
		}

		/// \brief Load an image of an already-opened HDF5 file into system memory
		[[nodiscard]] static ssize_t get_file_image(
			HH_hid_t file_id,
			void* buf_ptr,
			size_t buf_len)
		{
			return H5Fget_file_image(file_id(), buf_ptr, buf_len);
		}

		/// \brief Open a file image that is loaded into system memory as a regular HDF5 file
		[[nodiscard]] static File open_file_image(
			not_null<void*> buf_ptr,
			size_t buf_size,
			unsigned int flags)
		{
			hid_t res = H5LTopen_file_image(buf_ptr.get(), buf_size, flags);
			Expects(res >= 0);
			return File(HH_hid_t(res, Closers::CloseHDF5File::CloseP));
		}

		/// \brief Create a new file image (i.e. a file that exists purely in memory)
		/// \param filename is the file to write, if backing_store_in_file == true
		/// \param block_allocation_len is the size of each new allocation as the file grows
		/// \param backing_store_in_file determines whether a physical file is written upon close.
		[[nodiscard]] static File create_file_image(
			not_null<const char*> filename,
			size_t block_allocation_len = 10000000, // 10 MB
			bool backing_store_in_file = false,
			HH_hid_t ImageCreationPlist = HH_hid_t(H5Pcreate(H5P_FILE_ACCESS), Closers::CloseHDF5PropertyList::CloseP))
		{
			const auto h5Result = H5Pset_fapl_core(ImageCreationPlist(), block_allocation_len, backing_store_in_file);
			Expects(h5Result >= 0 && "H5Pset_fapl_core failed");
			// This new memory-only dataset needs to always be writable. The flags parameter
			// has little meaning in this context.
			/// \todo Check if truncation actually removes the file on the disk!!!!!
			hid_t res = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, ImageCreationPlist());
			Expects(res >= 0);
			return File(HH_hid_t(res, Closers::CloseHDF5File::CloseP));
		}

		// TODO: Creates a new file image (i.e. a file that exists purely in memory)
		// Add a function that will use only a pre-allocated buffer. No filename needed, as this will NEVER be written.

		/// \brief Get name of the file to which an object belongs
		/// \brief Is this path an HDF5 file?
		/// \breif Is this file opened for read/write or read/only access?
		/// \brief Reopen an already-open file (i.e. to strip mountpoints)

	};


}
