#pragma once
#include <hdf5.h>
#include <gsl/pointers>
#include "Handles.hpp"
#include "Types.hpp"
#include "Attributes.hpp"
#include "Datasets.hpp"

namespace HH {
	using namespace HH::Handles;
	using namespace HH::Types;
	using namespace gsl;
	using std::initializer_list;
	using std::tuple;

	struct Group {
	private:
		HH_hid_t base;
	public:
		Group(HH_hid_t hnd) : base(hnd), atts(hnd), dsets(hnd) { Expects(isGroup()); } //, grps(hnd) {}
		virtual ~Group() {}
		HH_hid_t get() const { return base; }

		/// \todo Redo this for C++17. Look at tl::Expected.
		herr_t get_info(H5G_info_t &info) const {
			herr_t err = H5Gget_info(base(), &info);
			if (err < 0) return err;
			return 1;
		}

		Has_Attributes atts;
		Has_Datasets dsets;

		static bool isGroup(HH_hid_t obj) {
			H5I_type_t typ = H5Iget_type(obj());
			if ((typ == H5I_GROUP) || (typ == H5I_FILE)) return true;
			//H5O_info_t oinfo;
			//herr_t err = H5Oget_info(obj(), &oinfo);
			//if (err < 0) return false;
			//if (oinfo.type == H5O_type_t::H5O_TYPE_GROUP) return true;
		}
		bool isGroup() const { return isGroup(base); }

		/// \brief Does a group exist at the specified path
		/// \returns <0 on failure (e.g. parent path not found)
		/// \returns 0 if the group does not exist
		/// returns >0 if the group does exist
		inline htri_t exists(
			not_null<const char*> name,
			HH_hid_t LinkAccessPlist = H5P_DEFAULT)
		{
			htri_t linkExistsFlag = H5Lexists(base(), name.get(), LinkAccessPlist());
			if (linkExistsFlag < 0) return -1;
			if (linkExistsFlag == 0) return 0;
			// Check that the object is a group
			H5O_info_t obj_info;
			herr_t err = H5Oget_info_by_name(base(), name.get(), &obj_info, LinkAccessPlist());
			if (err < 0) return -1;
			if (obj_info.type == H5O_TYPE_GROUP) return 1;
			return 0;
		}

		/// \brief Create a group
		/// \returns an invalid handle on failure.
		/// \returns a scoped handle to the group on success
		Group create(
			not_null<const char*> name,
			HH_hid_t LinkCreationPlist = H5P_DEFAULT,
			HH_hid_t GroupCreationPlist = H5P_DEFAULT,
			HH_hid_t GroupAccessPlist = H5P_DEFAULT)
		{
			hid_t res = H5Gcreate(
				base(),
				name.get(),
				LinkCreationPlist(),
				GroupCreationPlist(),
				GroupAccessPlist());
			Expects(res >= 0);
			return Group(HH_hid_t(res, Closers::CloseHDF5Group::CloseP));
		}

		/// \brief Open a group
		/// \returns an invalid handle if an error occurred
		/// \returns a scoped handle to the group upon success
		/// \note It is possible to have multiple handles opened for the group
		/// simultaneously. HDF5 has its own reference counting implementation.
		inline Group open(
			not_null<const char*> name,
			HH_hid_t GroupAccessPlist = H5P_DEFAULT)
		{
			hid_t g = H5Gopen(base(), name.get(), GroupAccessPlist());
			Expects(g >= 0);
			return Group(HH_hid_t(g, Closers::CloseHDF5Group::CloseP));
		}

		/// \brief Mount a file into a group
		/// \returns >=0 on success, negative on failure
		[[nodiscard]] herr_t mount(
			not_null<const char*> destination_groupname,
			HH_hid_t source_file,
			HH_hid_t FileMountPlist = H5P_DEFAULT)
		{
			return H5Fmount(base(), destination_groupname.get(),
				source_file(), FileMountPlist());
		}

		/// \brief Unmount a file from a group
		/// \returns >=0 success, negative on failure
		[[nodiscard]] herr_t unmount(
			const char* mountpoint)
		{
			return H5Funmount(base(), mountpoint);
		}
	};

	typedef Group Has_Groups;
}
