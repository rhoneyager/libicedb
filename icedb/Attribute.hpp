#pragma once
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <gsl/gsl_assert>
#include <gsl/gsl>
#include "Data_Types.hpp"


namespace icedb {
	namespace Attributes {
		template <class DataType> class Attribute {
		protected:
			Attribute() : Attribute("", {}, {}) {}
		public:
			typedef DataType type;
			std::vector<DataType> data;
			std::string name;
			std::vector<size_t> dimensionality;
			inline bool isArray() const {
				if (dimensionality.size() > 1) return true;
				return false;
			}

			Attribute(const std::string &name) : Attribute(name, {}, {}) {}
			Attribute(const std::string &name, DataType val)
				: Attribute(name, { 1 }, { val }) {}
			Attribute(const std::string &name, std::initializer_list<size_t> dims, std::initializer_list<DataType> data)
				: name{ name }, dimensionality{ dims }, data{ data }
			{
				static_assert(icedb::Data_Types::Is_Valid_Data_Type<DataType>() == true,
					"Attributes must be a valid data type");

				//size_t sz = 1;
				//for (const auto &d : dims) sz *= d;
				//Expects(data.size() == sz);
			}
		};

		class CanHaveAttributes {
			bool valid() const;
		protected:
			CanHaveAttributes();
			virtual void _setAttributeParent(std::shared_ptr<H5::H5Object> obj) = 0;
			virtual std::shared_ptr<H5::H5Object> _getAttributeParent() const = 0;
		public:
			~CanHaveAttributes();
			bool doesAttributeExist(const std::string &attributeName) const;
			static bool doesAttributeExist(gsl::not_null<const H5::H5Object*> parent, const std::string &attributeName);
			std::type_index getAttributeTypeId(const std::string &attributeName) const;
			static std::type_index getAttributeTypeId(gsl::not_null<const H5::H5Object*> parent, const std::string &attributeName);
			template<class Type> bool isAttributeOfType(const std::string &attributeName) const {
				std::type_index atype = getAttributeTypeId(attributeName);
				if (atype == typeid(Type)) return true;
				return false;
			}

			std::set<std::string> getAttributeNames() const;
			void deleteAttribute(const std::string &attributeName);


			static void readAttributeData(
				gsl::not_null<const H5::H5Object*> parent,
				const std::string &attributeName,
				std::vector<size_t> &dimensions,
				std::vector<Data_Types::All_Variant_type> &data);
			void readAttributeData(
				const std::string &attributeName,
				std::vector<size_t> &dimensions,
				std::vector<Data_Types::All_Variant_type> &data) const;
			void writeAttributeData(
				const std::string &attributeName,
				const std::type_info &type_id,
				const std::vector<size_t> &dimensionas,
				const std::vector<Data_Types::All_Variant_type> &data);

			template <class DataType> static Attribute<DataType> readAttribute(
					gsl::not_null<const H5::H5Object*> obj, const std::string &attributeName)
			{
				std::vector<Data_Types::All_Variant_type> vdata;
				Attribute<DataType> res(attributeName);
				//res.dimensionality = getAttributeDimensionality(attributeName);
				readAttributeData(obj, attributeName, res.dimensionality, vdata);
				res.data.resize(vdata.size());
				for (size_t i = 0; i < vdata.size(); ++i)
					res.data[i] = std::get<DataType>(vdata[i]);
				//std::copy_n(vdata.cbegin(), vdata.cend(), res.data.begin());
				return res;
			}
			template<class DataType> Attribute<DataType> readAttribute(const std::string &attributeName) const {
				std::vector<Data_Types::All_Variant_type> vdata;
				Attribute<DataType> res(attributeName);
				//res.dimensionality = getAttributeDimensionality(attributeName);
				readAttributeData(attributeName, res.dimensionality, vdata);
				res.data.resize(vdata.size());
				for (size_t i = 0; i < vdata.size(); ++i)
					res.data[i] = std::get<DataType>(vdata[i]);
				//std::copy_n(vdata.cbegin(), vdata.cend(), res.data.begin());
				return res;
			}
			template<class DataType> void writeAttribute(const Attribute<DataType> &attribute) {
				size_t sz = 1;
				for (const auto &d : attribute.dimensionality) sz *= d;
				Expects(attribute.data.size() == sz);

				std::vector<Data_Types::All_Variant_type> vdata(attribute.data.size());
				for (size_t i = 0; i < attribute.data.size(); ++i)
					vdata[i] = attribute.data[i];
				// copy_n will not work here...
				//std::copy_n(attribute.data.cbegin(), attribute.data.cend(), vdata.begin());
				writeAttributeData(attribute.name, typeid(DataType), attribute.dimensionality, vdata);
			}
			template<class DataType> void writeAttribute(
				const std::string &name,
				std::initializer_list<size_t> dims,
				std::initializer_list<DataType> data) {
				Attribute<DataType> attr(name, dims, data);
				writeAttribute(attr);
			}


		};

	}
}
