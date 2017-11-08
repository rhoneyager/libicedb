#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Data_Types.hpp"

namespace H5 {
	class H5Object;
}
namespace icedb {
	namespace Attributes {
		struct Attribute_impl;
		class CanHaveAttributes;
		template <class DataType> class Attribute {
			friend class CanHaveAttributes;
			std::shared_ptr<Attribute_impl> _impl;
		protected:
			Attribute() {
				static_assert(icedb::Data_Types::Is_Valid_Data_Type<DataType>() == true,
					"Attributes must be a valid data type");
			}
		public:
			typedef DataType type;
			std::vector<DataType> data;
			std::string name;

			Attribute(const std::string &name) : name{ name }, Attribute() {}
		};

		class CanHaveAttributes {
			class CanHaveAttributes_impl;
			std::shared_ptr<CanHaveAttributes_impl> _impl;
			void readAttributeData(const std::string &attributeName, std::vector<Data_Types::All_Variant_type> &data);
			void writeAttributeData(const std::string &attributeName, const std::vector<Data_Types::All_Variant_type> &data);
		protected:
			CanHaveAttributes(std::shared_ptr<H5::H5Object>);
		public:
			std::vector<std::string> getAttributeNames() const;
			bool doesAttributeExist(const std::string &attributeName) const;
			type_info getAttributeTypeId(const std::string &attributeName) const {}
			template<class Type> bool isAttributeOfType(const std::string &attributeName) const {
				type_info atype = getAttributeTypeId(attributeName);
				if (atype == typeid(Type)) return true;
				return false;
			}
			template<class DataType> Attribute<DataType> readAttribute(const std::string &attributeName) {
				std::vector<Data_Types::All_Variant_type> vdata;
				readAttributeData(attributeName, vdata);
				Attribute<DataType> res(attributeName);
				res.data;
				
			}
			template<class DataType> void writeAttribute(const Attribute<DataType> &attribute) {}
			void deleteAttribute(const std::string &attributeName);
		};

	}
}