#pragma once
#include <typeindex>
#include <typeinfo>
#include <string>
#include <string_view>
#include <gsl/gsl_util>
#include <hdf5.h>
#include "Handles.hpp"

namespace HH {
	namespace Types {
		using namespace HH::Handles;
		/// \todo extend to UTF-8 strings, as HDF5 supports these. No support for UTF-16, but conversion functions may be applied.
		/// \todo Fix for "const std::string".
		template<typename T>
		struct is_string :
			public std::integral_constant<bool,
			std::is_same<char*, typename std::decay<T>::type>::value ||
			std::is_same<const char*, typename std::decay<T>::type>::value
			> {};

		template<> struct is_string<std::string> : std::true_type {};
		template<> struct is_string<std::string_view> : std::true_type {};

		namespace constants {
			constexpr int _Unlimited_Length = -1;
			constexpr int _Unspecified = -1;
			constexpr int _Not_An_Array_type = -2;
		}

		/// For fundamental, non-string types.
		/// \note Template specializations are implemented for the actual data types, like int32_t, double, etc.
		/// \todo Change these signatures to allow for user extensibility into custom structs,
		/// or even objects like std::complex<T>.
		template <class DataType>
		HH_hid_t GetHDF5Type(
			typename std::enable_if<!is_string<DataType>::value>::type* = 0)
		{
			static_assert(false, "GetHDF5Type does not understand this data type.");
			return H5_fundamental_handle{ -1 }; // Should never reach this. Invalid handle, just in case.
		}
		/// For fundamental string types.
		template <class DataType, int String_Type_Length = constants::_Unlimited_Length>
		HH_hid_t GetHDF5Type(
			int Runtime_String_Type_Length = constants::_Unspecified,
			typename std::enable_if<is_string<DataType>::value>::type* = 0)
		{
			int strtlen = String_Type_Length;
			if (Runtime_String_Type_Length != constants::_Unspecified) strtlen = Runtime_String_Type_Length;
			return H5T_handle{ H5Tcreate(H5T_STRING, gsl::narrow_cast<size_t>(strtlen)) };
		}

		template<> inline HH_hid_t GetHDF5Type<char>(void*) { return H5_fundamental_ScopedHandle{ H5T_NATIVE_CHAR }; }
		template<> inline HH_hid_t GetHDF5Type<int8_t>(void*) { return H5_fundamental_ScopedHandle{ H5T_NATIVE_INT8 }; }
		template<> inline HH_hid_t GetHDF5Type<uint8_t>(void*) { return H5_fundamental_ScopedHandle{ H5T_NATIVE_UINT8 }; }
		template<> inline HH_hid_t GetHDF5Type<int16_t>(void*) { return H5_fundamental_ScopedHandle{ H5T_NATIVE_INT16 }; }
		template<> inline HH_hid_t GetHDF5Type<uint16_t>(void*) { return H5_fundamental_ScopedHandle{ H5T_NATIVE_UINT16 }; }
		template<> inline HH_hid_t GetHDF5Type<int32_t>(void*) { return H5_fundamental_ScopedHandle{ H5T_NATIVE_INT32 }; }
		template<> inline HH_hid_t GetHDF5Type<uint32_t>(void*) { return H5_fundamental_ScopedHandle{ H5T_NATIVE_UINT32 }; }
		template<> inline HH_hid_t GetHDF5Type<int64_t>(void*) { return H5_fundamental_ScopedHandle{ H5T_NATIVE_INT64 }; }
		template<> inline HH_hid_t GetHDF5Type<uint64_t>(void*) { return H5_fundamental_ScopedHandle{ H5T_NATIVE_UINT64 }; }
		template<> inline HH_hid_t GetHDF5Type<float>(void*) { return H5_fundamental_ScopedHandle{ H5T_NATIVE_FLOAT }; }
		template<> inline HH_hid_t GetHDF5Type<double>(void*) { return H5_fundamental_ScopedHandle{ H5T_NATIVE_DOUBLE }; }

	}
}
