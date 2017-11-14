#pragma once
#include <cstdint>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include <variant>

namespace H5 {
	class H5Object;
	class Group;
	class DataSet;
}
namespace icedb {

	namespace Data_Types {
		template <class T> constexpr bool Is_Valid_Data_Type() { return false; }
		template<> constexpr bool Is_Valid_Data_Type<uint64_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<int64_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<float>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<double>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<char>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<std::string>() { return true; }
		inline bool Is_Valid_Data_Type(const std::type_info &type_id) {
			if (type_id == typeid(uint64_t)) return true;
			else if (type_id == typeid(int64_t)) return true;
			else if (type_id == typeid(float)) return true;
			else if (type_id == typeid(double)) return true;
			else if (type_id == typeid(char)) return true;
			else if (type_id == typeid(std::string)) return true;
			return false;
		}

		typedef std::variant<uint64_t, int64_t, float, double, char, std::string> All_Variant_type;

		template <class T> const std::type_index getType() { return typeid(T); }
	}
}
