#pragma once
#include <hdf5.h>
#include <gsl/pointers>
#include "Handles.hpp"
#include "Types.hpp"
#include "Attributes.hpp"

namespace HH {
	using namespace HH::Handles;
	using namespace HH::Types;
	using namespace gsl;
	using std::initializer_list;
	using std::tuple;
	struct Has_Groups;
	struct Group {
	private:
		H5G_ScopedHandle grp;
	public:
		Group(H5G_ScopedHandle&& hnd_grp) : grp(std::move(hnd_grp)), atts(grp.getWeakHandle()), grps(grp.getWeakHandle()) {}
		Group(not_invalid<HH_hid_t> hnd) : grp(hnd.get().release()), atts(grp.getWeakHandle()), grps(grp.getWeakHandle()) {}
		Group(Group &&old) : grp(-1, false), atts(grp.getWeakHandle()), grps(grp.getWeakHandle()) {
			grp.swap(old.grp);
			/// \todo Check how weak objects behave on swap!
			//atts.swap
		}
		virtual ~Group() {}

		Has_Attributes atts;
		Has_Groups grps;
		//Has_Mountpoints mnts;
	};

	struct Has_Groups {
	private:
		typedef WeakHandle<hid_t, InvalidHDF5Handle> base_t;
		/// \note This is a weak object! It does not close.
		base_t base;
	public:
		Has_Groups(base_t obj) : base(obj) {}
		virtual ~Has_Groups() {}

	};

	namespace Groups {
		using namespace Handles;
		using namespace gsl;
		/// \brief Open a group
		/// \returns an invalid handle if an error occurred
		/// \returns a scoped handle to the group upon success
		/// \note It is possible to have multiple handles opened for the group
		/// simultaneously. HDF5 has its own reference counting implementation.
		[[nodiscard]] inline H5G_ScopedHandle openGroup(
			const not_invalid<HH_hid_t> &base,
			not_null<const char*> name,
			not_invalid<HH_hid_t> GroupAccessPlist = H5P_DEFAULT)
		{
			H5G_ScopedHandle res(H5Gopen(base.get().h, name.get(), GroupAccessPlist.get().h));
			return std::move(res);
		}
		/// \brief Does a group exist at the specified path
		/// \returns <0 on failure (e.g. parent path not found)
		/// \returns 0 if the group does not exist
		/// returns >0 if the group does exist
		[[nodiscard]] inline htri_t groupExists(
			const not_invalid<HH_hid_t> &base,
			not_null<const char*> name,
			not_invalid<HH_hid_t> LinkAccessPlist = H5P_DEFAULT)
		{
			htri_t linkExistsFlag = H5Lexists(base().h, name.get(), LinkAccessPlist().h);
			if (linkExistsFlag < 0) return -1;
			if (linkExistsFlag == 0) return 0;
			// Check that the object is a group
			H5O_info_t obj_info;
			herr_t err = H5Oget_info_by_name(base().h, name.get(), &obj_info, LinkAccessPlist().h);
			if (err < 0) return -1;
			if (obj_info.type == H5O_TYPE_GROUP) return 1;
			return 0;
		}
		/// \brief Create a group
		/// \returns an invalid handle on failure.
		/// \returns a scoped handle to the group on success
		[[nodiscard]] inline H5G_ScopedHandle createGroup(
			not_invalid<HH_hid_t> base,
			not_null<const char*> name,
			not_invalid<HH_hid_t> LinkCreationPlist = H5P_DEFAULT,
			not_invalid<HH_hid_t> GroupCreationPlist = H5P_DEFAULT,
			not_invalid<HH_hid_t> GroupAccessPlist = H5P_DEFAULT)
		{
			H5G_ScopedHandle res(H5Gcreate(
				base.get().h,
				name.get(),
				LinkCreationPlist.get().h,
				GroupCreationPlist.get().h,
				GroupAccessPlist.get().h));
			return std::move(res);
		}
	}
}
