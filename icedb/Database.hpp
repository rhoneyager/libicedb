#pragma once
#include <map>
#include <memory>
#include <string>
#include <gsl/span>
#include <gsl/pointers>
#include "util.hpp"
#include "Data_Types.hpp"
#include "fs.hpp"
#include "Group.hpp"

namespace icedb {
	namespace Databases {
		class Database {
		protected:
			Database();
		public:
			typedef std::unique_ptr<Database, mem::icedb_delete<Database> > Database_ptr;
			virtual ~Database();
			static Database_ptr createDatabase(const std::string &location);
			static Database_ptr openDatabase(const std::string &location, fs::IOopenFlags flags = fs::IOopenFlags::READ_ONLY);
			static void indexDatabase(const std::string &location);

			//static Database copyDatabase(const Database& sourceDB, const std::string &location);
			virtual Groups::Group::Group_ptr openGroup(const std::string &path) = 0;
			virtual Groups::Group::Group_ptr createGroup(const std::string &path) = 0;
			//ObjectIdPathSet_Type findObjects(ObjectTypes type, const std::string &base, bool recurse = true);
		};
	}

	// Search functions
	// Get all objects that match a set of object identifiers, under a certain base object.
	typedef std::map<std::string, fs::ObjectTypes> ObjectIdPathSet_Type;
}
