#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#if defined(ICEDB_HAS_GSL)
#include "gsl/span"
#endif

namespace icedb {
	enum class ObjectTypes {
		ANY,
		SHAPE
	};

	namespace Data_Types {
		template <class T> constexpr bool Is_Valid_Data_Type() { return false; }
		template<> constexpr bool Is_Valid_Data_Type<uint64_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<int64_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<float>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<double>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<char>() { return true; }
		inline bool Is_Valid_Data_Type(const type_info &type_id) {
			if (type_id == typeid(uint64_t)) return true;
			else if (type_id == typeid(int64_t)) return true;
			else if (type_id == typeid(float)) return true;
			else if (type_id == typeid(double)) return true;
			else if (type_id == typeid(char)) return true;
			return false;
		}
	}
	namespace fs {
		namespace impl {
			class Attribute_impl;
			class Table_impl;
			class Group_impl;
			class File_impl;
		}

		class Group;

		template <class DataType> class Attribute {
			std::unique_ptr<impl::Attribute_impl> _impl;
		public:
			typedef DataType type;
			std::vector<DataType> data;
			std::string name;

			Attribute() {
				static_assert(icedb::Data_Types::Is_Valid_Data_Type<DataType>() == true,
					"Attributes must be a valid data type");
			}
		};

		class CanHaveAttributes {
		public:
			std::vector<std::string> getAttributeNames() const;
			bool doesAttributeExist(const std::string &attributeName) const;
			template<class Type> bool isAttributeOfType(const std::string &attributeName) const {
				return false;
			}
			type_info getAttributeTypeId(const std::string &attributeName) const {}
			template<class DataType> Attribute<DataType> openAttribute(const std::string &attributeName) {}
			template<class DataType> void writeAttribute(const Attribute<DataType> &attribute) {}
			void deleteAttribute(const std::string &attributeName);
		};

		template <class DataType> class Table {
			std::unique_ptr<impl::Table_impl> _impl;
		public:
			typedef DataType Type;
			typedef std::vector<size_t> Dimensions_Type;
			const std::string name;
			Dimensions_Type getDimensions() const;
			void setDimensions(const Dimensions_Type& newDimensions);

#if defined(ICEDB_HAS_GSL)
			void readAll(gsl::span<DataType> &inData) const;
			void writeAll(const gsl::span<DataType> &outData);
#endif
			void readAll(std::vector<DataType> &inData) const;
			void writeAll(const std::vector<DataType> &outData);

			Table(Group &owner, const std::string &name) : name(name) {
				static_assert(icedb::Data_Types::Is_Valid_Data_Type<DataType>() == true,
					"Tables must be a valid data type");
			}

			/// \todo Need to ensure that tables have dimension scales attached in all cases.
			/// Required for netCDF compatibility.
		};

		class CanHaveTables {
		public:
			std::vector<std::string> getTableNames() const;
			bool doesTableExist(const std::string &tableName) const;
			void deleteTable(const std::string &tableName);
			template<class Type> bool isTableOfType(const std::string &tableName) const {
				return false;
			}
			type_info getTableTypeId(const std::string &tableName) const;
			template<class DataType> Table<DataType> openTable(const std::string &tableName) {

			}
			template<class DataType> Table<DataType> createTable(const std::string &tableName) {

			}
		};

		class Group : public CanHaveAttributes, CanHaveTables {
			std::unique_ptr<impl::Group_impl> _impl;
		public:
			const std::string name;
			Group createGroup(const std::string &groupName);
			Group openGroup(const std::string &groupName) const;
			bool doesGroupExist(const std::string &groupName) const;
			std::vector<std::string> getGroupNames() const;
			void deleteGroup(const std::string &groupName);
			bool isGroupEmpty() const;

			//void mountFile(File subfile);
			bool isMountedFile() const;
			void unmountFile();
		};

		enum class IOopenFlags {
			READ_ONLY,
			READ_WRITE,
			TRUNCATE,
			CREATE
		};



		class Database {
			class Database_impl;
			std::shared_ptr<Database_impl> _impl;
			Database();
		public:
			static Database createDatabase(const std::string &location);
			static void indexDatabase(const std::string &location);
			static Database openDatabase(const std::string &location, IOopenFlags flags = IOopenFlags::READ_ONLY);
			//static Database copyDatabase(const Database& sourceDB, const std::string &location);
			//Group openPath(const std::string &location);
			//Group openBasePath();

			// Search functions
			// Get all objects that match a set of object identifiers, under a certain base object.
			typedef std::map<std::string, ObjectTypes> ObjectIdPathSet_Type;
			//ObjectIdPathSet_Type findObjects(ObjectTypes type, const std::string &base, bool recurse = true);
		};
	}
}