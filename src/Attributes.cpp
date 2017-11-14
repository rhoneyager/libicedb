#include "../icedb/Attribute.hpp"
#include "../private/Attribute_impl.hpp"
#include "../icedb/compat/hdf5_load.h"
#include "../private/hdf5_supplemental.hpp"
#include <gsl/gsl_assert>
#include <stdexcept>

namespace icedb {
	namespace Attributes {

		CanHaveAttributes::CanHaveAttributes() {}
		CanHaveAttributes::~CanHaveAttributes() {}


		CanHaveAttributes_impl::CanHaveAttributes_impl() {}
		CanHaveAttributes_impl::CanHaveAttributes_impl(std::shared_ptr<H5::H5Object> parent) : parent(parent) {}
		CanHaveAttributes_impl::~CanHaveAttributes_impl() {}

		bool CanHaveAttributes::valid() const {
			if (!_getAttributeParent()) return false;
			return true;
		}

		void CanHaveAttributes_impl::_setAttributeParent(std::shared_ptr<H5::H5Object> obj)
		{
			parent = obj;
		}

		std::shared_ptr<H5::H5Object> CanHaveAttributes_impl::_getAttributeParent() const
		{
			return parent;
		}

		std::type_index CanHaveAttributes::getAttributeTypeId(gsl::not_null<const H5::H5Object*> parent, const std::string &attributeName) {
			if (fs::hdf5::isType<uint64_t, H5::H5Object>(parent.get(), attributeName)) return (typeid(uint64_t));
			if (fs::hdf5::isType<int64_t, H5::H5Object>(parent.get(), attributeName)) return (typeid(int64_t));
			if (fs::hdf5::isType<double, H5::H5Object>(parent.get(), attributeName)) return (typeid(double));
			if (fs::hdf5::isType<float, H5::H5Object>(parent.get(), attributeName)) return (typeid(float));
			if (fs::hdf5::isType<char, H5::H5Object>(parent.get(), attributeName)) return (typeid(char));
			if (fs::hdf5::isType<std::string, H5::H5Object>(parent.get(), attributeName)) return (typeid(std::string));
			throw;
		}

		std::type_index CanHaveAttributes::getAttributeTypeId(const std::string &attributeName) const {
			auto parent = _getAttributeParent();
			return getAttributeTypeId(parent.get(), attributeName);
		}

		void CanHaveAttributes::deleteAttribute(const std::string &attributeName)
		{
			Expects(valid());
			_getAttributeParent()->removeAttr(attributeName);
		}

		bool CanHaveAttributes::doesAttributeExist(const std::string &attributeName) const
		{
			Expects(valid());
			return _getAttributeParent()->attrExists(attributeName);
		}

		bool CanHaveAttributes::doesAttributeExist(
			gsl::not_null<const H5::H5Object*> parent, const std::string &attributeName)
		{
			return parent->attrExists(attributeName);
		}

		std::set<std::string> CanHaveAttributes::getAttributeNames() const {
			Expects(valid());
			//icedb::fs::hdf5::
			std::set<std::string> anames;
			int numAttrs = _getAttributeParent()->getNumAttrs();
			for (int i = 0; i < numAttrs; ++i) {
				H5::Attribute attr = _getAttributeParent()->openAttribute(i);
				anames.insert(attr.getName());
			}
			return anames;
		}

		/// Used to flag string types for special treatment.
		template <class DataType>
		constexpr bool isString() { return false; }
		template<> constexpr bool isString<std::string>() { return true; }

		/** \brief Template function to pull data from the HDF5 object.
		*
		* Use HDF5 function to query the dataspace, to first see if it is an array, a vector 
		* (not a std::vector), or a scalar value. Get and set dimensions, then resize the
		* input data array. Then, read data and copy into the appropriate Variant buffer (data).
		**/
		template <class DataType, class ObjectType>
		void pullData(
			const std::string &attributeName, 
			std::vector<size_t> &dims,
			std::vector<Data_Types::All_Variant_type> &data, 
			gsl::not_null<const ObjectType*> obj)
		{
			// Already assuming that the attribute exists (see calling function).

			std::vector<DataType> tdata;

			const icedb::fs::hdf5::DataContainerType datatype = icedb::fs::hdf5::getAttributeGroupingType(obj, attributeName.c_str());
			if (datatype == icedb::fs::hdf5::DataContainerType::BASIC) {
				icedb::fs::hdf5::readAttrVector(obj, attributeName.c_str(), tdata);
				dims.resize(1, tdata.size());
			}
			else if (datatype == icedb::fs::hdf5::DataContainerType::ARRAY) {
				icedb::fs::hdf5::readAttrArray(obj, attributeName.c_str(), dims, tdata);
			}
			else if (datatype == icedb::fs::hdf5::DataContainerType::STRING) {
				/// \todo Change code when reading multiple strings
				tdata.resize(1);
				icedb::fs::hdf5::readAttr(obj, attributeName.c_str(), tdata[0]);
				//throw;
			}
			else if (datatype == icedb::fs::hdf5::DataContainerType::VLEN) {
				throw;
			}
			else throw;

			data.resize(tdata.size());
			std::copy(tdata.cbegin(), tdata.cend(), data.begin());
		}

