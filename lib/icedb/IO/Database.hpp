#pragma once
#error "Replaced for now"
#include <map>
#include <memory>
#include <string>
#include <gsl/span>
#include <gsl/pointers>
#include "../Utils/util.hpp"
#include "../IO/fs.hpp"
#include "xxxxxGroup.hpp"

namespace icedb {
	/// Everything related to opening and manipulating icedb databases
	namespace Databases {

		class Database {
		protected:
			Database();
		public:
			typedef std::unique_ptr<Database> Database_ptr;
			//typedef std::unique_ptr<Database, mem::icedb_delete<Database> > Database_ptr;
			virtual ~Database();
			static Database_ptr createSampleDatabase(const std::string &location);
			static Database_ptr createSingleFileDatabase(const std::string &location);
			static Database_ptr openDatabase(const std::string &location, fs::IOopenFlags flags = fs::IOopenFlags::READ_ONLY);
			static Database_ptr openVirtualDatabase(size_t memSizeInBytes);
			static void indexDatabase(const std::string &location);

			//static Database copyDatabase(const Database& sourceDB, const std::string &location);
			virtual Groups::Group::Group_ptr openGroup(const std::string &path) = 0;
			virtual Groups::Group::Group_ptr createGroup(const std::string &path) = 0;
			virtual Groups::Group::Group_ptr createGroupStructure(const std::string &groupName) = 0;
			virtual Groups::Group::Group_ptr createGroupStructure(const std::vector<std::string> &groupNames) = 0;

			//ObjectIdPathSet_Type findObjects(ObjectTypes type, const std::string &base, bool recurse = true);
		};
	}

	// Search functions
	// Get all objects that match a set of object identifiers, under a certain base object.
	typedef std::map<std::string, fs::ObjectTypes> ObjectIdPathSet_Type;
}
