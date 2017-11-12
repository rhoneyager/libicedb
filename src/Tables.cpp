#include <array>
#include "../icedb/Table.hpp"
#include "../icedb/Group.hpp"
#include "../private/Table_impl.hpp"
#include "../private/Attribute_impl.hpp"
#include "../icedb/hdf5_supplemental.hpp"

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
			if (fs::hdf5::isType<double, H5::DataSet>(selfptr.get())) return (typeid(double));
			if (fs::hdf5::isType<float, H5::DataSet>(selfptr.get())) return (typeid(float));
			if (fs::hdf5::isType<char, H5::DataSet>(selfptr.get())) return (typeid(char));
			if (fs::hdf5::isType<std::string, H5::DataSet>(selfptr.get())) return (typeid(std::string));
			throw;
		}

		template <class DataType, class ObjectType>
		void pullData(
			std::vector<size_t> &dims,
			std::vector<Data_Types::All_Variant_type> &data,
			gsl::not_null<ObjectType*> obj)
		{
			std::vector<DataType> tdata;
			size_t sz = 1;
			for (const auto &s : dims) sz *= s;
			tdata.resize(sz);
			icedb::fs::hdf5::readDatasetArray<DataType, H5::DataSet>(obj, tdata.data());

			data.resize(tdata.size());
			std::copy(tdata.cbegin(), tdata.cend(), data.begin());
		}

		void Table::readAll(
			std::vector<size_t> &dims,
			std::vector<Data_Types::All_Variant_type> &data) const
		{
			Expects(valid());
			auto selfptr = _getTableSelf();
			fs::hdf5::readDatasetDimensions(selfptr.get(), dims);
			size_t sz = 1;
			for (const auto &s : dims) sz *= s;
			data.resize(sz);

			if (icedb::fs::hdf5::isType<uint64_t, H5::DataSet>(selfptr.get()))
				pullData<uint64_t, H5::DataSet>(dims, data, selfptr.get());
			else if (icedb::fs::hdf5::isType<int64_t, H5::DataSet>(selfptr.get()))
				pullData<int64_t, H5::DataSet>(dims, data, selfptr.get());
			else if (icedb::fs::hdf5::isType<float, H5::DataSet>(selfptr.get()))
				pullData<float, H5::DataSet>(dims, data, selfptr.get());
			else if (icedb::fs::hdf5::isType<double, H5::DataSet>(selfptr.get()))
				pullData<double, H5::DataSet>(dims, data, selfptr.get());
			else if (icedb::fs::hdf5::isType<char, H5::DataSet>(selfptr.get()))
				pullData<char, H5::DataSet>(dims, data, selfptr.get());
			else if (icedb::fs::hdf5::isType<std::string, H5::DataSet>(selfptr.get()))
				pullData<std::string, H5::DataSet>(dims, data, selfptr.get());
			else throw(std::exception("Unhandled data type"));
		}

		template <class DataType, class ObjectType>
		void pushData(
			const std::vector<size_t> &dims,
			gsl::not_null<ObjectType*> obj,
			const std::vector<Data_Types::All_Variant_type> &indata)
		{
			size_t numElems = 1;
			for (const auto &s : dims) numElems *= s;
			Expects(numElems > 0);
			std::vector<DataType> data(numElems);
			for (size_t i = 0; i < numElems; ++i)
				data[i] = std::get<DataType>(indata[i]);

			icedb::fs::hdf5::addDatasetArray<DataType, ObjectType>(dims, obj, data.data());
			//icedb::fs::hdf5::addAttrArray<DataType, ObjectType>(obj.get(), attributeName.c_str(), dimensionality, data);
		}

		void Table::writeAll(
			const type_info &type_id,
			const std::vector<Data_Types::All_Variant_type> &data) const
		{
			Expects(valid());
			auto selfptr = _getTableSelf();
			const auto dimensionality = getDimensions();
			// Need to copy from the variant structure into an array of the exact data type
			if (type_id == typeid(uint64_t))pushData<uint64_t, H5::DataSet>(dimensionality, selfptr.get(), data);
			else if (type_id == typeid(int64_t))pushData<int64_t, H5::DataSet>(dimensionality, selfptr.get(), data);
			else if (type_id == typeid(float))pushData<float, H5::DataSet>(dimensionality, selfptr.get(), data);
			else if (type_id == typeid(double))pushData<double, H5::DataSet>(dimensionality, selfptr.get(), data);
			else if (type_id == typeid(char))pushData<char, H5::DataSet>(dimensionality, selfptr.get(), data);
			else if (type_id == typeid(std::string))pushData<std::string, H5::DataSet>(dimensionality, selfptr.get(), data);
			else throw(std::exception("Unhandled data type"));
			//if (icedb::Data_Types::Is_Valid_Data_Type(type_id)) throw;
		}

	}
}