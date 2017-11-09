#pragma once
#include <memory>
#include <set>
#include "Attribute.hpp"
#include "Table.hpp"
namespace H5 {
	class Group;
}
namespace icedb {
	namespace Groups {
		class Group : public Attributes::CanHaveAttributes, Tables::CanHaveTables {
			class Group_impl;
			std::shared_ptr<Group_impl> _impl;
			Group();
		public:
			Group(const std::string &name, std::shared_ptr<H5::Group> parent);
			Group(const std::string &name, const Group& parent);
			const std::string name;
			Group createGroup(const std::string &groupName);
			Group openGroup(const std::string &groupName) const;
			bool doesGroupExist(const std::string &groupName) const;
			std::set<std::string> getGroupNames() const;
			//std::set<std::string> getTableNames() const; // Moved to CanHaveTables
			void deleteGroup(const std::string &groupName);
			//bool isGroupEmpty() const;
			std::shared_ptr<H5::Group> getHDF5Group() const;

			//void mountFile(File subfile);
			//bool isMountedFile() const;
			//void unmountFile();
		};

	}
}