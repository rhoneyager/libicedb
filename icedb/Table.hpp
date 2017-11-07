#pragma once
#include "Attribute.hpp"
namespace icedb {
	namespace Groups {
		class Group;
	}
	namespace Tables {
		template <class DataType> class Table {
			class Table_impl;
			std::shared_ptr<Table_impl> _impl;
		public:
			typedef DataType Type;
			typedef std::vector<size_t> Dimensions_Type;
			const std::string name;
			Dimensions_Type getDimensions() const;
			void setDimensions(const Dimensions_Type& newDimensions);

			void readAll(gsl::span<DataType> &inData) const;
			void writeAll(const gsl::span<DataType> &outData);
			void readAll(std::vector<DataType> &inData) const;
			void writeAll(const std::vector<DataType> &outData);

			//Table(Groups::Group &owner, const std::string &name) : name(name) {
			//	static_assert(icedb::Data_Types::Is_Valid_Data_Type<DataType>() == true,
			//		"Tables must be a valid data type");
			//}

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
	}
}