#pragma once
#include "defs.hpp"
#include <cstdio>
#include "gsl/gsl"
#include <tuple>
#include <hdf5.h>
#include <hdf5_hl.h>

#include "Errors.hpp"
#include "Handles.hpp"
#include "Types.hpp"
#include "Groups.hpp"
#include "Attributes.hpp"
#include "Datasets.hpp"

namespace HH {
	using namespace HH::Handles;
	using namespace HH::Types;
	using std::initializer_list;
	using std::tuple;

	struct HH_DL File : public Group {
	private:
		HH_hid_t base;
	public:
		File();
		File(HH_hid_t hnd);
		virtual ~File();
		HH_hid_t get() const;

		Has_Attributes atts;
		//Has_Groups grps;
		Has_Datasets dsets;

		H5F_info_t& get_info(H5F_info_t& info) const;

		/// \note Should ideally be open, but the Group::open functions get masked.
		HH_NODISCARD static File openFile(
			const std::string& filename,
			unsigned int FileOpenFlags,
			HH_hid_t FileAccessPlist = H5P_DEFAULT);

		/// \note Should ideally be create, but the Group::create functions get masked.
		HH_NODISCARD static File createFile(
			const std::string& filename,
			unsigned int FileCreateFlags,
			HH_hid_t FileCreationPlist = H5P_DEFAULT,
			HH_hid_t FileAccessPlist = H5P_DEFAULT);

		/// \brief Load an image of an already-opened HDF5 file into system memory
		HH_NODISCARD static ssize_t get_file_image(
			HH_hid_t file_id,
			void* buf_ptr,
			size_t buf_len);

		/// \brief Open a file image that is loaded into system memory as a regular HDF5 file
		HH_NODISCARD static File open_file_image(
			gsl::not_null<void*> buf_ptr,
			size_t buf_size,
			unsigned int flags);

		/// \brief Creates a new file image (i.e. a file that exists purely in memory)
		HH_NODISCARD static File create_file_mem(
			const std::string& filename,
			size_t increment_len = 1000000, // 1 MB
			bool flush_on_close = false);

		HH_NODISCARD static std::string genUniqueFilename();

		/// \brief Get name of the file to which an object belongs
		/// \brief Is this path an HDF5 file?
		/// \breif Is this file opened for read/write or read/only access?
		/// \brief Reopen an already-open file (i.e. to strip mountpoints)

	};
}
