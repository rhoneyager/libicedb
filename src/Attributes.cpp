#include "../icedb/Attribute.hpp"
#include "../icedb/compat/hdf5_load.h"
#include "../icedb/hdf5_supplemental.hpp"
#include "../icedb/gsl/gsl_assert"

namespace icedb {
	namespace Attributes {

		class CanHaveAttributes::CanHaveAttributes_impl{
		public:
			std::shared_ptr<H5::H5Object> parent;
			CanHaveAttributes_impl() {}
			CanHaveAttributes_impl(std::shared_ptr<H5::H5Object> parent) : parent(parent) {}
		};

		CanHaveAttributes::CanHaveAttributes(std::shared_ptr<H5::H5Object> obj) {
			_impl = std::make_shared<CanHaveAttributes_impl>();
		}

		std::vector<size_t> CanHaveAttributes::getAttributeDimensionality(const std::string &attributeName) const
		{
			Expects(doesAttributeExist(attributeName));
			return icedb::fs::hdf5::getAttrDimensionality(_impl->parent, attributeName.c_str());
		}

		template <class DataType, class ObjectType>
		void pullData(size_t numElems, 
			const std::string &attributeName, 
			std::vector<Data_Types::All_Variant_type> &data, 
			std::shared_ptr<ObjectType> obj)
		{
			std::vector<DataType> tdata(numElems);
			icedb::fs::hdf5::readAttrVector(obj, attributeName.c_str(), tdata);
			data.resize(numElems);
			std::copy(tdata.cbegin(), tdata.cend(), data.begin());
		}

		void CanHaveAttributes::readAttributeData(
			const std::string &attributeName,
			std::vector<Data_Types::All_Variant_type> &data)
		{
			Expects(doesAttributeExist(attributeName));
			auto sz = getAttributeDimensionality(attributeName);
			size_t numElems = 1;
			for (const auto &s : sz) numElems *= s;
			
			// Need to read into an array of the exact data type, then copy into the
			// variant structure of data.
			if (icedb::fs::hdf5::isType<uint64_t, H5::H5Object>(_impl->parent, attributeName))
				pullData<uint64_t, H5::H5Object>(numElems, attributeName, data, _impl->parent);
			else if (icedb::fs::hdf5::isType<int64_t, H5::H5Object>(_impl->parent, attributeName))
				pullData<int64_t, H5::H5Object>(numElems, attributeName, data, _impl->parent);
			else if (icedb::fs::hdf5::isType<float, H5::H5Object>(_impl->parent, attributeName))
				pullData<float, H5::H5Object>(numElems, attributeName, data, _impl->parent);
			else if (icedb::fs::hdf5::isType<double, H5::H5Object>(_impl->parent, attributeName))
				pullData<double, H5::H5Object>(numElems, attributeName, data, _impl->parent);
			else if (icedb::fs::hdf5::isType<char, H5::H5Object>(_impl->parent, attributeName))
				pullData<char, H5::H5Object>(numElems, attributeName, data, _impl->parent);
			else throw(std::exception("Unhandled data type"));
		}

		template <class DataType, class ObjectType>
		void pushData(
			const std::string &attributeName,
			const std::vector<size_t> &dimensionality,
			std::shared_ptr<ObjectType> obj,
			const std::vector<Data_Types::All_Variant_type> &indata
			)
		{
			size_t numElems = 1;
			for (const auto &s : dimensionality) numElems *= s;
			std::vector<DataType> data(numElems);
			for (size_t i = 0; i < numElems; ++i)
				data[i] = std::get<DataType>(indata[i]);

			icedb::fs::hdf5::addAttrVector(obj, attributeName.c_str(), dimensionality, data);
		}

		void CanHaveAttributes::writeAttributeData(
			const std::string &attributeName,
			const type_info &type_id,
			const std::vector<size_t> &dimensionality,
			const std::vector<Data_Types::All_Variant_type> &data)
		{
			if (doesAttributeExist(attributeName)) deleteAttribute(attributeName);
			// Need to copy from the variant structure into an array of the exact data type
			if (type_id == typeid(uint64_t))pushData<uint64_t, H5::H5Object>(attributeName, dimensionality, _impl->parent, data);
			else if (type_id == typeid(int64_t))pushData<int64_t, H5::H5Object>(attributeName, dimensionality, _impl->parent, data);
			else if (type_id == typeid(float))pushData<float, H5::H5Object>(attributeName, dimensionality, _impl->parent, data);
			else if (type_id == typeid(double))pushData<double, H5::H5Object>(attributeName, dimensionality, _impl->parent, data);
			else if (type_id == typeid(char))pushData<char, H5::H5Object>(attributeName, dimensionality, _impl->parent, data);
			else throw(std::exception("Unhandled data type"));
			//if (icedb::Data_Types::Is_Valid_Data_Type(type_id)) throw;
		}

		void CanHaveAttributes::deleteAttribute(const std::string &attributeName)
		{
			_impl->parent->removeAttr(attributeName);
		}

		bool CanHaveAttributes::doesAttributeExist(const std::string &attributeName) const 
		{
			return _impl->parent->attrExists(attributeName);
		}

		std::vector<std::string> CanHaveAttributes::getAttributeNames() const {
			//icedb::fs::hdf5::
			std::vector<std::string> anames;
			int numAttrs = _impl->parent->getNumAttrs();
			for (int i = 0; i < numAttrs; ++i) {
				H5::Attribute attr = _impl->parent->openAttribute(i);
				anames.push_back(attr.getName());
			}
			return anames;
		}
		//type_info CanHaveAttributes::getAttributeTypeId(const std::string &attributeName) const { 
		//	return typeid(std::string); }
	}
}