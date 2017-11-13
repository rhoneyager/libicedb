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
			//void readAttachedDimensionScales() const;
		protected:
			virtual void _setTableSelf(std::shared_ptr<H5::DataSet> obj) = 0;
			virtual std::shared_ptr<H5::DataSet> _getTableSelf() const = 0;
			bool valid() const;
			Table(const std::string &name = "");
		public:
			virtual ~Table();
			const std::string name; // Constant, and in a base class, so no need to change.
			typedef std::vector<size_t> Dimensions_Type;
			typedef std::unique_ptr<Table> Table_Type;

			/// \todo Need to ensure that tables have dimension scales attached in all cases.
			/// Required for netCDF compatibility.
			//bool hasAllDimensionScalesAttached() const;
			//bool hasDimensionScaleAttached(size_t DimensionNumber) const;
			//bool hasDimensionScaleAttached(size_t DimensionNumber, gsl::not_null<const Table *> scale) const;
			//bool hasDimensionScalesAttached() const;

			size_t getNumDimensions() const;
			Dimensions_Type getDimensions() const;
			void attachDimensionScale(size_t DimensionNumber, gsl::not_null<const Table *> scale); ///< Attach a dimension scale to this table.
			void detachDimensionScale(size_t DimensionNumber, gsl::not_null<const Table *> scale);
			//Table_Type readDimensionScale(size_t DimensionNumber) const;
			
			bool isDimensionScale() const;
			void setDimensionScale(const std::string &dimensionScaleName); ///< Make this table a dimension scale with name dimensionScaleName.
			void setDimensionScaleAxisLabel(size_t DimensionNumber, const std::string &label);
			std::string getDimensionScaleAxisLabel(size_t DimensionNumber) const;
			std::string getDimensionScaleName() const;

			template<class Type> bool isTableOfType() const {
				std::type_index atype = getTableTypeId();
				if (atype == typeid(Type)) return true;
				return false;
			}
			std::type_index getTableTypeId() const;

			void readAll(
				std::vector<size_t> &dimensions,
				std::vector<Data_Types::All_Variant_type> &data) const;
			template <class DataType>
			void readAll(std::vector<size_t> &dims, std::vector<DataType> &data) const {
				assert(isTableOfType<DataType>());
				std::vector<Data_Types::All_Variant_type> vdata;

				readAll(dims, vdata);
				data.resize(vdata.size());
				for (size_t i = 0; i < vdata.size(); ++i)
					res.data[i] = std::get<DataType>(vdata[i]);
				return res;
			}

			void writeAll(
				const type_info &type_id,
				const std::vector<Data_Types::All_Variant_type> &data) const;
			/// Fixed size of data. Cannot resize without re-creating the dataset, for now.
			/// Eventually, will support unlimited dimensions.
			template <class DataType>
			void writeAll(const std::vector<DataType> &outData) const {
				assert(isTableOfType<DataType>());
				size_t sz = 1;
				for (const auto &d : getDimensions()) sz *= d;
				Expects(outData.size() == sz);

				std::vector<Data_Types::All_Variant_type> vdata(outData.size());
				for (size_t i = 0; i < outData.size(); ++i)
					vdata[i] = outData[i];
				// copy_n will not work here...
				//std::copy_n(attribute.data.cbegin(), attribute.data.cend(), vdata.begin());
				writeAll(typeid(DataType), vdata);
			}
			template <class DataType>
			void writeAll(const gsl::span<DataType> &outData) const {
				assert(isTableOfType<DataType>());
				size_t sz = 1;
				for (const auto &d : getDimensions()) sz *= d;
				Expects(outData.size() == sz);

				std::vector<Data_Types::All_Variant_type> vdata(outData.size());
				for (size_t i = 0; i < static_cast<size_t>(outData.size()); ++i)
					vdata[i] = outData[i];
				// copy_n will not work here...
				//std::copy_n(attribute.data.cbegin(), attribute.data.cend(), vdata.begin());
				writeAll(typeid(DataType), vdata);
			}
		};

		class CanHaveTables {
			bool valid() const;
			void _createTable(const std::string &tableName, const std::type_index& type, const std::vector<size_t> &dims);
		protected:
			CanHaveTables();
			virtual void _setTablesParent(std::shared_ptr<H5::Group> obj) = 0;
			virtual std::shared_ptr<H5::Group> _getTablesParent() const = 0;
		public:
			~CanHaveTables();
			std::set<std::string> getTableNames() const;
			bool doesTableExist(const std::string &tableName) const;
			void unlinkTable(const std::string &tableName);
			Table::Table_Type openTable(const std::string &tableName);
			template <class DataType>
			Table::Table_Type createTable(const std::string &tableName, const std::vector<size_t> &dims) {
				Expects(!doesTableExist(tableName));
				_createTable(tableName, Data_Types::getType<DataType>(), dims);
				return openTable(tableName);
			}
			template <class DataType>
			Table::Table_Type createTable(
				const std::string &tableName,
				std::initializer_list<size_t> dims)
			{
				std::vector<size_t> vdims{ dims };
				auto tbl = createTable<typename DataType>(tableName, vdims);
				return tbl;
			}
			template <class DataType>
			Table::Table_Type createTable(
				const std::string &tableName,
				std::initializer_list<size_t> dims,
				std::initializer_list<DataType> data)
			{
				auto tbl = createTable<typename DataType>(tableName, dims);
				std::vector<DataType> vdata{ data };
				tbl->writeAll<typename DataType>(vdata);
				return tbl;
			}
			template <class DataType>
			Table::Table_Type createTable(
				const std::string &tableName,
				std::initializer_list<size_t> dims,
				const std::vector<DataType> &data)
			{
				auto tbl = createTable<typename DataType>(tableName, dims);
				tbl->writeAll<typename DataType>(data);
				return tbl;
			}
			template <class DataType>
			Table::Table_Type createTable(
				const std::string &tableName,
				std::initializer_list<size_t> dims,
				const gsl::span<DataType> &data)
			{
				auto tbl = createTable<typename DataType>(tableName, dims);
				tbl->writeAll<typename DataType>(data);
				return tbl;
			}
		};
	}
}