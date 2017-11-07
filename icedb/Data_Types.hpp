#pragma once
#include <cstdint>
#include <typeinfo>

namespace icedb {
	namespace Data_Types {
		template <class T> constexpr bool Is_Valid_Data_Type() { return false; }
		template<> constexpr bool Is_Valid_Data_Type<uint64_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<int64_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<float>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<double>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<char>() { return true; }
		inline bool Is_Valid_Data_Type(const type_info &type_id) {
			if (type_id == typeid(uint64_t)) return true;
			else if (type_id == typeid(int64_t)) return true;
			else if (type_id == typeid(float)) return true;
			else if (type_id == typeid(double)) return true;
			else if (type_id == typeid(char)) return true;
			return false;
		}
	}
}