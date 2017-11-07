#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Data_Types.hpp"

namespace icedb {
	namespace Attributes {
		template <class DataType> class Attribute {
			class Attribute_impl;
			std::shared_ptr<Attribute_impl> _impl;
		public:
			typedef DataType type;
			std::vector<DataType> data;
			const std::string name;

			Attribute() {
				static_assert(icedb::Data_Types::Is_Valid_Data_Type<DataType>() == true,
					"Attributes must be a valid data type");
			}
		};

		class CanHaveAttributes {
		public:
			std::vector<std::string> getAttributeNames() const;
			bool doesAttributeExist(const std::string &attributeName) const;
			template<class Type> bool isAttributeOfType(const std::string &attributeName) const {
				return false;
			}
			type_info getAttributeTypeId(const std::string &attributeName) const {}
			template<class DataType> Attribute<DataType> openAttribute(const std::string &attributeName) {}
			template<class DataType> void writeAttribute(const Attribute<DataType> &attribute) {}
			void deleteAttribute(const std::string &attributeName);
		};

	}
}