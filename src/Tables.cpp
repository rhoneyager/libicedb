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

		/*
		Table::Table_Type Table::readDimensionScale(size_t DimensionNumber) const {
			Expects(valid());
			Expects(DimensionNumber < getNumDimensions());
			auto selfptr = _getTableSelf();

		}
		*/

		//bool Table::hasDimensionScale(size_t DimensionNumber, gsl::not_null<const Table *> scale) const;
		//bool Table::hasDimensionScale(size_t DimensionNumber) const;
		//bool Table::hasDimensionScales() const;

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
			Expects(!hasDimensionScalesAttached());
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
		
	}
}