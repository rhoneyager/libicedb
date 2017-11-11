#pragma once
#include <memory>
#include <set>
#include "../icedb/Group.hpp"
namespace icedb {
	namespace Groups {
		class Group_impl : public Group
		{
			/// \note This shared pointer has a custom deallocator, to ensure that it is freed in the correct module.
			std::shared_ptr<H5::Group> grp;

			friend class Group;
			Group_impl();
			Group_impl(const std::string &name, std::shared_ptr<H5::Group> parent);
			Group_impl(const std::string &name, gsl::not_null<const Group*> parent);
		public:
			Group_ptr createGroup(const std::string &groupName) override;
			Group_ptr openGroup(const std::string &groupName) const override;
			bool doesGroupExist(const std::string &groupName) const override;
			std::shared_ptr<std::set<std::string> > getGroupNames() const override;
			void deleteGroup(const std::string &groupName) override;
			std::shared_ptr<H5::Group> getHDF5Group() const override;
		};

	}
}