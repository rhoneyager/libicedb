#pragma once

#include "defs.hpp"
#include <cstddef>
#include <type_traits>
#include "gsl/gsl"
#include <array>
#include <vector>
#include <cstdio>
#include <iostream>

#ifdef BIO_USING_EIGEN
# include "Eigen/Dense"
#endif

BIO_BEGIN_DECL_CPP

namespace bIO {
	namespace _impl {
#ifdef BIO_SECURE_FOPEN
		inline void bIO_memcpy(void* const dest, size_t destSz, const void* const src, size_t srcSz)
		{
			errno_t e = memcpy_s(dest, destSz, src, srcSz);
			if (e != 0) {
				std::cerr << "Cannot open file!" << std::endl;
				throw;
			}
		}
#else
		inline void bIO_memcpy(void* const dest, size_t, const void* const src, size_t srcSz)
		{
			//void* res = 
			memcpy(dest, src, srcSz);
		}
#endif
	}

	template <int blockSize = 4>
	void reverseBufferBasic(bIO::byte buffer[], size_t sz)
	{
		Expects(sz % blockSize == 0);
		for (bIO::byte* it = buffer; it < buffer + sz; it += blockSize)
		{
			bIO::byte* i = it;
			bIO::byte* j = i + blockSize;
			std::reverse(i, j);
		}
	}

	template <>
	inline void reverseBufferBasic<4>(bIO::byte buffer[], size_t sz)
	{
		constexpr int blockSize = 4;
		Expects(sz % blockSize == 0);
		for (bIO::byte* it = buffer; it < buffer + sz; it += blockSize)
		{
#if defined(_MSC_FULL_VER)
			//unsigned long _byteswap_ulong(unsigned long value);
			// Explanation of implementation:
			//unsigned long *a = (unsigned long*)(it);
			//unsigned long b = *a;
			//unsigned long c = _byteswap_ulong(b);
			//unsigned long *d = (unsigned long*)(it);
			//*d = c;
			auto ot = (unsigned long*)(it);
			*ot = _byteswap_ulong(*(unsigned long*)(it));

			//*it = (std::byte) c;
			//*it = (std::byte) _byteswap_ulong((unsigned long)(*it));
			//#elif defined(__GNUC__)
			//int32_t __builtin_bswap32 (int32_t x)
			//*it = (int32_t)__builtin_bswap32((int32_t)(*it));
#else
			bIO::byte* i = it;
			bIO::byte* j = i + blockSize;
			std::reverse(i, j);
#endif
		}
	}

	template <int blockSize = 4>
	void reverseBuffer(bIO::byte buffer[], size_t sz)
	{
		reverseBufferBasic<blockSize>(buffer, sz);
	}

	enum class Endianness { ENDIAN_LITTLE, ENDIAN_BIG, ENDIAN_IGNORE };

	struct Buffer_Accessor {
		bIO::byte** buf = nullptr;
		FILE* fbuf = nullptr;
		size_t cur_offset = 0;
		size_t max_sz = 0;
		Endianness endianness = Endianness::ENDIAN_LITTLE;
		int record_padding_bytes = 0;
		const Endianness get_system_endianness() {
			union {
				uint32_t i;
				char c[4];
			} bint = { 0x01020304 };

			if (bint.c[0] == 1) return Endianness::ENDIAN_BIG;
			return Endianness::ENDIAN_LITTLE;
		}
		const bool need_swap() {
			if (endianness == Endianness::ENDIAN_IGNORE) return false;
			if ((get_system_endianness() == Endianness::ENDIAN_BIG)
				&& (endianness == Endianness::ENDIAN_LITTLE)) return true;
			if ((get_system_endianness() == Endianness::ENDIAN_LITTLE)
				&& (endianness == Endianness::ENDIAN_BIG)) return true;
			return false;
		}

		Buffer_Accessor(bIO::byte * *b, Endianness e = Endianness::ENDIAN_BIG, size_t max_size = 0)
			: buf(b), endianness(e), max_sz(max_size) {}

		Buffer_Accessor(FILE * f, Endianness e = Endianness::ENDIAN_BIG, size_t max_size = 0)
			: fbuf(f), endianness(e), max_sz(max_size) {}

		bool _padIsOpen = false;
	};

	/// Kept separate even though is is a one-liner. For code clarity.
	inline void advance(Buffer_Accessor & src, int num_bytes) {
		if (src.max_sz && (src.cur_offset + num_bytes >= src.max_sz)) throw;
		if (src.buf)
			* (src.buf) += num_bytes;
		else if (src.fbuf) {
			if (fseek(src.fbuf, num_bytes, SEEK_CUR)) {
				throw; // If fseek returns a nonzero value, then an error occurred.
			}
		}
		else throw;
		src.cur_offset += num_bytes;
	}

