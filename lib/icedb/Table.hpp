#pragma once
#include "Attribute.hpp"
#include "compat/gsl/gsl"
#include "compat/gsl/gsl_assert"
#include "compat/gsl/span"
namespace icedb {
	namespace Groups {
		class Group;
	}
	/// Everything to do with Tables is located in this namespace
	namespace Tables {
		class CanHaveTables;

		/// This defines a table.
		/// \see CanHaveTables for the functions to create, access and remove tables.
		class Table : virtual public Attributes::CanHaveAttributes {
			friend class CanHaveTables;
			//void readAttachedDimensionScales() const;
		protected:
			/// Associates the icedb table with the fundamental HDF5 DataSet. Only used internally.
			virtual void _setTableSelf(std::shared_ptr<H5::DataSet> obj) = 0;
			/// Gets the fundamental HDF5 DataSet associated with the table. Only used internally.
			virtual std::shared_ptr<H5::DataSet> _getTableSelf() const = 0;
			/// Checks that Table is well-formed during runtime. Only used internally.
			bool valid() const;
			/// Default constructor, used by CanHaveTables
			Table(const std::string &name = "");
		public:
			virtual ~Table();
			/// The name of the Table
			const std::string name; // Constant, and in a base class, so no need to change.
			/// The dimensions of a Table are expressed as a vector of unsigned integers.
			typedef std::vector<size_t> Dimensions_Type;
			/// The preferred method of access of a Table is through std::unique_ptr
			typedef std::unique_ptr<Table> Table_Type;

			/// \todo Need to ensure that tables have dimension scales attached in all cases.
			/// Required for netCDF compatibility.
			//bool hasAllDimensionScalesAttached() const;
			//bool hasDimensionScaleAttached(size_t DimensionNumber) const;
			//bool hasDimensionScaleAttached(size_t DimensionNumber, gsl::not_null<const Table *> scale) const;
			//bool hasDimensionScalesAttached() const;

			/// Returns the number of dimensions of a table.
			size_t getNumDimensions() const;
			/// Returns a vector containing the dimensions of the table.
			Dimensions_Type getDimensions() const;
			/// Attaches a dimension scale to a table.
			/// \see the HDF5 HL dimension scale API docs, at https://support.hdfgroup.org/HDF5/doc/HL/RM_H5DS.html
			void attachDimensionScale(size_t DimensionNumber, gsl::not_null<const Table *> scale); ///< Attach a dimension scale to this table.
			/// Detaches a dimension scale from this table.
			void detachDimensionScale(size_t DimensionNumber, gsl::not_null<const Table *> scale);
			//Table_Type readDimensionScale(size_t DimensionNumber) const;
			
			/// Is this Table used as a dimension scale?
			bool isDimensionScale() const;

			/// Designate this table as a dimension scale
			void setDimensionScale(const std::string &dimensionScaleName);
			/// Set the axis label for the dimension designated by DimensionNumber
			void setDimensionScaleAxisLabel(size_t DimensionNumber, const std::string &label);
			/// Get the axis label for the dimension designated by DimensionNumber
			std::string getDimensionScaleAxisLabel(size_t DimensionNumber) const;
			/// Get the name of this table's defined dimension scale
			std::string getDimensionScaleName() const;

			/// Check if this table has the matching Type
			template<class Type> bool isTableOfType() const {
				std::type_index atype = getTableTypeId();
				if (atype == typeid(Type)) return true;
				return false;
			}

			/// Get the type of the table (i.e. int64_t, float, etc.)
			std::type_index getTableTypeId() const;

			/// \brief Read all of the data in the associated table
			/// \param DataType is the type of data.
			/// \throws if the DataType is mismatched (see isTableOfType and getTableTypeId).
			/// \param dims is an output parameter representing the size of each of the table's dimensions
			/// \param data is an output parameter that stores the table's data.
			/// \note This convenience function uses a std::vector to store the output. This uses dynamic memory.
			template <class DataType>
			void readAll(std::vector<size_t> &dims, std::vector<DataType> &data) const;

			// Fixed size of data. Cannot resize without re-creating the dataset, for now.
			// Eventually, will support unlimited dimensions.
			// These all require that the data being written has the correct type for the table.
		private:
			template <class DataType>
			void writeAllInner(const gsl::span<const DataType> &outData) const;
		public:
			
