#pragma once
#include <memory>
#include <set>
#include "../icedb/Group.hpp"
#include "Attribute_impl.hpp"
#include "Table_impl.hpp"
namespace icedb {
	namespace Groups {

#ifdef _MSC_FULL_VER
#pragma warning( push )
#pragma warning(disable:4250)
#endif
		class Group_impl : virtual public Group, virtual public Attributes::CanHaveAttributes_impl, virtual public Tables::CanHaveTables_impl
		{
			/// \note This shared pointer has a custom deallocator, to ensure that it is freed in the correct module.
			Group_HDF_shared_ptr grp;

			friend class Group;
		public:
			Group_impl();
			Group_impl(const std::string &name, gsl::not_null<H5::Group*> parent);
			Group_impl(const std::string &name, gsl::not_null<const Group*> parent);
			Group_impl(Group_HDF_shared_ptr grp);
			virtual ~Group_impl();
			Group_ptr createGroup(const std::string &groupName) override;
			Group_ptr openGroup(const std::string &groupName) const override;
			bool doesGroupExist(const std::string &groupName) const override;
			/// \note This shared pointer gets a custom deallocator. TODO: Add deallocator for strings.
			std::set<std::string> getGroupNames() const override;
			void deleteGroup(const std::string &groupName) override;
			Group_HDF_shared_ptr getHDF5Group() const override;
		};
#ifdef _MSC_FULL_VER
#pragma warning( pop )
#endif

	}
}