	template <typename OutputType, int Bytes = sizeof(OutputType)> //, bool PreserveBuffer = true>
	OutputType read(Buffer_Accessor & src)
	{
		if (src.max_sz && (src.cur_offset + Bytes >= src.max_sz)) throw;
		OutputType res;
		if (src.buf) {
			if (src.need_swap())
				reverseBuffer<Bytes>(*(src.buf), Bytes);
			res = *(OutputType*)(*src.buf);
			advance(src, Bytes);
		}
		else if (src.fbuf) {
			bIO::byte obuf[Bytes];
			if (fread(obuf, 1, Bytes, src.fbuf)) throw;
			if (src.need_swap())
				reverseBuffer<Bytes>(obuf, Bytes);
			res = *(OutputType*)(obuf);
		}
		else throw;
		return res;
	}

	template <typename OutputType, int Bytes = sizeof(OutputType)>
	void write(Buffer_Accessor & dest, const OutputType & obj)
	{
		if (dest.max_sz && (dest.cur_offset + Bytes >= dest.max_sz)) throw;
		if (dest.buf) {
			_impl::bIO_memcpy(*dest.buf, Bytes, (const void*)(&obj), Bytes);
			if (dest.need_swap())
				reverseBuffer(*(dest.buf), Bytes);
			advance(dest, Bytes);
		}
		else if (dest.fbuf) {
			bIO::byte obuf[Bytes];
			_impl::bIO_memcpy(obuf, Bytes, (const void*)(&obj), Bytes);
			if (dest.need_swap())
				reverseBuffer(obuf, Bytes);
			fwrite(obuf, 1, Bytes, dest.fbuf);
		}
		else throw;
	}


	inline void writePad(Buffer_Accessor & src, uint64_t reclen_in_bytes = 0) {
		// reclen_in_bytes is only set on write operations.
		if (!reclen_in_bytes) return;
		// Check that the record length fits in the number of available padding bytes.
		size_t max_reclen_in_bytes = (uint64_t)1 << ((uint64_t)8 * (uint64_t)src.record_padding_bytes);
		if (reclen_in_bytes >= max_reclen_in_bytes) throw;
		switch (src.record_padding_bytes)
		{
		case 4:
		{
			uint32_t conv_reclen = (uint32_t)reclen_in_bytes;
			write<uint32_t>(src, conv_reclen);
		}
		break;
		case 8:
			write<uint64_t>(src, reclen_in_bytes);
			break;
		default:
			throw; // You monster!
			break;
		}
	}
	inline bool PadOpen(Buffer_Accessor & src, uint64_t write_reclen_in_bytes = 0)
	{
		const bool doPad = !src._padIsOpen;
		if (doPad) {
			src._padIsOpen = true;
			if (write_reclen_in_bytes) writePad(src, write_reclen_in_bytes); // The if statement is syntactic fluff (readability).
			if (!write_reclen_in_bytes) advance(src, src.record_padding_bytes); // writePad invokes write, which advances automatically.
		}
		return doPad;
	}
	inline void PadClose(Buffer_Accessor & src, uint64_t write_reclen_in_bytes = 0)
	{
		if (write_reclen_in_bytes) writePad(src, write_reclen_in_bytes); // The if statement is syntactic fluff (readability).
		if (!write_reclen_in_bytes) advance(src, src.record_padding_bytes); // writePad invokes write, which advances automatically.
		src._padIsOpen = false;
	}

	template <typename OutputType>
	void pop(Buffer_Accessor & src, OutputType & varout)
	{
		const bool doPad = PadOpen(src);
		constexpr int sz = sizeof(OutputType);
		varout = read<typename std::remove_reference<OutputType>::type>(src);
		//advance(src, sz); // Now part of read.
		if (doPad) PadClose(src);
	}

	template <typename OutputType, typename... Rest>
	void pop(Buffer_Accessor & src, OutputType && varout, Rest && ... rest)
	{
		const bool doPad = PadOpen(src);
		pop<OutputType>(src, varout);
		pop<Rest...>(src, std::forward<Rest>(rest)...);
		if (doPad) PadClose(src);
	}

	// Support code for reads into a structure
	template <typename StructType, typename ObjType>
	void popStruct(
		Buffer_Accessor & src,
		StructType & out,
		ptrdiff_t offset)
	{
		const bool doPad = PadOpen(src);
		/// \note Neither static_cast nor reinterpret_cast handle all cases!
		bIO::byte* obj_as_bytes = reinterpret_cast<bIO::byte*>(&out);
		bIO::byte* obj_write_location = obj_as_bytes + offset;
		ObjType temp;
		pop<ObjType>(src, temp);
		std::memcpy(obj_write_location, &temp, sizeof(ObjType));
		if (doPad) PadClose(src);
	}

