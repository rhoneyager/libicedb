#include <array>
#include "../icedb/Table.hpp"
#include "../icedb/Group.hpp"
#include "../private/Table_impl.hpp"
#include "../private/Attribute_impl.hpp"
#include "../private/hdf5_supplemental.hpp"

namespace icedb {
	namespace Tables {
		Table::~Table() {}
		Table::Table(const std::string &name) : name{ name } {}
		// this->_setAttributeParent(_getTableSelf()); // Goes in implementation class.
		bool Table::valid() const { if (_getTableSelf()) return true; return false; }

		size_t Table::getNumDimensions() const {
			Expects(valid());
			auto ptr = _getTableSelf();
			return fs::hdf5::readDatasetNumDimensions(ptr.get());
		}

		Table::Dimensions_Type Table::getDimensions() const {
			Expects(valid());
			auto ptr = _getTableSelf();
			Table::Dimensions_Type dims;
			fs::hdf5::readDatasetDimensions(ptr.get(), dims);
			return dims;
		}

		void Table::attachDimensionScale(size_t DimensionNumber, gsl::not_null<const Table *> scale) {
			Expects(valid());
			Expects(scale->valid());
			Expects(DimensionNumber < getNumDimensions());
			auto selfptr = _getTableSelf();
			auto tblscaleptr = scale->_getTableSelf();
			const herr_t res = H5DSattach_scale(selfptr->getId(), tblscaleptr->getId(), static_cast<unsigned int>(DimensionNumber));
			assert(res == 0);
		}

		void Table::detachDimensionScale(size_t DimensionNumber, gsl::not_null<const Table *> scale) {
			Expects(valid());
			Expects(scale->valid());
			Expects(DimensionNumber < getNumDimensions());
			auto selfptr = _getTableSelf();
			auto tblscaleptr = scale->_getTableSelf();
			const herr_t res = H5DSdetach_scale(selfptr->getId(), tblscaleptr->getId(), static_cast<unsigned int>(DimensionNumber));
			assert(res == 0);
		}

		/*
		bool Table::hasDimensionScaleAttached(size_t DimensionNumber, gsl::not_null<const Table *> scale) const {
			Expects(valid());
			Expects(scale->valid());
			Expects(DimensionNumber < getNumDimensions());
			auto selfptr = _getTableSelf();
			auto tblscaleptr = scale->_getTableSelf();
			const herr_t res = H5DSis_attached(selfptr->getId(), tblscaleptr->getId(), DimensionNumber);
			assert(res >= 0);
			if (res > 0) return true;
			return false;
		}
		*/
		/*
		Table::Table_Type Table::readDimensionScale(size_t DimensionNumber) const {
			Expects(valid());
			Expects(DimensionNumber < getNumDimensions());
			auto selfptr = _getTableSelf();

		}
		*/
		/*
		void Table::readAttachedDimensionScales() const {
			Expects(valid());
			auto selfptr = _getTableSelf();
			size_t numDims = getNumDimensions();
			int *idx = nullptr; // input the index to start iterating, output the next index. Start at NULL.
			struct DimScaleInfo {
				std::string name;
			};
			// Each dimension can have multiple scales attached.
			typedef std::vector<std::vector<DimScaleInfo> > DimScaleInfo_vt;
			DimScaleInfo_vt DimScaleInfo_v;
			auto visitor = [](hid_t dimscaledataset, unsigned int dimnum, hid_t scale, void *visitor_data) ->herr_t {
				DimScaleInfo_vt *vd = static_cast<DimScaleInfo_vt*>(visitor_data);
				
				return 0;
			};
			for (unsigned int dim = 0; dim < static_cast<unsigned int>(numDims); ++dim) {

				herr_t err = H5DSiterate_scales(selfptr->getId(), dim, idx, visitor, &DimScaleInfo_v);
			}
		}

		bool Table::hasDimensionScaleAttached(size_t DimensionNumber) const;

		bool Table::hasAllDimensionScalesAttached() const;
		*/
		bool Table::isDimensionScale() const {
			Expects(valid());
			auto selfptr = _getTableSelf();
			const htri_t res = H5DSis_scale(selfptr->getId());
			assert(res >= 0);
			if (res > 0) return true;
			return false;
		}

		void Table::setDimensionScale(const std::string &dimensionScaleName) {
			Expects(valid());
			Expects(!isDimensionScale());
			auto selfptr = _getTableSelf();
			const htri_t res = H5DSset_scale(selfptr->getId(), dimensionScaleName.c_str());
			assert(res == 0);
		}

		void Table::setDimensionScaleAxisLabel(size_t DimensionNumber, const std::string &label) {
			Expects(valid());
			Expects(DimensionNumber < getNumDimensions());
			auto selfptr = _getTableSelf();
			const htri_t res = H5DSset_label(selfptr->getId(), static_cast<unsigned int>(DimensionNumber), label.c_str());
			assert(res == 0);
		}

