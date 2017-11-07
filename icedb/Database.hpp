#pragma once
#include <map>
#include <memory>
#include <string>
#include "gsl/span"
#include "Data_Types.hpp"
#include "fs.hpp"
#include "Group.hpp"

namespace icedb {
	namespace Databases {
		class Database {
			class Database_impl;
			std::shared_ptr<Database_impl> _impl;
			Database();
		public:
			static Database createDatabase(const std::string &location);
			static void indexDatabase(const std::string &location);
			static Database openDatabase(const std::string &location, fs::IOopenFlags flags = fs::IOopenFlags::READ_ONLY);
			//static Database copyDatabase(const Database& sourceDB, const std::string &location);
			//Group openPath(const std::string &location);
			//Group openBasePath();

			// Search functions
			// Get all objects that match a set of object identifiers, under a certain base object.
			typedef std::map<std::string, fs::ObjectTypes> ObjectIdPathSet_Type;
			//ObjectIdPathSet_Type findObjects(ObjectTypes type, const std::string &base, bool recurse = true);
		};
	}
}