	template <typename StructType, typename ObjType, typename... Rest>
	void popStruct(
		Buffer_Accessor & src,
		StructType & out,
		const gsl::span<ptrdiff_t> & members)
	{
		const bool doPad = PadOpen(src);
		popStruct<StructType, ObjType>(src, out, *(members.begin()));
		/// \note Old code
		/// auto poppedmembers = members.make_subspan(1, members.size() - 1, gsl::dynamic_extent);
		auto poppedmembers = gsl::span<ptrdiff_t>(members.data() + 1, members.size() - 1);
		popStruct<StructType, ObjType>(src, out, poppedmembers);
		if (doPad) PadClose(src);
	}

	// Support code for reads into an array.
	template <typename ArrayType, typename ObjType>
	void popArray(
		Buffer_Accessor & src,
		ArrayType * start,
		ArrayType * end,
		ptrdiff_t arrayMemberOffset = 0)
	{
		Expects(start);
		Expects(end);
		Expects(start <= end);
		const bool doPad = PadOpen(src);
		for (auto a = start; a < end; ++a) {
			popStruct<ArrayType, ObjType>(src, *a, arrayMemberOffset);
		}
		if (doPad) PadClose(src);
	}


#ifdef BIO_USING_EIGEN
	// Support code for reads into Eigen. Usually, make an Eigen::Block for storage.
	// Assumes that we span only a single dimension.
	template <class EigenClass>
	void popEigen(
		Buffer_Accessor & src,
		EigenClass && out)
	{
		const bool doPad = PadOpen(src);
		const auto sz = out.size();
		for (int i = 0; i < out.rows(); ++i) {
			for (int j = 0; j < out.cols(); ++j) {
				//auto pre = out(i, j);
				pop(src, out(i, j));
				//auto post = out(i, j);
				//auto diff = post - pre;
			}
		}
		if (doPad) PadClose(src);
	}
#endif

	template <typename OutputType>
	size_t getRecl(const OutputType &) {
		return sizeof(typename std::remove_reference<OutputType>::type);
	}

	template <typename OutputType, typename... Rest>
	size_t getRecl(const OutputType & o, Rest&&... rest)
	{
		size_t a = getRecl<OutputType>(o);
		a += getRecl<Rest...>(std::forward<Rest>(rest)...);
		return a;
	}
	
	template <typename OutputType>
	void push(Buffer_Accessor &src, const OutputType &varout)
	{
		const bool doPad = PadOpen(src, getRecl<OutputType>(varout));
		//constexpr int sz = sizeof(OutputType);
		write<typename std::remove_reference<OutputType>::type>(src, varout);
		//advance(src, sz);
		if (doPad) PadClose(src, getRecl<OutputType>(varout));
	}
	
	template <typename OutputType, typename... Rest>
	void push(Buffer_Accessor &src, const OutputType &varout, Rest&&... rest)
	{
		const bool doPad = PadOpen(src, getRecl<OutputType>(varout) + getRecl<Rest...>(std::forward<Rest>(rest)...));
		push<OutputType>(src, varout);
		push<Rest...>(src, std::forward<Rest>(rest)...);
		if (doPad) PadClose(src, getRecl<OutputType>(varout) + getRecl<Rest...>(std::forward<Rest>(rest)...));
	}

#ifdef BIO_USING_EIGEN
	// Support code for writes from Eigen. Usually, make an Eigen::Block for storage.
	// Assumes that we span only a single dimension.
	template <class EigenClass>
	void pushEigen(
		Buffer_Accessor &src,
		const EigenClass &out)
	{
		const auto sz = out.size();
		const size_t rec_sz_bytes = sizeof(typename EigenClass::Scalar) * sz;

		const bool doPad = PadOpen(src, rec_sz_bytes);
		for (int i = 0; i < out.rows(); ++i) {
			for (int j = 0; j < out.cols(); ++j) {
				//auto pre = out(i, j);
				push(src, out(i, j));
				//auto post = out(i, j);
				//auto diff = post - pre;
			}
		}
		if (doPad) PadClose(src, rec_sz_bytes);
	}
#endif


}


/// Provides a method for calculating the offsets from std::arrays of data
#define BIO_ARRAYOFFSET(TYPE, INDEX) [](){TYPE a; return (size_t) &a[INDEX] - (size_t) &a; }()


BIO_END_DECL_CPP

