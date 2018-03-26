#pragma once
#include "defs.h"
#include <cstdint>
#include <memory>
#include <typeinfo>
#include <typeindex> // C++ 11 really is required.
//#include "compat/variant_backend.hpp"

namespace H5 {
	class H5Object;
	class Group;
	class DataSet;
	class CommonFG;
	class H5Location;
}
namespace icedb {

	/// Functions to detect the type of stored data
	namespace Data_Types {
		/// Is the type (T) a type supported for I/O by this library?
		template <class T> constexpr bool Is_Valid_Data_Type() { return false; }
		template<> constexpr bool Is_Valid_Data_Type<uint64_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<int64_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<uint32_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<int32_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<uint16_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<int16_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<uint8_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<int8_t>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<float>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<double>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<char>() { return true; }
		template<> constexpr bool Is_Valid_Data_Type<std::string>() { return true; }
		/// Is the type (type_id) a type supported for I/O by this library?
		inline bool Is_Valid_Data_Type(const std::type_info &type_id) {
			if (type_id == typeid(uint64_t)) return true;
			else if (type_id == typeid(int64_t)) return true;
			else if (type_id == typeid(uint32_t)) return true;
			else if (type_id == typeid(int32_t)) return true;
			else if (type_id == typeid(uint16_t)) return true;
			else if (type_id == typeid(int16_t)) return true;
			else if (type_id == typeid(uint8_t)) return true;
			else if (type_id == typeid(int8_t)) return true;
			else if (type_id == typeid(float)) return true;
			else if (type_id == typeid(double)) return true;
			else if (type_id == typeid(char)) return true;
			else if (type_id == typeid(std::string)) return true;
			return false;
		}

		//typedef variant<uint64_t, int64_t, uint32_t, int32_t, uint16_t, int16_t, uint8_t, int8_t, float, double, char, std::string> All_Variant_type;

		template <class T> const std::type_index getType() { return typeid(T); }
	}
}
