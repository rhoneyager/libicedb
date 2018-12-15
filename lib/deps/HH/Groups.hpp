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

	enum class RecursionType {
		BASE,
		ONE,
		SUBTREE
	};

	struct Group {
	private:
		HH_hid_t base;
	public:
		Group(HH_hid_t hnd) : base(hnd), atts(hnd), dsets(hnd) {} //, grps(hnd) {}
		virtual ~Group() {}
		HH_hid_t get() const { return base; }

		/// \todo Redo this for C++17. Look at tl::Expected.
		herr_t get_info(H5G_info_t &info) const {
			Expects(isGroup());
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
			return false;
		}
		bool isGroup() const { return isGroup(base); }

		/// \brief Does a group exist at the specified path
		/// \returns <0 on failure (e.g. parent path not found)
		/// \returns 0 if the group does not exist
		/// \returns >0 if the group does exist
		/// \todo Need to check that intermediate objects exist, and that
		///       these intermediate objects can have children. Otherwise,
		///       H5Lexists can trigger a core dump.
		inline htri_t exists(
			not_null<const char*> name,
			HH_hid_t LinkAccessPlist = H5P_DEFAULT)
		{
			Expects(isGroup());
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
			Expects(isGroup());
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
			Expects(isGroup());
			hid_t g = H5Gopen(base(), name.get(), GroupAccessPlist());
			Expects(g >= 0);
			return Group(HH_hid_t(g, Closers::CloseHDF5Group::CloseP));
		}

		/// \brief Mount a file into a group
		/// \returns >=0 on success, negative on failure
		HH_NODISCARD herr_t mount(
			not_null<const char*> destination_groupname,
			HH_hid_t source_file,
			HH_hid_t FileMountPlist = H5P_DEFAULT)
		{
			return H5Fmount(base(), destination_groupname.get(),
				source_file(), FileMountPlist());
		}

		/// \brief Unmount a file from a group
		/// \returns >=0 success, negative on failure
		HH_NODISCARD herr_t unmount(
			const char* mountpoint)
		{
			return H5Funmount(base(), mountpoint);
		}

		/// Get names of all child groups
		std::vector<std::string> listChildGroupNames() const {
			std::vector<std::string> res;
			H5G_info_t info;
			herr_t e = H5Gget_info(base(), &info);
			Expects(e >= 0);
			for (hsize_t i = 0; i < info.nlinks; ++i) {
				// Get the name
				ssize_t szName = H5Lget_name_by_idx(base(), ".", H5_INDEX_NAME, H5_ITER_INC,
					i, NULL, 0, H5P_DEFAULT);
				Expects(szName >= 0);
				std::vector<char> vName(szName + 1, '\0');
				H5Lget_name_by_idx(base(), ".", H5_INDEX_NAME, H5_ITER_INC,
					i, vName.data(), szName + 1, H5P_DEFAULT);

				// Get the object and check the type
				H5O_info_t oinfo;
				herr_t err = H5Oget_info_by_name(base(), vName.data(), &oinfo, H5P_DEFAULT); // H5P_DEFAULT only, per docs.
				if (err < 0) continue;
				if (oinfo.type == H5O_type_t::H5O_TYPE_GROUP) res.push_back(std::string(vName.data()));
			}
			return res;
		}

		/// Search for a group using the current group as a base.
		/// Follows a recursion policy. Recursive searches assume that the graph is not cyclic.
		/// Allows for the specification of a maximum depth.
		HH_NODISCARD herr_t search(
			std::function<bool(const HH::Group&)> searchFunc,
			std::vector<HH::Group> &res,
			size_t max_depth = 0,
			HH::RecursionType ret = RecursionType::ONE,
			bool followSymLinks = true,
			bool followHardLinks = true,
			bool followExtLinks = true)
		{
			if (!isGroup()) return -1;
			// Search the BASE
			if (ret != RecursionType::ONE)
				if (searchFunc(*this)) {
					res.push_back(*this);
				}
			// Search children / subtree
			if (ret != RecursionType::BASE) {
				auto childGrpNames = listChildGroupNames();
				for (const auto &n : childGrpNames) {
					auto child = this->open(n.c_str());
					RecursionType childRecur = RecursionType::BASE;
					size_t newDepth = max_depth;
					if (ret == RecursionType::SUBTREE && max_depth==1)
						childRecur = RecursionType::ONE;
					else if (ret == RecursionType::SUBTREE && max_depth == 0)
						childRecur = RecursionType::SUBTREE;
					else {
						childRecur = RecursionType::SUBTREE;
						newDepth--;
					}
					herr_t childErr = child.search(searchFunc, res, newDepth, childRecur);
					if (childErr < 0) return childErr;
				}
			}
			return 0;
		}

		HH_NODISCARD herr_t search(
			const std::string &id_regex = "",
			const std::string &group_obj_type_regex = "",
			size_t max_depth = 0) { return 0; }
	};

	typedef Group Has_Groups;
}
