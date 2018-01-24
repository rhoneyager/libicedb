#include "../icedb/Attribute.hpp"
#include "../private/Attribute_impl.hpp"
#include "../private/hdf5_load.h"
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
			if (fs::hdf5::isType<uint32_t, H5::H5Object>(parent.get(), attributeName)) return (typeid(uint32_t));
			if (fs::hdf5::isType<int32_t, H5::H5Object>(parent.get(), attributeName)) return (typeid(int32_t));
			if (fs::hdf5::isType<uint16_t, H5::H5Object>(parent.get(), attributeName)) return (typeid(uint16_t));
			if (fs::hdf5::isType<int16_t, H5::H5Object>(parent.get(), attributeName)) return (typeid(int16_t));
			if (fs::hdf5::isType<uint8_t, H5::H5Object>(parent.get(), attributeName)) return (typeid(uint8_t));
			if (fs::hdf5::isType<int8_t, H5::H5Object>(parent.get(), attributeName)) return (typeid(int8_t));
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
			/** \note Despite the documentation in HDF5 1.8, the attrExists
			 * function does not, in fact, exist! Tested on HDF5 1.8.5-patch1 on
			 * CentOS6, GCC 7. Docs state that this is a method of H5Object.
			 **/
			Expects(valid());
#if ICEDB_H5_HASH5OBJECTATTREXISTS == 1
			return _getAttributeParent()->attrExists(attributeName);
#else
			hid_t objid = _getAttributeParent()->getId();
			htri_t res = H5Aexists(objid, attributeName.c_str());
			if (res < 0) throw;
			return (res > 0) ? true : false;
#endif
		}

		bool CanHaveAttributes::doesAttributeExist(
			gsl::not_null<const H5::H5Object*> parent, const std::string &attributeName)
		{
			/** \note Despite the documentation in HDF5 1.8, the attrExists
			 * function does not, in fact, exist! Tested on HDF5 1.8.5-patch1 on
			 * CentOS6, GCC 7. Docs state that this is a method of H5Object.
			 **/
#if ICEDB_H5_HASH5OBJECTATTREXISTS == 1
			return parent->attrExists(attributeName);
#else
			hid_t objid = parent->getId();
			htri_t res = H5Aexists(objid, attributeName.c_str());
			if (res < 0) throw;
			return (res > 0) ? true : false;
#endif
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
		template <class DataType>
		void pullData(
			const std::string &attributeName, 
			std::vector<size_t> &dims,
			std::vector<DataType> &tdata,
			gsl::not_null<const H5::H5Object*> obj)
		{
			/// \note Assumes that the attribute exists (must be checked by calling function).
			/// \todo Use Checked_Existing_Attribute
			/// \see Checked_Existing_Attribute

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
		}

		template <class DataType>
		void CanHaveAttributes::readAttributeData(
			gsl::not_null<const H5::H5Object*> parent,
			const std::string &attributeName,
			std::vector<size_t> &dims,
			std::vector<DataType> &data)
		{
			Expects(doesAttributeExist(parent,attributeName));

			if (icedb::fs::hdf5::isType<DataType, H5::H5Object>(parent.get(), attributeName))
				pullData<DataType>(attributeName, dims, data, parent.get());
			else throw(std::invalid_argument("Unhandled data type"));
		}

		template <class DataType>
		void CanHaveAttributes::readAttributeData(
			const std::string &attributeName,
			std::vector<size_t> &dims,
			std::vector<DataType> &data) const
		{
			Expects(valid());
			Expects(doesAttributeExist(attributeName));
			auto parent = _getAttributeParent();

			readAttributeData<DataType>(parent.get(), attributeName, dims, data);
		}



#define INST_READ_ATTR_TYPE(x) \
		template void CanHaveAttributes::readAttributeData<x>( \
			gsl::not_null<const H5::H5Object*> parent, \
			const std::string &attributeName, \
			std::vector<size_t> &dims, \
			std::vector<x> &data) ; \
		template void CanHaveAttributes::readAttributeData<x>( \
			const std::string &attributeName,\
			std::vector<size_t> &dims, \
			std::vector<x> &data) const;
		
		//INST_READ_ATTR_TYPE(double);
		INST_ATTR(INST_READ_ATTR_TYPE);

		template <class DataType, class ObjectType>
		void pushData(
			const std::string &attributeName,
			const std::vector<size_t> &dimensionality,
			std::shared_ptr<ObjectType> obj,
			const std::vector<DataType> &data,
			bool forceArray = false
			)
		{
			size_t numElems = 1;
			for (const auto &s : dimensionality) numElems *= s;
			Expects(numElems > 0);

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

		template <class DataType>
		void CanHaveAttributes::writeAttributeData(
			const std::string &attributeName,
			const std::vector<size_t> &dimensionality,
			const std::vector<DataType> &data)
		{
			Expects(valid());
			if (doesAttributeExist(attributeName)) deleteAttribute(attributeName);
			auto parent = _getAttributeParent();
			// Need to copy from the variant structure into an array of the exact data type

			//if (icedb::fs::hdf5::isType<DataType, H5::H5Object>(parent.get(), attributeName))
				//pullData<DataType>(attributeName, dims, data, parent.get());
			pushData<DataType, H5::H5Object>(attributeName, dimensionality, parent, data);
		}

#define INST_WRITE_ATTR_TYPE(x) \
		template void CanHaveAttributes::writeAttributeData( \
		const std::string &attributeName, \
			const std::vector<size_t> &dimensionality, \
			const std::vector<x> &data);

		INST_ATTR(INST_WRITE_ATTR_TYPE);
	}
}
