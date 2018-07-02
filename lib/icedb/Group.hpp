#pragma once
#error "Replaced for now"
#include <memory>
#include <set>
#include <gsl/gsl>
#include "Attribute.hpp"
#include "Table.hpp"
#include "util.hpp"
#include <HH/Group.hpp>
namespace icedb {
	/// All stuff related to Groups.
	namespace Groups {
#ifdef _MSC_FULL_VER
#pragma warning( push )
#pragma warning(disable:4250)
#endif

		/// A group is similar to a folder / directory. It can have Attributes and Tables.
		class Group : virtual public Attributes::CanHaveAttributes, virtual public Tables::CanHaveTables
		{
		protected:
			Group();
			Group(const std::string &name);
		public:
			/// The tag used in icedb to identify a group
			static const std::string _icedb_obj_type_identifier;
			typedef std::unique_ptr<Groups::Group> Group_ptr;
			//typedef std::unique_ptr<Groups::Group, mem::icedb_delete<Groups::Group> > Group_ptr;
			typedef std::shared_ptr<H5::Group> Group_HDF_shared_ptr;
			/// The name of the group
			const std::string name;

			virtual ~Group();
			/// \brief Create a group
			/// \param groupName is the new group's name
			/// \throws if read-only
			/// \throws if a group with htis name already exists
			/// \returns a pointer to the new group
			virtual Group_ptr createGroup(const std::string &groupName) = 0;
			/// \brief Opens a group
			/// \param groupName is the name of the group to be opened
			/// \throws if the group does not exist
			/// \returns a pointer to the newly-opened group
			virtual Group_ptr openGroup(const std::string &groupName) const = 0;
			/// \brief Does a group with this name exist?
			/// \returns true if it exists, false otherwise
			/// \param groupName is the name of the group
			/// \throws if groupName is somehow invalid
			virtual bool doesGroupExist(const std::string &groupName) const = 0;
			/// \brief Get the names of all sub-groups within the current group.
			/// \returns a set of group names
			virtual std::set<std::string> getGroupNames() const = 0;
			/// \brief Unlink the specified child group
			/// \param groupName is the name of the group to be unlinked
			/// \throws if groupName is invalid
			/// \throws if a group with name groupName does not exist
			virtual void deleteGroup(const std::string &groupName) = 0;
			/// \brief Get the fundamental HDF5 object that the group is built on.
			virtual Group_HDF_shared_ptr getHDF5Group() const = 0;


#if ICEDB_H5_UNIFIED_GROUP_FILE == 1
			/// \brief Create a group with a specified parent (static function)
			/// \param name is the new group's name
			/// \param is the new group's parent
			/// \throws if parent is invalid
			/// \throws if name is invalid
			/// \throws if parent is read only
			/// \throws if a group with this name already exists
			static Group_ptr createGroup(const std::string &name, gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> parent);
#else
			/// \brief Create a group with a specified parent (static function)
			/// \param name is the new group's name
			/// \param is the new group's parent
			/// \throws if parent is invalid
			/// \throws if name is invalid
			/// \throws if parent is read only
			/// \throws if a group with this name already exists
			static Group_ptr createGroup(const std::string &name, gsl::not_null<H5::Group*> parent);
			/// \brief Create a group with a specified parent (static function)
			/// \param name is the new group's name
			/// \param is the new group's parent
			/// \throws if parent is invalid
			/// \throws if name is invalid
			/// \throws if parent is read only
			/// \throws if a group with this name already exists
			static Group_ptr createGroup(const std::string &name, gsl::not_null<H5::H5File*> parent);
#endif
			/// \brief Create a group with a specified parent (static function)
			/// \param name is the new group's name
			/// \param is the new group's parent
			/// \throws if parent is invalid
			/// \throws if name is invalid
			/// \throws if parent is read only
			/// \throws if a group with this name already exists
			static Group_ptr createGroup(const std::string &name, gsl::not_null<const Group*> parent);

#if ICEDB_H5_UNIFIED_GROUP_FILE == 1
			/// \brief Open a group under the specified parent (static function)
			/// \param name is the group's name
			/// \param is the group's parent
			/// \throws if parent is invalid
			/// \throws if name is invalid
			/// \throws if a group with this name does not exist
			static Group_ptr openGroup(const std::string &name, gsl::not_null<ICEDB_H5_GROUP_OWNER_PTR> parent);
#else
			/// \brief Open a group under the specified parent (static function)
			/// \param name is the group's name
			/// \param is the group's parent
			/// \throws if parent is invalid
			/// \throws if name is invalid
			/// \throws if a group with this name does not exist
			static Group_ptr openGroup(const std::string &name, gsl::not_null<H5::Group*> parent);
			/// \brief Open a group under the specified parent (static function)
			/// \param name is the group's name
			/// \param is the group's parent
			/// \throws if parent is invalid
			/// \throws if name is invalid
			/// \throws if a group with this name does not exist
			static Group_ptr openGroup(const std::string &name, gsl::not_null<H5::H5File*> parent);
#endif
			/// \brief Open a group under the specified parent (static function)
			/// \param name is the group's name
			/// \param is the group's parent
			/// \throws if parent is invalid
			/// \throws if name is invalid
			/// \throws if a group with this name does not exist
			static Group_ptr openGroup(const std::string &name, gsl::not_null<const Group*> parent);
			/// \brief Open an icedb group from an HDF5 group object
			/// \param group is the HDF5 group
			static Group_ptr openGroup(Group_HDF_shared_ptr group);

			/// \brief Mount an HDF5 file as a child 'group'
			/// \param subdirname is the mount point
			/// \param containingParent is the parent location of the mount point
			/// \param pointsTo is the file that gets mounted into containingParent/subdirname
			/// \throws if either containingParent or points to are invalid
			/// \returns A pointer to the mounted location
			static Group_ptr mount(const std::string &subdirname, gsl::not_null<const Group*> containingParent, gsl::not_null<H5::H5File*> pointsTo);
		};

#ifdef _MSC_FULL_VER
#pragma warning( pop )
#endif
	}
}
