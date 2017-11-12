#pragma once
#include "Attribute.hpp"
#include <gsl/gsl>
#include <gsl/gsl_assert>
#include <gsl/span>
namespace icedb {
	namespace Groups {
		class Group;
	}
	namespace Tables {
		class CanHaveTables;

		class Table : virtual public Attributes::CanHaveAttributes {
			friend class CanHaveTables;
		protected:
			virtual void _setTableSelf(std::shared_ptr<H5::DataSet> obj) = 0;
			virtual std::shared_ptr<H5::DataSet> _getTableSelf() const = 0;
			bool valid() const;
		public:
			virtual ~Table();
			Table(const std::string &name = "");
			const std::string name; // Constant, and in a base class, so no need to change.
			typedef std::vector<size_t> Dimensions_Type;
			typedef std::unique_ptr<Table> Table_Type;

			/// \todo Need to ensure that tables have dimension scales attached in all cases.
			/// Required for netCDF compatibility.

			size_t getNumDimensions() const;
			Dimensions_Type getDimensions() const;
			void attachDimensionScale(size_t DimensionNumber, gsl::not_null<const Table *> scale); ///< Attach a dimension scale to this table.
			void detachDimensionScale(size_t DimensionNumber, gsl::not_null<const Table *> scale);
			Table_Type readDimensionScale(size_t DimensionNumber) const;
			bool hasDimensionScaleAttached(size_t DimensionNumber) const;
			bool hasDimensionScaleAttached(size_t DimensionNumber, gsl::not_null<const Table *> scale) const;
			bool hasDimensionScalesAttached() const;

			bool isDimensionScale() const;
			void setDimensionScale(const std::string &dimensionScaleName); ///< Make this table a dimension scale with name dimensionScaleName.
			void setDimensionScaleAxisLabel(size_t DimensionNumber, const std::string &label);
			std::string getDimensionScaleAxisLabel(size_t DimensionNumber) const;
			std::string getDimensionScaleName() const;

			template<class Type> bool isTableOfType() const {
				return false;
			}
			std::type_index getTableTypeId() const;

			template <class DataType>
			void readAll(std::vector<DataType> &inData) const {
				assert(isTableOfType<DataType>());
			}
			template <class DataType>
			void writeAll(const std::vector<DataType> &outData) {
				assert(isTableOfType<DataType>());
			}
		};

		class CanHaveTables {
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
			/*
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
			*/
		};
	}
}