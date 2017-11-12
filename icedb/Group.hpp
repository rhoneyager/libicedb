#pragma once
#include <memory>
#include <set>
#include <gsl/gsl>
#include "Attribute.hpp"
#include "Table.hpp"
#include "util.hpp"
namespace H5 {
	class Group;
}
namespace icedb {
	namespace Groups {
		class Group : virtual public Attributes::CanHaveAttributes //, Tables::CanHaveTables
		{
		protected:
			Group();
			Group(const std::string &name);
		public:
			typedef std::unique_ptr<Groups::Group, mem::icedb_delete<Groups::Group> > Group_ptr;
			typedef std::shared_ptr<H5::Group> Group_HDF_shared_ptr;
			const std::string name;

			virtual Group_ptr createGroup(const std::string &groupName) = 0;
			virtual Group_ptr openGroup(const std::string &groupName) const = 0;
			virtual bool doesGroupExist(const std::string &groupName) const = 0;
			virtual std::set<std::string> getGroupNames() const = 0;
			virtual void deleteGroup(const std::string &groupName) = 0;
			virtual Group_HDF_shared_ptr getHDF5Group() const = 0;

			static Group_ptr createGroup(const std::string &name, gsl::not_null<H5::Group*> parent);
			static Group_ptr createGroup(const std::string &name, gsl::not_null<const Group*> parent);
		};
	}
}