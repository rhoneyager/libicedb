#pragma once
#include "../Types.hpp"
#include <array>
#include <complex>
#include <vector>

namespace HH {
	namespace Types {
		template <class ComplexDataType>
		HH_hid_t GetHDF5TypeComplex()
		{
			typedef typename ComplexDataType::value_type value_type;
			// Complex number is a compound datatype of two objects.
			return GetHDF5Type<value_type, 1>({ 2 });
		}

#define HH_SPECIALIZE_COMPLEX(x) \
template<> inline HH_hid_t GetHDF5Type< x >(std::initializer_list<hsize_t>, void*) { return GetHDF5TypeComplex< x >(); }
		HH_SPECIALIZE_COMPLEX(std::complex<double>);
		HH_SPECIALIZE_COMPLEX(std::complex<float>);
#undef HH_SPECIALIZE_COMPLEX

		// Complex number accessor:
		// Strictly, the real and imaginary parts are internal to the complex number. They must be
		// marshalled.

		// Handles complex number reads and writes from/to HDF5.
		template <class ComplexDataType>
		struct Object_Accessor_Complex
		{
		private:
			typedef typename ComplexDataType::value_type value_type;
			std::vector<value_type > _buf;
		public:
			Object_Accessor_Complex(ssize_t sz = -1) {}
			/// \brief Converts an object into a void* array that HDF5 can natively understand.
			/// \note The shared_ptr takes care of "deallocation" when we no longer need the "buffer".
			const void* serialize(::gsl::span<const ComplexDataType> d)
			{
				_buf.resize(d.size() * 2);
				for (size_t i = 0; i < (size_t)d.size(); ++i) {
					_buf[(2 * i) + 0] = d.at(i).real();
					_buf[(2 * i) + 1] = d.at(i).imag();
				}
				return (const void*)_buf.data();
				//return std::shared_ptr<const void>((const void*)d.data(), [](const void*) {});
			}
			/// \brief Gets the size of the buffer needed to store the object from HDF5. Used
			/// in variable-length string / complex object reads.
			/// \note For POD objects, we do not have to allocate a buffer.
			/// \returns Size needed. If negative, then we can directly write to the object, 
			/// sans allocation or deallocation.
			ssize_t getFromBufferSize() {
				return -1;
			}
			/// \brief Allocates a buffer that HDF5 can read/write into; used later as input data for object construction.
			/// \note For POD objects, we can directly write to the object.
			void marshalBuffer(ComplexDataType * objStart) {}
			/// \brief Construct an object from an HDF5-provided data stream, 
			/// and deallocate any temporary buffer.
			/// \note For trivial (POD) objects, there is no need to do anything.
			void deserialize(ComplexDataType *objStart) {}
			void freeBuffer() {}
		};

		template<> struct Object_Accessor<std::complex<double> > : public Object_Accessor_Complex<std::complex<double> > {};
		template<> struct Object_Accessor<std::complex<float> > : public Object_Accessor_Complex<std::complex<float> > {};

		/*
		template<>
		struct Object_Accessor<std::string>
		{
		private:
			// Do serialization by making copies of each of the strings.
			std::vector<char*> _bufStrPointers;
			std::vector<std::unique_ptr<char[]> > _bufStrs;
		public:
			Object_Accessor(ssize_t sz = 0) : _bufStrPointers(sz), _bufStrs(sz) {}
			~Object_Accessor() {}
			/// \note Remember: the return value does not persist past another call to serialize, and
			/// it does not persist past object lifetime.
			const void* serialize(::gsl::span<const std::string> d)
			{
				_bufStrPointers.clear();
				_bufStrs.clear();
				for (const auto &s : d) {
					size_t sz = s.size() + 1;
					std::unique_ptr<char[]> sobj(new char[sz]);
					ICEDB_COMPAT_strncpy_s(sobj.get(), sz, s.data(), sz);
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
		};
		*/
	}
}