			/// \brief Write the passed data to the table. Writes whole table.
			/// \param DataType is the type of the data.
			/// \throws if the DataType does not match the fundamental type of the table.
			/// \param outData is the data to be written to the table.
			/// \throws if outData's size does not match the table's size.
			template <class DataType>
			void writeAll(const gsl::span<const DataType> &outData) const {
				static_assert(icedb::Data_Types::Is_Valid_Data_Type<DataType>() == true,
					"Data to be written must be a valid data type");
				this->template writeAllInner<DataType>(outData);
			}
			/// \brief Write the passed data to the table. Writes whole table.
			/// \param DataType is the type of the data.
			/// \throws if the DataType does not match the fundamental type of the table.
			/// \param outData is the data to be written to the table.
			/// \throws if outData's size does not match the table's size.
			template <class DataType>
			void writeAll(const gsl::span<DataType> &outData) const {
				this->template writeAll<DataType>(gsl::span<const DataType>(outData.cbegin(), outData.cend()));
			}
			/// \brief Write the passed data to the table. Writes whole table. Convenience converter function.
			/// \param DataType is the type of the data.
			/// \throws if the DataType does not match the fundamental type of the table.
			/// \param outData is the data to be written to the table.
			/// \throws if outData's size does not match the table's size.
			template <class DataType>
			void writeAll(const std::vector<DataType> &outData) const {
				this->template writeAll<DataType>(gsl::span<const DataType>(outData.data(), outData.size()));
			}
		};

