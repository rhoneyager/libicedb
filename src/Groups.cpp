#include "../icedb/Group.hpp"
#include "../icedb/compat/hdf5_load.h"
#include "../icedb/hdf5_supplemental.hpp"
#include "../icedb/gsl/gsl_assert"

namespace icedb {
	namespace Groups {
		class Group::Group_impl {
			friend class Group;
			std::shared_ptr<H5::Group> grp;
		};

		Group::Group() {}

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

		std::shared_ptr<H5::Group> Group::getHDF5Group() const {
			return _impl->grp;
		}

		Group Group::createGroup(const std::string &groupName) {
			_impl->grp->createGroup(groupName);
			Group res(groupName, *this);
			return std::move(res);
		}

		Group Group::openGroup(const std::string &groupName) const {
			Group res(groupName, _impl->grp);
			return res;
		}

		void Group::deleteGroup(const std::string &groupName) {
			_impl->grp->unlink(groupName);
		}

		bool Group::doesGroupExist(const std::string &groupName) const {
			auto gnames = getGroupNames();
			if (gnames.count(groupName)) return true;
			return false;
		}

		std::set<std::string> Group::getGroupNames() const {
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