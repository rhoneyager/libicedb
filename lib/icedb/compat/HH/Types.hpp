#pragma once
#include "defs.hpp"
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <string>
#include <string_view>
#include <gsl/gsl>
#include <hdf5.h>
#include "Handles.hpp"

namespace HH {
	namespace _impl {
		inline size_t COMPAT_strncpy_s(
			char * dest, size_t destSz,
			const char * src, size_t srcSz)
		{
			/** \brief Safe char array copy.
			\returns the number of characters actually written.
			\param dest is the pointer to the destination. Always null terminated.
			\param destSz is the size of the destination buller, including the trailing null character.
			\param src is the pointer to the source. Characters from src are copied either until the
			first null character or until srcSz. Note that null termination comes later.
			\param srcSz is the max size of the source buffer.
			**/
			if (!dest || !src) throw;
#if HH_USING_SECURE_STRINGS
			strncpy_s(dest, destSz, src, srcSz);
#else
			if (srcSz <= destSz) {
				strncpy(dest, src, srcSz);
			}
			else {
				strncpy(dest, src, destSz);
			}
#endif
			dest[destSz - 1] = 0;
			for (size_t i = 0; i < destSz; ++i) if (dest[i] == '\0') return i;
			return 0; // Should never be reached
		}

	}
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
			constexpr int _Variable_Length = -1;
			//constexpr int _Not_An_Array_type = -3;
		}

		/// Accessor for class-like objects
		//template <class DataType, class ObjType = DataType>
		//DataType* Accessor(ObjType* e) { return e; }
		//template <>
		//const char* Accessor<const char*, std::string>(std::string* e) { return e.c_str(); }

		/// For fundamental, non-string types.
		/// \note Template specializations are implemented for the actual data types, like int32_t, double, etc.
		/// \todo Change these signatures to allow for user extensibility into custom structs,
		/// or even objects like std::complex<T>.
		template <class DataType, int Array_Type_Dimensionality = 0>
		HH_hid_t GetHDF5Type(
			std::initializer_list<hsize_t> Adims = {},
			typename std::enable_if<!is_string<DataType>::value>::type* = 0)
		{
			if (Array_Type_Dimensionality <= 0) {
				//static_assert(false, "HH::Types::GetHDF5Type does not understand this data type.");
				throw std::string("HH::Types::GetHDF5Type does not understand this data type.");
				return HH_hid_t(-1, HH::Handles::Closers::DoNotClose::CloseP); // Should never reach this. Invalid handle, just in case.
			} else {
				// This is a compound array type of the same object, repeated.
				HH_hid_t fundamental_type = GetHDF5Type<DataType, 0>();
				hid_t t = H5Tarray_create2(fundamental_type(), Array_Type_Dimensionality, Adims.begin());
				Expects(t >= 0);
				return HH_hid_t(t, HH::Handles::Closers::CloseHDF5Datatype::CloseP);
			}
		}
		/// For fundamental string types. These are either constant or variable length arrays. Separate handling elsewhere.
		template <class DataType, int String_Type_Length = constants::_Variable_Length>
		HH_hid_t GetHDF5Type(
			int Runtime_String_Type_Length = constants::_Variable_Length,
			typename std::enable_if<is_string<DataType>::value>::type* = 0)
		{
			size_t strtlen = String_Type_Length;
			if (Runtime_String_Type_Length != constants::_Variable_Length) strtlen = Runtime_String_Type_Length;
			if (strtlen == constants::_Variable_Length) strtlen = H5T_VARIABLE;
			hid_t t = H5Tcreate(H5T_STRING, strtlen);
			Expects(t >= 0);
			return HH_hid_t(t, HH::Handles::Closers::CloseHDF5Datatype::CloseP);
		}

		template<> inline HH_hid_t GetHDF5Type<char>(std::initializer_list<hsize_t>, void*) { return HH_hid_t(H5T_NATIVE_CHAR); }
		template<> inline HH_hid_t GetHDF5Type<int8_t>(std::initializer_list<hsize_t>, void*) { return HH_hid_t(H5T_NATIVE_INT8); }
		template<> inline HH_hid_t GetHDF5Type<uint8_t>(std::initializer_list<hsize_t>, void*) { return HH_hid_t(H5T_NATIVE_UINT8); }
		template<> inline HH_hid_t GetHDF5Type<int16_t>(std::initializer_list<hsize_t>, void*) { return HH_hid_t(H5T_NATIVE_INT16); }
		template<> inline HH_hid_t GetHDF5Type<uint16_t>(std::initializer_list<hsize_t>, void*) { return HH_hid_t(H5T_NATIVE_UINT16); }
		template<> inline HH_hid_t GetHDF5Type<int32_t>(std::initializer_list<hsize_t>, void*) { return HH_hid_t(H5T_NATIVE_INT32); }
		template<> inline HH_hid_t GetHDF5Type<uint32_t>(std::initializer_list<hsize_t>, void*) { return HH_hid_t(H5T_NATIVE_UINT32); }
		template<> inline HH_hid_t GetHDF5Type<int64_t>(std::initializer_list<hsize_t>, void*) { return HH_hid_t(H5T_NATIVE_INT64); }
		template<> inline HH_hid_t GetHDF5Type<uint64_t>(std::initializer_list<hsize_t>, void*) { return HH_hid_t(H5T_NATIVE_UINT64); }
		template<> inline HH_hid_t GetHDF5Type<float>(std::initializer_list<hsize_t>, void*) { return HH_hid_t(H5T_NATIVE_FLOAT); }
		template<> inline HH_hid_t GetHDF5Type<double>(std::initializer_list<hsize_t>, void*) { return HH_hid_t(H5T_NATIVE_DOUBLE); }

		/// Function to tell if a datatype is of constant or variable length.

		/// \note HDF5 wants void* types. These are horribly hacked :-(
		/// \note By default, we are using the POD accessor. Valid for simple data types,
		/// where multiple objects are in the same dataspace, and each object is a 
		/// singular instance of the base data type.

		struct Object_Accessor_Base
		{
			Object_Accessor_Base() {}
			virtual ~Object_Accessor_Base() {}
			virtual bool isCompound() const { return false; }
			virtual ssize_t getFromBufferSize() const { return -1; }
		};

		template <class DataType>
		struct Object_Accessor : public Object_Accessor_Base
		{
		private:
			void *_buffer;
		public:
			Object_Accessor(ssize_t sz = -1) {}
			virtual ~Object_Accessor() {}
			/// \brief Converts an object into a void* array that HDF5 can natively understand.
			/// \note The shared_ptr takes care of "deallocation" when we no longer need the "buffer".
			const void* serialize(::gsl::span<const DataType> d)
			{
				return (const void*)d.data();
				//return std::shared_ptr<const void>((const void*)d.data(), [](const void*) {});
			}
			/// \brief Gets the size of the buffer needed to store the object from HDF5. Used
			/// in variable-length string / complex object reads.
			/// \note For POD objects, we do not have to allocate a buffer.
			/// \returns Size needed. If negative, then we can directly write to the object, 
			/// sans allocation or deallocation.
			virtual ssize_t getFromBufferSize() {
				return -1;
			}
			/// \brief Allocates a buffer that HDF5 can read/write into; used later as input data for object construction.
			/// \note For POD objects, we can directly write to the object.
			void marshalBuffer(DataType * objStart) { _buffer = static_cast<void*>(objStart); }
			/// \brief Construct an object from an HDF5-provided data stream, 
			/// and deallocate any temporary buffer.
			/// \note For trivial (POD) objects, there is no need to do anything.
			void deserialize(DataType *objStart) { }
			void freeBuffer() {}
		};

		template<>
		struct Object_Accessor<std::string>
			: public Object_Accessor_Base
		{
		private:
			// Do serialization by making copies of each of the strings.
			std::vector<char*> _bufStrPointers;
			std::vector<std::unique_ptr<char[]> > _bufStrs;
		public:
			Object_Accessor(ssize_t sz = 0) : _bufStrPointers(sz), _bufStrs(sz) {}
			virtual ~Object_Accessor() {}
			/// \note Remember: the return value does not persist past another call to serialize, and
			/// it does not persist past object lifetime.
			const void* serialize(::gsl::span<const std::string> d)
			{
				_bufStrPointers.clear();
				_bufStrs.clear();
				for (const auto &s : d) {
					size_t sz = s.size() + 1;
					std::unique_ptr<char[]> sobj(new char[sz]);
					_impl::COMPAT_strncpy_s(sobj.get(), sz, s.data(), sz);
					_bufStrPointers.push_back(sobj.get());
					_bufStrs.push_back(std::move(sobj));
				}
				return (const void*)_bufStrPointers.data();
				//return std::shared_ptr<const void>(_bufStrPointers.data(), [](const void*) {});
			}
			/// \note For this class specialization, _buffer is marshaled on object construction.
			void marshalBuffer(std::string* objStart) { }
			void deserialize(std::string* objStart) {
				//*objStart = std::string(_buffer.get()[0], _sz);
				//*objStart = std::string(_buffer.get(), _sz);
			}
			/// HDF5 will allocate character strings on read. These should all be freed.
			void freeBuffer() {
				//for (ssize_t i = 0; i < _sz; ++i) delete[] _buffer.get()[i];
			}
			virtual bool isCompound() const override { return true; }
			virtual ssize_t getFromBufferSize() const override {
				return -1;
			}
		};


		/*
		template<>
		struct Object_Accessor<char*>
		{
		private:
		std::unique_ptr<char[]> _buffer;
		ssize_t _sz; ///< Size (in bytes) of _buffer.
		public:
		/// \param sz is the size of the string (+ NULL-termination) to be
		/// read. Only needed if reading the object. Should be passed in via the
		/// appropriate calls to the HDF5 backend to determine the string's size.
		Object_Accessor(ssize_t sz = -1) : _sz(sz), _buffer(new char[sz]) {}
		~Object_Accessor() {}
		std::shared_ptr<const void> serialize(const char* d)
		{
		return std::shared_ptr<const void>((const void*)d, [](const void*) {});
		}
		ssize_t getFromBufferSize() {
		return _sz;
		}
		/// \note For this class specialization, _buffer is marshaled on object construction.
		void marshalBuffer(char** objStart) { }
		void deserialize(char **objStart) {
		std::memcpy(*objStart, _buffer.get(), _sz);
		}
		void freeBuffer() {} ///< No need in this impl., as the unique-ptr frees itself.
		};
		*/

	}
}