		void CanHaveAttributes::readAttributeData(
			gsl::not_null<const H5::H5Object*> parent,
			const std::string &attributeName,
			std::vector<size_t> &dims,
			std::vector<Data_Types::All_Variant_type> &data)
		{
			Expects(doesAttributeExist(parent,attributeName));
			//auto sz = getAttributeDimensionality(attributeName);
			//size_t numElems = 1;
			//for (const auto &s : sz) numElems *= s;
			
			// Need to read into an array of the exact data type, then copy into the
			// variant structure of data.
			if (icedb::fs::hdf5::isType<uint64_t, H5::H5Object>(parent.get(), attributeName))
				pullData<uint64_t, H5::H5Object>(attributeName, dims, data, parent.get());
			else if (icedb::fs::hdf5::isType<int64_t, H5::H5Object>(parent.get(), attributeName))
				pullData<int64_t, H5::H5Object>(attributeName, dims, data, parent.get());
			else if (icedb::fs::hdf5::isType<float, H5::H5Object>(parent.get(), attributeName))
				pullData<float, H5::H5Object>(attributeName, dims, data, parent.get());
			else if (icedb::fs::hdf5::isType<double, H5::H5Object>(parent.get(), attributeName))
				pullData<double, H5::H5Object>(attributeName, dims, data, parent.get());
			else if (icedb::fs::hdf5::isType<char, H5::H5Object>(parent.get(), attributeName))
				pullData<char, H5::H5Object>(attributeName, dims, data, parent.get());
			else if (icedb::fs::hdf5::isType<std::string, H5::H5Object>(parent.get(), attributeName))
				pullData<std::string, H5::H5Object>(attributeName, dims, data, parent.get());
			else throw(std::invalid_argument("Unhandled data type"));
		}

		void CanHaveAttributes::readAttributeData(
			const std::string &attributeName,
			std::vector<size_t> &dims,
			std::vector<Data_Types::All_Variant_type> &data) const
		{
			Expects(valid());
			Expects(doesAttributeExist(attributeName));
			auto parent = _getAttributeParent();

			readAttributeData(parent.get(), attributeName, dims, data);
		}

		
		template <class DataType, class ObjectType>
		void pushData(
			const std::string &attributeName,
			const std::vector<size_t> &dimensionality,
			std::shared_ptr<ObjectType> obj,
			const std::vector<Data_Types::All_Variant_type> &indata,
			bool forceArray = false
			)
		{
			size_t numElems = 1;
			for (const auto &s : dimensionality) numElems *= s;
			Expects(numElems > 0);
			std::vector<DataType> data(numElems);
			for (size_t i = 0; i < numElems; ++i)
#if (have_stdcpplib_variant==1)
					data[i] = std::get<DataType>(indata[i]);
#else
					data[i] = mpark::get<DataType>(indata[i]);
#endif

			if (isString<DataType>()) {
				/// \todo Re-work the internal logic to allow multiple string writes into a single attribute.
				Expects(dimensionality.size() == 1);
				Expects(dimensionality[0] == 1);
				icedb::fs::hdf5::addAttr<DataType, ObjectType>(obj.get(), attributeName.c_str(), data[0]);
			}
			else {
				if ((dimensionality.size() == 1) && !forceArray)
					icedb::fs::hdf5::addAttrVector<DataType, ObjectType>(obj.get(), attributeName.c_str(), data);
				else icedb::fs::hdf5::addAttrArray<DataType, ObjectType>(obj.get(), attributeName.c_str(), dimensionality, data);
			}
		}

		void CanHaveAttributes::writeAttributeData(
			const std::string &attributeName,
			const std::type_info &type_id,
			const std::vector<size_t> &dimensionality,
			const std::vector<Data_Types::All_Variant_type> &data)
		{
			Expects(valid());
			if (doesAttributeExist(attributeName)) deleteAttribute(attributeName);
			auto parent = _getAttributeParent();
			// Need to copy from the variant structure into an array of the exact data type
			if (type_id == typeid(uint64_t))pushData<uint64_t, H5::H5Object>(attributeName, dimensionality, parent, data);
			else if (type_id == typeid(int64_t))pushData<int64_t, H5::H5Object>(attributeName, dimensionality, parent, data);
			else if (type_id == typeid(float))pushData<float, H5::H5Object>(attributeName, dimensionality, parent, data);
			else if (type_id == typeid(double))pushData<double, H5::H5Object>(attributeName, dimensionality, parent, data);
			else if (type_id == typeid(char))pushData<char, H5::H5Object>(attributeName, dimensionality, parent, data);
			else if (type_id == typeid(std::string))pushData<std::string, H5::H5Object>(attributeName, dimensionality, parent, data);
			else throw(std::invalid_argument("Unhandled data type"));
			//if (icedb::Data_Types::Is_Valid_Data_Type(type_id)) throw;
		}

	}
}
