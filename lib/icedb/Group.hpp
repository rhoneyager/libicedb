#pragma once
#include <memory>
#include <set>
#include "compat/gsl/gsl"
#include "Attribute.hpp"
#include "Table.hpp"
#include "util.hpp"
#include "icedb_h5.h"
namespace H5 {
	class Group;
	class H5File;
}
namespace icedb {
	/// All stuff related to Groups.
	namespace Groups {
#ifdef _MSC_FULL_VER
#pragma warning( push )
#pragma warning(disable:4250)
#endif

		class Group : virtual public Attributes::CanHaveAttributes, virtual public Tables::CanHaveTables
		{
		protected:
			Group();
			Group(const std::string &name);
		public:
			static std::string _icedb_obj_type_identifier;
			typedef std::unique_ptr<Groups::Group> Group_ptr;
			//typedef std::unique_ptr<Groups::Group, mem::icedb_delete<Groups::Group> > Group_ptr;
			typedef std::shared_ptr<H5::Group> Group_HDF_shared_ptr;
			const std::string name;

			virtual ~Group();
			virtual Group_ptr createGroup(const std::string &groupName) = 0;
			virtual Group_ptr openGroup(const std::string &groupName) const = 0;
			virtual bool doesGroupExist(const std::string &groupName) const = 0;
			virtual std::set<std::string> getGroupNames() const = 0;
			virtual void deleteGroup(const std::string &groupName) = 0;
			virtual Group_HDF_shared_ptr getHDF5Group() const = 0;


#if ICEDB_H5_UNIFIED_GROUP_FILE == 1
			static Group_ptr createGroup(const std::string &name, gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> parent);
#else
			static Group_ptr createGroup(const std::string &name, gsl::not_null<H5::Group*> parent);
			static Group_ptr createGroup(const std::string &name, gsl::not_null<H5::H5File*> parent);
#endif
			static Group_ptr createGroup(const std::string &name, gsl::not_null<const Group*> parent);

#if ICEDB_H5_UNIFIED_GROUP_FILE == 1
			static Group_ptr openGroup(const std::string &name, gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> parent);
#else
			static Group_ptr openGroup(const std::string &name, gsl::not_null<H5::Group*> parent);
			static Group_ptr openGroup(const std::string &name, gsl::not_null<H5::H5File*> parent);
#endif
			static Group_ptr openGroup(const std::string &name, gsl::not_null<const Group*> parent);
			static Group_ptr openGroup(Group_HDF_shared_ptr group);

			static Group_ptr mount(const std::string &subdirname, gsl::not_null<const Group*> containingParent, gsl::not_null<H5::H5File*> pointsTo);
		};

#ifdef _MSC_FULL_VER
#pragma warning( pop )
#endif
	}
}
