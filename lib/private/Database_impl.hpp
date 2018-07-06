#pragma once
#include "../icedb/Database.hpp"
//#include "../private/hdf5_supplemental.hpp"
#include <memory>
#include <hdf5.h>

namespace icedb {
	namespace Databases {
		class file_image {
			hid_t propertyList;
			std::vector<char> buffer;
			const std::string filename;
			std::shared_ptr<hid_t> hFile;
		public:
			std::shared_ptr<hid_t> getHFile() const;
			file_image(const std::string &filename,
				size_t desiredSizeInBytes);
			~file_image();
		};

		class Database_impl : public Database {
			friend class Database;
			std::shared_ptr<hid_t> hFile;
			file_image hFileImage; ///< Used if a virtual base is needed (the typical case)
			static std::shared_ptr<hid_t> makeDatabaseFileStandard(const std::string &p);
		public:
			Database_impl();
			Database_impl(size_t virtualMemSizeInBytes);
			virtual ~Database_impl();
			Groups::Group::Group_ptr openGroup(const std::string &path) override;
			Groups::Group::Group_ptr createGroup(const std::string &path) override;
			Groups::Group::Group_ptr createGroupStructure(const std::string &groupName) override;
			Groups::Group::Group_ptr createGroupStructure(const std::vector<std::string> &groupNames) override;
			//ObjectIdPathSet_Type findObjects(ObjectTypes type, const std::string &base, bool recurse = true);
		};
	}

}