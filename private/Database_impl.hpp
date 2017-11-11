#pragma once
#include "../icedb/Database.hpp"
#include "../icedb/hdf5_supplemental.hpp"

namespace icedb {
	namespace Databases {
		class file_image {
			hid_t propertyList;
			std::vector<char> buffer;
			const std::string filename;
			std::shared_ptr<H5::H5File> hFile;
		public:
			std::shared_ptr<H5::H5File> getHFile() const;
			file_image(const std::string &filename,
				size_t desiredSizeInBytes);
			~file_image();
		};

		class Database_impl : public Database {
			friend class Database;
			std::shared_ptr<H5::H5File> hFile;
			file_image hFileImage; ///< Used if a virtual base is needed (the typical case)
			static std::shared_ptr<H5::H5File> makeDatabaseFileStandard(const std::string &p);
		public:
			Database_impl();
			virtual ~Database_impl();
			Groups::Group openGroup(const std::string &path) override;
			//ObjectIdPathSet_Type findObjects(ObjectTypes type, const std::string &base, bool recurse = true);
		};
	}

}
