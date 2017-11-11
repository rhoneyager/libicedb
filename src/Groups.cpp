#include "../icedb/Group.hpp"
#include "../icedb/compat/hdf5_load.h"
#include "../icedb/hdf5_supplemental.hpp"
#include "../private/Group_impl.hpp"
#include <gsl/gsl_assert>

namespace icedb {
	namespace Groups {
		Group::Group() : name{} {}
		Group::Group(const std::string &name) : name{ name } {}
		Group_impl::Group_impl() : Group() {}

		Group_impl::Group_impl(const std::string &name, std::shared_ptr<H5::Group> parent)
			: Group{ name }
		{
			auto oldg = fs::hdf5::openGroup(parent, name.c_str());
			grp.reset()
		}
		Group_impl::Group_impl(const std::string &name, gsl::not_null<const Group*> parent)
			: Group{ name }
		{

		}

		Group::Group(const std::string &name, std::shared_ptr<H5::Group> parent) : name(name)
		{
			_impl = std::make_shared<Group_impl>();
			_impl->grp = fs::hdf5::openGroup(parent, name.c_str());
			this->_setAttributeParent(_impl->grp);
		}

		Group::Group(const std::string &name, const Group& pgrp) : name(name)
		{
			_impl = std::make_shared<Group_impl>();
			_impl->grp = fs::hdf5::openGroup(pgrp.getHDF5Group(), name.c_str());
			this->_setAttributeParent(_impl->grp);
		}

		std::shared_ptr<H5::Group> Group_impl::getHDF5Group() const {
			return grp;
		}

		Group::Group_ptr Group_impl::createGroup(const std::string &groupName) {
			//_impl->grp->createGroup(groupName); // Bad for NetCDF. See http://www.unidata.ucar.edu/software/netcdf/docs/file_format_specifications.html#creation_order
			hid_t baseGrpID = _impl->grp->getId();
			/* Create group, with link_creation_order set in the group
			* creation property list. */
			// No suitable C++ methods found.
			//H5::ObjCreatPropList gprops;
			//gprops.setAttrCrtOrder(H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);

			hid_t gcpl_id = H5Pcreate(H5P_GROUP_CREATE);
			if (gcpl_id < 0) throw;
			if (H5Pset_link_creation_order(gcpl_id, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED) < 0) throw;
			if (H5Pset_attr_creation_order(gcpl_id, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED) < 0) throw;
			hid_t newGrp_id = H5Gcreate2(baseGrpID, groupName.c_str(), H5P_DEFAULT, gcpl_id, H5P_DEFAULT);
			if (newGrp_id < 0) throw;
			if (H5Gclose(newGrp_id) < 0) throw;
			if (H5Pclose(gcpl_id) < 0) throw;

			Group res(groupName, *this);
			return std::move(res);
		}

		Group::Group_ptr Group_impl::openGroup(const std::string &groupName) const {
			Group res(groupName, _impl->grp);
			return res;
		}

		void Group_impl::deleteGroup(const std::string &groupName) {
			_impl->grp->unlink(groupName);
		}

		bool Group_impl::doesGroupExist(const std::string &groupName) const {
			auto gnames = getGroupNames();
			if (gnames.count(groupName)) return true;
			return false;
		}

		std::set<std::string> Group_impl::getGroupNames() const {
			auto objs = fs::hdf5::getGroupMembersTypes(*(_impl->grp.get()));
			std::set<std::string> res;
			for (const auto &o : objs)
			{
				if (o.second == H5G_obj_t::H5G_GROUP) res.insert(o.first);
			}
			return res;
		}

		/*
		std::set<std::string> Group::getTableNames() const {
			auto objs = fs::hdf5::getGroupMembersTypes(*(_impl->grp.get()));
			std::set<std::string> res;
			for (const auto &o : objs)
			{
				if (o.second == H5G_obj_t::H5G_DATASET) res.insert(o.first);
			}
			return res;
		}
		*/
	}
}