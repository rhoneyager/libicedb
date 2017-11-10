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

		bool CanHaveAttributes::valid() const {
			if (!_impl) return false;
			if (!_impl->parent) return false;
			return true;
		}

		void CanHaveAttributes::_setAttributeParent(std::shared_ptr<H5::H5Object> obj)
		{
			_impl->parent = obj;
		}

		CanHaveAttributes::CanHaveAttributes(std::shared_ptr<H5::H5Object> obj)
		{
			_impl = std::make_shared<CanHaveAttributes_impl>(obj);
		}

		CanHaveAttributes::CanHaveAttributes() {
			_impl = std::make_shared<CanHaveAttributes_impl>();
		}


		void CanHaveAttributes::deleteAttribute(const std::string &attributeName)
		{
			Expects(valid());
			_impl->parent->removeAttr(attributeName);
		}

		bool CanHaveAttributes::doesAttributeExist(const std::string &attributeName) const
		{
			Expects(valid());
			return _impl->parent->attrExists(attributeName);
		}

		std::vector<std::string> CanHaveAttributes::getAttributeNames() const {
			Expects(valid());
			//icedb::fs::hdf5::
			std::vector<std::string> anames;
			int numAttrs = _impl->parent->getNumAttrs();
			for (int i = 0; i < numAttrs; ++i) {
				H5::Attribute attr = _impl->parent->openAttribute(i);
				anames.push_back(attr.getName());
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
			std::shared_ptr<ObjectType> obj)
		{
			// Already assuming that the attribute exists (see calling function).

			std::vector<DataType> tdata;

			icedb::fs::hdf5::DataContainerType datatype = icedb::fs::hdf5::getAttributeGroupingType(obj, attributeName.c_str());
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
			const std::string &attributeName,
			std::vector<size_t> &dims,
			std::vector<Data_Types::All_Variant_type> &data)
		{
			Expects(valid());
			Expects(doesAttributeExist(attributeName));
			//auto sz = getAttributeDimensionality(attributeName);
			//size_t numElems = 1;
			//for (const auto &s : sz) numElems *= s;
			
			// Need to read into an array of the exact data type, then copy into the
			// variant structure of data.
			if (icedb::fs::hdf5::isType<uint64_t, H5::H5Object>(_impl->parent, attributeName))
				pullData<uint64_t, H5::H5Object>(attributeName, dims, data, _impl->parent);
			else if (icedb::fs::hdf5::isType<int64_t, H5::H5Object>(_impl->parent, attributeName))
				pullData<int64_t, H5::H5Object>(attributeName, dims, data, _impl->parent);
			else if (icedb::fs::hdf5::isType<float, H5::H5Object>(_impl->parent, attributeName))
				pullData<float, H5::H5Object>(attributeName, dims, data, _impl->parent);
			else if (icedb::fs::hdf5::isType<double, H5::H5Object>(_impl->parent, attributeName))
				pullData<double, H5::H5Object>(attributeName, dims, data, _impl->parent);
			else if (icedb::fs::hdf5::isType<char, H5::H5Object>(_impl->parent, attributeName))
				pullData<char, H5::H5Object>(attributeName, dims, data, _impl->parent);
			else if (icedb::fs::hdf5::isType<std::string, H5::H5Object>(_impl->parent, attributeName))
				pullData<std::string, H5::H5Object>(attributeName, dims, data, _impl->parent);
			else throw(std::exception("Unhandled data type"));
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
				data[i] = std::get<DataType>(indata[i]);

			if (isString<DataType>()) {
				/// \todo Re-work the internal logic to allow multiple string writes into a single attribute.
				Expects(dimensionality.size() == 1);
				Expects(dimensionality[0] == 1);
				icedb::fs::hdf5::addAttr(obj, attributeName.c_str(), data[0]);
			}
			else {
				if ((dimensionality.size() == 1) && !forceArray)
					icedb::fs::hdf5::addAttrVector(obj, attributeName.c_str(), data);
				else icedb::fs::hdf5::addAttrArray(obj, attributeName.c_str(), dimensionality, data);
			}
		}

		void CanHaveAttributes::writeAttributeData(
			const std::string &attributeName,
			const type_info &type_id,
			const std::vector<size_t> &dimensionality,
			const std::vector<Data_Types::All_Variant_type> &data)
		{
			Expects(valid());
			if (doesAttributeExist(attributeName)) deleteAttribute(attributeName);
			// Need to copy from the variant structure into an array of the exact data type
			if (type_id == typeid(uint64_t))pushData<uint64_t, H5::H5Object>(attributeName, dimensionality, _impl->parent, data);
			else if (type_id == typeid(int64_t))pushData<int64_t, H5::H5Object>(attributeName, dimensionality, _impl->parent, data);
			else if (type_id == typeid(float))pushData<float, H5::H5Object>(attributeName, dimensionality, _impl->parent, data);
			else if (type_id == typeid(double))pushData<double, H5::H5Object>(attributeName, dimensionality, _impl->parent, data);
			else if (type_id == typeid(char))pushData<char, H5::H5Object>(attributeName, dimensionality, _impl->parent, data);
			else if (type_id == typeid(std::string))pushData<std::string, H5::H5Object>(attributeName, dimensionality, _impl->parent, data);
			else throw(std::exception("Unhandled data type"));
			//if (icedb::Data_Types::Is_Valid_Data_Type(type_id)) throw;
		}

	}
}