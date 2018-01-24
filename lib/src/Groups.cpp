#include "../icedb/Group.hpp"
#include "../icedb/compat/hdf5_load.h"
#include "../private/hdf5_supplemental.hpp"
#include "../private/Group_impl.hpp"
#include <gsl/gsl_assert>

namespace icedb {
	namespace Groups {
		Group::Group() : name{} {}
		Group::Group(const std::string &name) : name{ name } {}
		Group_impl::Group_impl() : Group() {}

		std::string Group::_icedb_obj_type_identifier = { "_icedb_obj_type" };

		Group::~Group() {}

		Group_impl::~Group_impl() {}

		Group_impl::Group_impl(const std::string &name, gsl::not_null<H5::Group*> parent)
			: Group{ name }
		{
			auto ugrp = fs::hdf5::openGroup(parent.get(), name.c_str());
			//grp = std::shared_ptr<H5::Group>(ugrp.release());
			grp = std::shared_ptr<H5::Group>(ugrp.release()); // , mem::icedb_delete<H5::Group>());
			this->_setAttributeParent(grp);
			this->_setTablesParent(grp);
		}
		Group_impl::Group_impl(const std::string &name, gsl::not_null<H5::H5File*> parent)
			: Group{ name }
		{
			auto ugrp = fs::hdf5::openGroup(parent.get(), name.c_str());
			//grp = std::shared_ptr<H5::Group>(ugrp.release());
			grp = std::shared_ptr<H5::Group>(ugrp.release()); // , mem::icedb_delete<H5::Group>());
			this->_setAttributeParent(grp);
			this->_setTablesParent(grp);
		}


		Group_impl::Group_impl(const std::string &name, gsl::not_null<const Group*> parent)
			: Group{ name }
		{
			auto ugrp = fs::hdf5::openGroup(parent->getHDF5Group().get(), name.c_str());
			//grp = std::shared_ptr<H5::Group>(ugrp.release());
			grp = std::shared_ptr<H5::Group>(ugrp.release()); // , mem::icedb_delete<H5::Group>());
			this->_setAttributeParent(grp);
			this->_setTablesParent(grp);
		}

		Group_impl::Group_impl(Group_HDF_shared_ptr grp)
			: Group( "UNKNOWN" )
		{
			this->grp = grp;
			this->_setAttributeParent(grp);
			this->_setTablesParent(grp);
		}

		std::shared_ptr<H5::Group> Group_impl::getHDF5Group() const {
			return grp;
		}

		Group::Group_ptr Group_impl::createGroup(const std::string &groupName) {
			return std::move(Group::createGroup(groupName, this->grp.get()));
		}

		/// This exists because HDF5's API changed.
		template<class HPointerType>
		Group::Group_ptr _Impl_createGroup(const std::string &groupName, gsl::not_null<HPointerType*> parent) {
			//_impl->grp->createGroup(groupName); // Bad for NetCDF. See http://www.unidata.ucar.edu/software/netcdf/docs/file_format_specifications.html#creation_order
			hid_t baseGrpID = parent->getId();
			/* Create group, with link_creation_order set in the group
			* creation property list. */
			// No suitable C++ methods found.
			
			hid_t gcpl_id = H5Pcreate(H5P_GROUP_CREATE);
			assert(gcpl_id >= 0);
			assert(H5Pset_link_creation_order(gcpl_id, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED) >= 0);
			assert(H5Pset_attr_creation_order(gcpl_id, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED) >= 0);
			hid_t newGrp_id = H5Gcreate2(baseGrpID, groupName.c_str(), H5P_DEFAULT, gcpl_id, H5P_DEFAULT);
			assert(newGrp_id >= 0);
			assert(H5Gclose(newGrp_id) >= 0);
			assert(H5Pclose(gcpl_id) >= 0);

			return std::make_unique<Group_impl>(groupName, parent);
			//return Group::Group_ptr(new Group_impl(groupName, parent));
		}
#if ICEDB_H5_UNIFIED_GROUP_FILE == 1
		Group::Group_ptr Group::createGroup(const std::string &groupName, gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> parent) {
			return _Impl_createGroup<ICEDB_H5_GROUP_OWNER>(groupName, parent);
		}
#else
		Group::Group_ptr Group::createGroup(const std::string &groupName, gsl::not_null<H5::Group*> parent) {
			return _Impl_createGroup<H5::Group>(groupName, parent);
		}
	
		Group::Group_ptr Group::createGroup(const std::string &groupName, gsl::not_null<H5::H5File*> parent) {
			return _Impl_createGroup<H5::H5File>(groupName, parent);
		}
#endif	

	
		Group::Group_ptr Group::createGroup(const std::string &name, gsl::not_null<const Group*> parent) {
			return std::move(Group::createGroup(name, parent->getHDF5Group().get()));
		}



		Group::Group_ptr Group_impl::openGroup(const std::string &groupName) const {
			return std::make_unique<Group_impl>(groupName, grp.get());
			//return std::move(Group::Group_ptr(new Group_impl(groupName, grp.get())));
		}
	

#if ICEDB_H5_UNIFIED_GROUP_FILE == 1	
		Group::Group_ptr Group::openGroup(const std::string &name, gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> parent) {
			return std::make_unique<Group_impl>(name, parent);
			//return std::move(Group::Group_ptr( new Group_impl(name, parent)));
		}
#else
		Group::Group_ptr Group::openGroup(const std::string &name, gsl::not_null<H5::Group*> parent) {
			return std::make_unique<Group_impl>(name, parent);
			//return std::move(Group::Group_ptr( new Group_impl(name, parent)));
		}
		Group::Group_ptr Group::openGroup(const std::string &name, gsl::not_null<H5::H5File*> parent) {
			return std::make_unique<Group_impl>(name, parent);
			//return std::move(Group::Group_ptr( new Group_impl(name, parent)));
		}

#endif

		Group::Group_ptr Group::openGroup(Group_HDF_shared_ptr parent) {
			return std::make_unique<Group_impl>(parent);
		}

		Group::Group_ptr Group::openGroup(const std::string &name, gsl::not_null<const Group*> parent) {
			return std::move(Group::openGroup(name, parent->getHDF5Group().get()));
		}

		void Group_impl::deleteGroup(const std::string &groupName) {
			grp->unlink(groupName);
		}

		bool Group_impl::doesGroupExist(const std::string &groupName) const {
			auto gnames = getGroupNames();
			if (gnames.count(groupName)) return true;
			return false;
		}

		std::set<std::string> Group_impl::getGroupNames() const {
			auto objs = fs::hdf5::getGroupMembersTypes(*(grp.get()));
			std::set<std::string> res;
			for (const auto &o : objs)
			{
				if (o.second == H5G_obj_t::H5G_GROUP) res.insert(o.first);
			}
			return res;
		}

		Group::Group_ptr Group::mount(const std::string &subdirname, 
			gsl::not_null<const Group*> containingParent, 
			gsl::not_null<H5::H5File*> pointsTo)
		{
			/** \note HDF5 C++ bug in constness on 1.8.5, CentOS 6. I cannot directly pass
			* H5P_DEFAULT to the trivial constructor for H5::PropList as an rvalue **/
			H5::PropList pl(H5P_DEFAULT);
			containingParent->getHDF5Group()->mount(subdirname, *(pointsTo.get()), pl);
			return openGroup(subdirname, containingParent);
		}
	}
}
