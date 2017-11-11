#pragma once
#include "Attribute.hpp"
#include <gsl/gsl_assert>
#include <gsl/span>
namespace icedb {
	namespace Groups {
		class Group;
	}
	namespace Tables {
		class Table {
			class Table_impl;
			std::shared_ptr<Table_impl> _impl;
		public:
			typedef std::vector<size_t> Dimensions_Type;
			const std::string name;
			Dimensions_Type getDimensions() const;
			void attachDimensions(const Dimensions_Type& newDimensions);
			void attachDimensionScale(size_t DimensionNumber, const Table &scale);
			Table readDimensionScale(size_t DimensionNumber) const;
			bool hasDimensionScale(size_t DimensionNumber) const;

			bool isDimensionScale() const;
			bool setDimensionScale(const std::string &dimensionName);
			void setDimensionScaleAxisLabel(size_t DimensionNumber, const std::string &label);
			std::string getDimensionScaleAxisLabel(size_t DimensionNumber) const;
			std::string getDimensionScaleLabel() const;

			template <class DataType>
			void readAll(gsl::span<DataType> &inData) const;
			template <class DataType>
			void writeAll(const gsl::span<DataType> &outData);
			template <class DataType>
			void readAll(std::vector<DataType> &inData) const;
			template <class DataType>
			void writeAll(const std::vector<DataType> &outData);

			Table(Groups::Group &owner, const std::string &name);
			Table();

			//Table(Groups::Group &owner, const std::string &name) : name(name) {
			//	static_assert(icedb::Data_Types::Is_Valid_Data_Type<DataType>() == true,
			//		"Tables must be a valid data type");
			//}

			/// \todo Need to ensure that tables have dimension scales attached in all cases.
			/// Required for netCDF compatibility.
		};

		class CanHaveTables {
			class CanHaveTables_impl;
			std::shared_ptr<CanHaveTables_impl> _impl;
			bool valid() const;
			/// \todo Ensure netCDF4 compatability.
			void _createTable(const std::string &tableName, const type_info& type);
			// Open table
			// Check table type
		protected:
			CanHaveTables(std::shared_ptr<H5::Group>);
			CanHaveTables();
			void _setTablesParent(std::shared_ptr<H5::Group> obj);
		public:
			std::set<std::string> getTableNames() const;
			bool doesTableExist(const std::string &tableName) const;
			void deleteTable(const std::string &tableName);
			template<class Type> bool isTableOfType(const std::string &tableName) const {
				return false;
			}
			std::type_index getTableTypeId(const std::string &tableName) const;
			Table openTable(const std::string &tableName) {

			}
			template <class DataType>
			Table createTable(const std::string &tableName) {
				Expects(!doesTableExist(tableName));
				_createTable(tableName, Data_Types::getType<DataType>());
				return std::move(openTable(tableName));
			}
		};
	}
}