		std::string Table::getDimensionScaleAxisLabel(size_t DimensionNumber) const {
			Expects(valid());
			Expects(DimensionNumber < getNumDimensions());
			auto selfptr = _getTableSelf();
			constexpr size_t max_label_size = 1000;
			std::array<char, max_label_size> label;
			label.fill('\0');
			const ssize_t res = H5DSget_label(selfptr->getId(), static_cast<unsigned int>(DimensionNumber), label.data(), max_label_size);
			// res is the size of the label. The HDF5 documentation does not include whether the label is null-terminated,
			// so I am terminating it manually.
			label[max_label_size - 1] = '\0';
			return std::string(label.data());
		}

		std::string Table::getDimensionScaleName() const {
			Expects(valid());
			auto selfptr = _getTableSelf();
			constexpr size_t max_label_size = 1000;
			std::array<char, max_label_size> label;
			label.fill('\0');
			const ssize_t res = H5DSget_scale_name(selfptr->getId(), label.data(), max_label_size);
			// res is the size of the label. The HDF5 documentation does not include whether the label is null-terminated,
			// so I am terminating it manually.
			label[max_label_size - 1] = '\0';
			return std::string(label.data());
		}
		
		std::type_index Table::getTableTypeId() const {
			Expects(valid());
			auto selfptr = _getTableSelf();
			if (fs::hdf5::isType<uint64_t, H5::DataSet>(selfptr.get())) return (typeid(uint64_t));
			if (fs::hdf5::isType<int64_t, H5::DataSet>(selfptr.get())) return (typeid(int64_t));
			if (fs::hdf5::isType<uint32_t, H5::DataSet>(selfptr.get())) return (typeid(uint32_t));
			if (fs::hdf5::isType<int32_t, H5::DataSet>(selfptr.get())) return (typeid(int32_t));
			if (fs::hdf5::isType<uint16_t, H5::DataSet>(selfptr.get())) return (typeid(uint16_t));
			if (fs::hdf5::isType<int16_t, H5::DataSet>(selfptr.get())) return (typeid(int16_t));
			if (fs::hdf5::isType<uint8_t, H5::DataSet>(selfptr.get())) return (typeid(uint8_t));
			if (fs::hdf5::isType<int8_t, H5::DataSet>(selfptr.get())) return (typeid(int8_t));
			if (fs::hdf5::isType<double, H5::DataSet>(selfptr.get())) return (typeid(double));
			if (fs::hdf5::isType<float, H5::DataSet>(selfptr.get())) return (typeid(float));
			if (fs::hdf5::isType<char, H5::DataSet>(selfptr.get())) return (typeid(char));
			if (fs::hdf5::isType<std::string, H5::DataSet>(selfptr.get())) return (typeid(std::string));
			throw;
		}

		template <class DataType, class ObjectType>
		void pullData(
			std::vector<size_t> &dims,
			std::vector<DataType> &tdata,
			gsl::not_null<ObjectType*> obj)
		{
			size_t sz = 1;
			for (const auto &s : dims) sz *= s;
			tdata.resize(sz);
			icedb::fs::hdf5::readDatasetArray<DataType, H5::DataSet>(obj, tdata.data());
		}

		template <class DataType>
		void Table::readAll(
			std::vector<size_t> &dims,
			std::vector<DataType> &data) const
		{
			Expects(valid());
			auto selfptr = _getTableSelf();
			fs::hdf5::readDatasetDimensions(selfptr.get(), dims);
			size_t sz = 1;
			for (const auto &s : dims) sz *= s;
			data.resize(sz);

			pullData<DataType, H5::DataSet>(dims, data, selfptr.get());
		}

		template <class DataType, class ObjectType>
		void pushData(
			const std::vector<size_t> &dims,
			gsl::not_null<ObjectType*> obj,
			const std::vector<DataType> &data)
		{
			size_t numElems = 1;
			for (const auto &s : dims) numElems *= s;
			Expects(numElems > 0);

			icedb::fs::hdf5::writeDatasetArray<DataType, ObjectType>(dims, obj, data.data());
		}

		/*
		template <class DataType>
		void Table::writeAll(
			const std::vector<DataType> &data) const
		{
			Expects(valid());
			auto selfptr = _getTableSelf();
			const auto dimensionality = getDimensions();
			// Need to copy from the variant structure into an array of the exact data type
			pushData<DataType, H5::DataSet>(dimensionality, selfptr.get(), data);
		}
		*/

		template <class DataType>
		void Table::writeAllInner(const gsl::span<const DataType> &outData) const {
			Expects(valid());
			Expects(isTableOfType<DataType>());
			auto selfptr = _getTableSelf();
			const auto dimensionality = getDimensions();
			size_t sz = 1;
			for (const auto &d : dimensionality) sz *= d;
			Expects(outData.size() == sz);
			icedb::fs::hdf5::writeDatasetArray<DataType, H5::DataSet>(dimensionality, selfptr.get(), outData.data());
		}


#define INST_WRITE_TBL_INNER_TYPE(x) \
		template void Table::writeAllInner<x>(const gsl::span<const x> &outData) const;

