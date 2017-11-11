#pragma once
#include <map>
#include <memory>
#include <string>
#include <gsl/span>
#include <gsl/pointers>
#include "Data_Types.hpp"
#include "fs.hpp"
#include "Group.hpp"

namespace icedb {
	namespace Databases {
		class Database {
		protected:
			Database();
		public:
			virtual ~Database();
			static std::unique_ptr<Database> createDatabase(const std::string &location);
			static std::unique_ptr<Database> openDatabase(const std::string &location, fs::IOopenFlags flags = fs::IOopenFlags::READ_ONLY);
			static void indexDatabase(const std::string &location);

			//static Database copyDatabase(const Database& sourceDB, const std::string &location);
			virtual Groups::Group openGroup(const std::string &path) = 0;
			//ObjectIdPathSet_Type findObjects(ObjectTypes type, const std::string &base, bool recurse = true);
		};
	}

	// Search functions
	// Get all objects that match a set of object identifiers, under a certain base object.
	typedef std::map<std::string, fs::ObjectTypes> ObjectIdPathSet_Type;
}