		/// \brief The virtual base class used in all objects that can contain tables (groups and datasets).
		class CanHaveTables {
			/// Checks that the class is well-formed, constructed and activated. CanHaveTables _needs_ post-constructor setup.
			bool valid() const;
			/// \brief Internal function to create a table. Does not write data.
			/// \param tableName is the name of the table.
			/// \param type is the type of the table.
			/// \param dims are the dimensions of the table.
			/// \param chunks are the HDF5 chunking parameters, used when writing a table.
			void _createTable(const std::string &tableName, const std::type_index& type, const std::vector<size_t> &dims, const std::vector<size_t> &chunks);
		protected:
			/// Trivial constructor used when CanHaveTables is a virtual base class.
			CanHaveTables();
			/// CanHaveTables needs post-constructor setup. This sets the base HDF5 object that gets manipulated.
			virtual void _setTablesParent(std::shared_ptr<H5::Group> obj) = 0;
			/// Gets the base HDF5 object that is manipulated.
			virtual std::shared_ptr<H5::Group> _getTablesParent() const = 0;
		public:
			~CanHaveTables();
			/// \brief Lists all table names that are children of this object
			std::set<std::string> getTableNames() const;
			/// \brief Does a table exist with the given name
			bool doesTableExist(const std::string &tableName) const;
			/// \brief Unlink a table. In HDF5, this is not the same as erasing a table, which never actually happens.
			/// \throws if a table with name tableName does not exist.
			/// \throws if the open file is read only
			void unlinkTable(const std::string &tableName);
			/// Open the table with the matching name.
			/// \throws if a table with this name does not exist.
			Table::Table_Type openTable(const std::string &tableName);
			/// \brief The default chunking strategy for this table. Used for storage i/o speed, and for compression.
			/// \todo Use a more intelligent strategy
			/// \returns the ideal size of each chunk of the dataset.
			inline std::vector<size_t> getChunkStrategy(const std::vector<size_t> &dims) {
				return dims;
			}
			/// \brief Create a table
			/// \param DataType is the type of the data
			/// \param tableName is the name of the table
			/// \throws if a table with this name already exists
			/// \throws if the base object is read only
			/// \param dims are the dimensions of the table
			/// \param chunks is the size of each N-dimensional chunk, used for storage and compression. HDF5
			/// writes each table as a group of chunked objects. When reading or writing, entire chunks are always
			/// read or written.
			/// \returns The new table.
			template <class DataType>
			Table::Table_Type createTable(const std::string &tableName, const std::vector<size_t> &dims, const std::vector<size_t> *chunks = nullptr) {
				Expects(!doesTableExist(tableName));
				std::vector<size_t> chunksGuess;
				if (!chunks) {
					chunksGuess = getChunkStrategy(dims);
					chunks = &chunksGuess;
				}
				_createTable(tableName, Data_Types::getType<DataType>(), dims, *chunks);
				return openTable(tableName);
			}
			/// \brief Create a table
			/// \param DataType is the type of the data
			/// \param tableName is the name of the table
			/// \throws if a table with this name already exists
			/// \throws if the base object is read only
			/// \param dims are the dimensions of the table
			/// \param chunks is the size of each N-dimensional chunk, used for storage and compression. HDF5
			/// writes each table as a group of chunked objects. When reading or writing, entire chunks are always
			/// read or written.
			/// \returns The new table.
			template <class DataType>
			Table::Table_Type createTable(
				const std::string &tableName,
				std::initializer_list<size_t> dims,
				const std::vector<size_t> *chunks = nullptr)
			{
				std::vector<size_t> vdims{ dims };
				auto tbl = createTable<DataType>(tableName, vdims, chunks);
				return tbl;
			}
			/// \brief Create a table and writes initial data. Used with small tables.
			/// \param DataType is the type of the data
			/// \param tableName is the name of the table
			/// \throws if a table with this name already exists
			/// \throws if the base object is read only
			/// \param dims are the dimensions of the table
			/// \param data are the initial data of the table
			/// \param chunks is the size of each N-dimensional chunk, used for storage and compression. HDF5
			/// writes each table as a group of chunked objects. When reading or writing, entire chunks are always
			/// read or written.
			/// \returns The new table.
			template <class DataType>
			Table::Table_Type createTable(
				const std::string &tableName,
				std::initializer_list<size_t> dims,
				std::initializer_list<DataType> data,
				const std::vector<size_t> *chunks = nullptr)
			{
				auto tbl = createTable<DataType>(tableName, dims, chunks);
				//std::vector<DataType> vdata{ data };
				tbl->template writeAll<DataType>(gsl::span<const DataType>(data.begin(), data.end()));
				return tbl;
			}
			/// \brief Create a table and sets the table's initial data
			/// \param DataType is the type of the data
			/// \param tableName is the name of the table
			/// \throws if a table with this name already exists
			/// \throws if the base object is read only
			/// \param dims are the dimensions of the table
			/// \param data are the data of the table
			/// \param chunks is the size of each N-dimensional chunk, used for storage and compression. HDF5
			/// writes each table as a group of chunked objects. When reading or writing, entire chunks are always
			/// read or written.
			/// \returns The new table.
			template <class DataType>
			Table::Table_Type createTable(
				const std::string &tableName,
				std::initializer_list<size_t> dims,
				const std::vector<DataType> &data,
				const std::vector<size_t> *chunks = nullptr)
			{
				auto tbl = createTable<DataType>(tableName, dims, chunks);
				tbl->template writeAll<DataType>(data);
				return tbl;
			}
			/// \brief Create a table and sets the table's initial data
			/// \param DataType is the type of the data
			/// \param tableName is the name of the table
			/// \throws if a table with this name already exists
			/// \throws if the base object is read only
			/// \param dims are the dimensions of the table
			/// \param data are the data of the table
			/// \param chunks is the size of each N-dimensional chunk, used for storage and compression. HDF5
			/// writes each table as a group of chunked objects. When reading or writing, entire chunks are always
			/// read or written.
			/// \returns The new table.
			template <class DataType>
			Table::Table_Type createTable(
				const std::string &tableName,
				std::initializer_list<size_t> dims,
				const gsl::span<DataType> &data,
				const std::vector<size_t> *chunks = nullptr)
			{
				auto tbl = createTable<DataType>(tableName, dims, chunks);
				tbl->template writeAll<DataType>(data);
				return tbl;
			}
			/// \brief Create a table and sets the table's initial data
			/// \param DataType is the type of the data
			/// \param tableName is the name of the table
			/// \throws if a table with this name already exists
			/// \throws if the base object is read only
			/// \param dims are the dimensions of the table
			/// \param data are the data of the table
			/// \param chunks is the size of each N-dimensional chunk, used for storage and compression. HDF5
			/// writes each table as a group of chunked objects. When reading or writing, entire chunks are always
			/// read or written.
			/// \returns The new table.
			template <class DataType>
			Table::Table_Type createTable(
				const std::string &tableName,
				std::initializer_list<size_t> dims,
				const gsl::span<const DataType> &data,
				const std::vector<size_t> *chunks = nullptr)
			{
				auto tbl = createTable<DataType>(tableName, dims, chunks);
				tbl->template writeAll<DataType>(data);
				return tbl;
			}
		};
	}
}