		INST_ATTR(INST_WRITE_TBL_INNER_TYPE);

#define INST_READ_TBL_TYPE(x) \
		template void Table::readAll<x>( \
		std::vector<size_t> &dims, \
		std::vector<x> &data) const;

		INST_ATTR(INST_READ_TBL_TYPE);

		CanHaveTables::CanHaveTables() {}
		CanHaveTables::~CanHaveTables() {}
		bool CanHaveTables::valid() const { if (_getTablesParent()) return true; return false; }

		/// \note Must ensure NetCDF-4 compatability.
		void CanHaveTables::_createTable(
			const std::string &tableName,
			const std::type_index& type_id,
			const std::vector<size_t> &dims,
			const std::vector<size_t> &chunks)
		{
			Expects(valid());
			std::vector<hsize_t> hdata;
			for (const auto &d : dims) hdata.push_back(static_cast<hsize_t>(d));
			std::vector<hsize_t> cdata;
			for (const auto &c : chunks) cdata.push_back(static_cast<hsize_t>(c));

			//fs::hdf5::addDatasetArray
			/* Turn on creation order tracking. */
			//assert(H5Pset_attr_creation_order(plistid, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED) >= 0);
			std::shared_ptr<H5::DSetCreatPropList> plist = std::make_shared<H5::DSetCreatPropList>();
			int fillvalue = -1;
			plist->setFillValue(H5::PredType::NATIVE_INT, &fillvalue);
			plist->setChunk(static_cast<int>(cdata.size()), cdata.data());
			if (fs::hdf5::useZLIB()) {
				plist->setDeflate(fs::hdf5::useZLIB());
			}
#if ICEDB_H5_CREATEPROPLIST_SETATTRCRTORDER == 1
			plist->setAttrCrtOrder(H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);
#else
			{
				hid_t id = plist->getId();
				herr_t e = H5Pset_attr_creation_order(id, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);
				if (e < 0) throw;
			}
#endif

			if (type_id == typeid(uint64_t))fs::hdf5::createDatasetRaw<uint64_t, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else if (type_id == typeid(int64_t))fs::hdf5::createDatasetRaw<int64_t, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else if (type_id == typeid(uint32_t))fs::hdf5::createDatasetRaw<uint32_t, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else if (type_id == typeid(int32_t))fs::hdf5::createDatasetRaw<int32_t, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else if (type_id == typeid(uint16_t))fs::hdf5::createDatasetRaw<uint16_t, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else if (type_id == typeid(int16_t))fs::hdf5::createDatasetRaw<int16_t, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else if (type_id == typeid(uint8_t))fs::hdf5::createDatasetRaw<uint8_t, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else if (type_id == typeid(int8_t))fs::hdf5::createDatasetRaw<int8_t, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else if (type_id == typeid(float))fs::hdf5::createDatasetRaw<float, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else if (type_id == typeid(double))fs::hdf5::createDatasetRaw<double, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else if (type_id == typeid(char))fs::hdf5::createDatasetRaw<char, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else if (type_id == typeid(std::string))fs::hdf5::createDatasetRaw<std::string, H5::Group>(_getTablesParent().get(), tableName.c_str(), dims, plist);
			else throw(std::invalid_argument("Unhandled data type"));
		}

		std::set<std::string> CanHaveTables::getTableNames() const {
			Expects(valid());
			auto base = _getTablesParent();
			auto objs = fs::hdf5::getGroupMembersTypes(*(base.get()));
			std::set<std::string> res;
			for (const auto &o : objs)
			{
				if (o.second == H5G_obj_t::H5G_DATASET) res.insert(o.first);
			}
			return res;
		}

		bool CanHaveTables::doesTableExist(const std::string &tableName) const {
			const auto tnames = getTableNames();
			if (tnames.count(tableName)) return true;
			return false;
		}

		void CanHaveTables::unlinkTable(const std::string &tableName) {
			Expects(valid());
			auto base = _getTablesParent();
			base->unlink(tableName.c_str());
		}

		Table::Table_Type CanHaveTables::openTable(const std::string &tableName) {
			Expects(valid());
			const auto base = _getTablesParent();
			const auto tbl = std::make_shared<H5::DataSet>(base->openDataSet(tableName));
			Table::Table_Type res = std::make_unique<Table_impl>(tbl,tableName);
			return res;
		}




		void Table_impl::_setTableSelf(std::shared_ptr<H5::DataSet> obj) { this->obj = obj; }
		std::shared_ptr<H5::DataSet> Table_impl::_getTableSelf() const { return obj; }
		Table_impl::~Table_impl() {}
		Table_impl::Table_impl(std::shared_ptr<H5::DataSet> obj, const std::string &name) : Table{ name }, obj(obj) {
			this->_setAttributeParent(obj);
		}





		void CanHaveTables_impl::_setTablesParent(std::shared_ptr<H5::Group> newobj) { this->obj = newobj; }
		std::shared_ptr<H5::Group> CanHaveTables_impl::_getTablesParent() const { return obj; }
		CanHaveTables_impl::~CanHaveTables_impl() {}
		CanHaveTables_impl::CanHaveTables_impl() {}
	}
}