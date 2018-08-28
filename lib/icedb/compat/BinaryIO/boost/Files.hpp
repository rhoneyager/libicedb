#pragma once
#if defined _MSC_FULL_VER
#pragma warning(push)
#pragma warning( disable : 4996 ) // Warning in boost
#endif

#include "../Files.hpp"

#include <iostream>
#include <fstream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/stream.hpp>
#include <sstream>

#ifdef BIO_ENABLE_COMPRESS_BZIP2
#if BIO_ENABLE_COMPRESS_BZIP2
#include <boost/iostreams/filter/bzip2.hpp>
#endif
#endif
#ifdef BIO_ENABLE_COMPRESS_GZIP
#if BIO_ENABLE_COMPRESS_GZIP
#include <boost/iostreams/filter/gzip.hpp>
#endif
#endif
#ifdef BIO_ENABLE_COMPRESS_ZLIB
#if BIO_ENABLE_COMPRESS_ZLIB
#include <boost/iostreams/filter/zlib.hpp>
#endif
#endif

namespace bIO {
	inline bool prep_decompression(const std::string &meth, boost::iostreams::filtering_istream &sbuf)
	{
		using namespace boost::iostreams;
#ifdef BIO_ENABLE_COMPRESS_BZIP2
#if BIO_ENABLE_COMPRESS_BZIP2
		if (meth == "bzip2")
		{
			sbuf.push(bzip2_decompressor());
			return true;
		}
#endif
#endif
#ifdef BIO_ENABLE_COMPRESS_GZIP
#if BIO_ENABLE_COMPRESS_GZIP
		if (meth == "gzip")
		{
			sbuf.push(gzip_decompressor());
			return true;
		}
#endif
#endif
#ifdef BIO_ENABLE_COMPRESS_ZLIB
#if BIO_ENABLE_COMPRESS_ZLIB
		if (meth == "zlib")
		{
			//sbuf.push(gzip_decompressor());
			sbuf.push(zlib_decompressor());
			return true;
		}
#endif
#endif
		return false;
	}

	inline BIO_NODISCARD HasError_t readCompressedFileToBuffer(
		const std::string& filename, std::vector<bIO::byte> &buffer, const std::string &compressionMethod)
	{
		Expects(buffer.size());
		//auto pFile = _impl::openFile(filename);
		std::ifstream pFile(filename.c_str());
		using namespace boost::iostreams;
		filtering_istream sin;
		if (compressionMethod.size())
			Expects(prep_decompression(compressionMethod, sin));
		sin.push(pFile);

		size_t sz = buffer.size();
		// Irritating boost iostreams missing feature: unsigned character arrays.
		basic_array_sink<char> a_sink{ (char*)buffer.data(), sz };
		//basic_array_sink<bIO::byte> a_sink{ buffer.data(), sz };
		//stream<decltype(a_sink)> os{ a_sink };
		copy(sin, a_sink);

		return false;
	}

	inline std::pair<bool, std::string> isCompressed(const std::string &filename)
	{
		std::string extension = filename.substr(filename.find_last_of('.'));
		std::string cmeth;
		if (extension == ".Z") cmeth = "zlib";
		if (extension == ".gz") cmeth = "gzip";
		if (extension == ".bz2") cmeth = "bzip2";

		return std::make_pair(cmeth.size() > 0, cmeth);
	}

	inline BIO_NODISCARD HasError_t readCompressedFileToBuffer(
		const std::string& filename, std::vector<bIO::byte> &buffer)
	{
		auto cinfo = isCompressed(filename);
		return readCompressedFileToBuffer(filename, buffer, cinfo.second);
	}

}


#if defined _MSC_FULL_VER
//#pragma warning(pop)
#endif
