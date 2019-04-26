#pragma once
#include "defs.hpp"
#include <hdf5.h>
#include "gsl/gsl"
#include "Handles.hpp"
#include "Types.hpp"
#include "Attributes.hpp"
#include "Datasets.hpp"
#include "Errors.hpp"

namespace HH {
	using namespace HH::Handles;
	using namespace HH::Types;
	using std::initializer_list;
	using std::tuple;

	struct GroupParameterPack
	{
		AttributeParameterPack atts;
		HH_hid_t GroupAccessPlist = H5P_DEFAULT;

		struct GroupCreationPListProperties {
			HH_hid_t GroupCreationPlistCustom = H5P_DEFAULT;
			bool UseCustomGroupCreationPlist = true;
			bool set_link_creation_order = true;

			HH_hid_t generateGroupCreationPlist() const {
				if (UseCustomGroupCreationPlist) return GroupCreationPlistCustom;
				hid_t plid = H5Pcreate(H5P_GROUP_CREATE);
				if (plid < 0) throw HH_throw;
				if (set_link_creation_order) {
					auto cres = H5Pset_create_intermediate_group(plid, H5P_CRT_ORDER_TRACKED & H5P_CRT_ORDER_INDEXED);
					if(cres < 0) throw HH_throw;
				}
				HH_hid_t pl(plid, Handles::Closers::CloseHDF5PropertyList::CloseP);
				return pl;
			}
		} groupCreationProperties;

		struct LinkCreationPListProperties {
			HH_hid_t LinkCreationPlistCustom = H5P_DEFAULT;
			bool UseCustomLinkCreationPlist = false;

			bool CreateIntermediateGroups = false;
			HH_hid_t generateLinkCreationPlist() const {
				if (UseCustomLinkCreationPlist) return LinkCreationPlistCustom;
				hid_t plid = H5Pcreate(H5P_LINK_CREATE);
				HH_Expects(plid >= 0);
				if (CreateIntermediateGroups) {
					auto cres = H5Pset_create_intermediate_group(plid, 1);
					HH_Expects(cres >= 0);
				}
				HH_hid_t pl(plid, Handles::Closers::CloseHDF5PropertyList::CloseP);
				return pl;
			}
		} linkCreationProperties;

		GroupParameterPack() {}
		GroupParameterPack(const AttributeParameterPack& a,
			HH_hid_t GroupAccessPlist = H5P_DEFAULT
		) : atts(a), GroupAccessPlist(GroupAccessPlist) {}
	};

	struct Group {
	private:
		HH_hid_t base;
	public:
		Group() : Group(HH_hid_t{}) {}
		Group(HH_hid_t hnd) : base(hnd), atts(hnd), dsets(hnd) { } //, grps(hnd) {}
		Group(Group &g) : base(g.get()), atts(g.get()), dsets(g.get()) { }
		virtual ~Group() {}
		HH_hid_t get() const { return base; }

		H5G_info_t& get_info(H5G_info_t& info) const {
			HH_Expects(isGroup());
			herr_t err = H5Gget_info(base(), &info);
			HH_Expects(err >= 0);
			return info;
		}

		Has_Attributes atts;
		Has_Datasets dsets;

		static bool isGroup(HH_hid_t obj) {
			H5I_type_t typ = H5Iget_type(obj());
			if (typ == H5I_BADID) return false;
			if ((typ == H5I_GROUP) || (typ == H5I_FILE)) return true;
			return false;
		}
		bool isGroup() const { return isGroup(base); }

		/// \brief List all groups under this group
		std::vector<std::string> list() const {
			std::vector<std::string> res;
			H5G_info_t info;
			herr_t e = H5Gget_info(base(), &info);
			HH_Expects(e >= 0);
			for (hsize_t i = 0; i < info.nlinks; ++i) {
				// Get the name
				ssize_t szName = H5Lget_name_by_idx(base(), ".", H5_INDEX_NAME, H5_ITER_INC,
					i, NULL, 0, H5P_DEFAULT);
				HH_Expects(szName >= 0);
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


		/// \brief Does a group exist at the specified path?
		inline bool exists(
			const std::string & name,
			HH_hid_t LinkAccessPlist = H5P_DEFAULT)
		{
			HH_Expects(isGroup());
			htri_t linkExistsFlag = H5Lexists(base(), name.c_str(), LinkAccessPlist());
			if (linkExistsFlag < 0) throw HH_throw;
			if (linkExistsFlag == 0) return false;
			// Check that the object is a group
			H5O_info_t obj_info;
			herr_t err = H5Oget_info_by_name(base(), name.c_str(), &obj_info, LinkAccessPlist());
			if (err < 0) throw HH_throw;
			if (obj_info.type == H5O_TYPE_GROUP) return true;
			return false;
		}

		/// \brief Create a group
		/// \returns an invalid handle on failure.
		/// \returns a scoped handle to the group on success
		Group create(
			const std::string & name,
			GroupParameterPack gp = GroupParameterPack())
		{
			HH_Expects(isGroup());
			hid_t res = H5Gcreate(
				base(),
				name.c_str(),
				gp.linkCreationProperties.generateLinkCreationPlist()(),
				gp.groupCreationProperties.generateGroupCreationPlist()(),
				gp.GroupAccessPlist());
			HH_Expects(res >= 0);
			HH_hid_t h(res, Closers::CloseHDF5Group::CloseP);
			gp.atts.apply(h);
			return Group(h);
		}

		/// \brief Open a group
		/// \returns an invalid handle if an error occurred
		/// \returns a scoped handle to the group upon success
		/// \note It is possible to have multiple handles opened for the group
		/// simultaneously. HDF5 has its own reference counting implementation.
		inline Group open(
			const std::string & name,
			HH_hid_t GroupAccessPlist = H5P_DEFAULT)
		{
			HH_Expects(isGroup());
			hid_t g = H5Gopen(base(), name.c_str(), GroupAccessPlist());
			HH_Expects(g >= 0);
			return Group(HH_hid_t(g, Closers::CloseHDF5Group::CloseP));
		}

		/// \brief Mount a file into a group
		/// \returns >=0 on success, negative on failure
		void mount(
			const std::string & destination_groupname,
			HH_hid_t source_file,
			HH_hid_t FileMountPlist = H5P_DEFAULT)
		{
			auto res = H5Fmount(base(), destination_groupname.c_str(),
				source_file(), FileMountPlist());
			HH_Expects(res >= 0);
		}

		/// \brief Unmount a file from a group
		/// \returns >=0 success, negative on failure
		void unmount(
			const std::string & mountpoint)
		{
			auto res = H5Funmount(base(), mountpoint.c_str());
			HH_Expects(res >= 0);
		}
	};

	typedef Group Has_Groups;
}
