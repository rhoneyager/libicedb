#pragma once
#include "defs.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <cstdio>
#include <cassert>
#include <cerrno>
#include <cstddef>

#include "gsl/gsl"

#if defined(BIO_OS_WINDOWS)
#include <Windows.h>
#elif defined(BIO_OS_UNIX) || defined(BIO_OS_LINUX) || defined(BIO_OS_APPLE)
#include <sys/stat.h>
#else
#error "This code builds only on POSIX platforms, macOS and Windows!"
#endif

namespace bIO {

	namespace _impl {
#ifdef BIO_SECURE_FOPEN
		inline std::unique_ptr < FILE, decltype(&fclose)> openFile(const std::string &filename, const char *desc = "rb")
		{
			FILE* fp = nullptr;
			errno_t errs = fopen_s(&fp, filename.c_str(), desc);
			if (errs != 0) {
				std::cerr << "Cannot open file!" << std::endl;
				throw;
			}
			return std::unique_ptr<FILE, decltype(&fclose)>(fp, &fclose);
		};
#else
		inline std::unique_ptr < FILE, decltype(&fclose)> openFile(const std::string &filename, const char *desc = "rb")
		{
			std::unique_ptr<FILE, decltype(&fclose)> pFile(fopen(filename.c_str(), desc), &fclose);
			return std::move(pFile);
		}
#endif


#if defined(__unix__)
		inline size_t getSize(const std::string &filename) {
			struct stat st;
			stat(filename.c_str(), &st);
			return st.st_size;
		}
#elif defined(BIO_OS_WINDOWS)
		inline size_t getSize(const std::string &filename) {
			BOOL bFlag = false;
			LARGE_INTEGER sz; // A 64-bit signed integer.
			HANDLE hFile = CreateFileA(filename.c_str(),
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				std::cerr << "Cannot open file handle!" << std::endl;
				std::cerr << GetLastError() << std::endl;
				throw;
			}
			bFlag = GetFileSizeEx(hFile, &sz);
			if (!bFlag) {
				std::cerr << GetLastError() << std::endl;
				throw;
			}

			bFlag = CloseHandle(hFile);
			if (!bFlag) {
				std::cerr << GetLastError() << std::endl;
				throw;
			}
			return static_cast<size_t>(sz.QuadPart);
		}
#else
		inline size_t getSize(const std::string &filename) {
			// This is problematic. See StackOverflow and C guidelines for why.
			// So, let's use this only as a method of last resort.
			auto pFile = openFile(filename);
			fseek(pFile.get(), 0L, SEEK_END);
			size_t sz = ftell(pFile.get());
			//rewind(pFile.get());
			return sz;
		}
#endif
	}
	
	using _impl::openFile;
	using _impl::getSize;
	//inline size_t getSize(const std::string &filename) { return _impl::getSize(filename); }

	/// \brief Convenience function for reading a whole file into a vector.
	/// \note If the file does not exist, or if filename points to a non-file object,
	/// then getSize and openFile will throw.
	inline HasError_t BIO_NODISCARD readFileToBuffer(const std::string& filename, std::vector<bIO::byte> &buffer, bool resizeBuffer = true)
	{
		auto pFile = _impl::openFile(filename);
		size_t sz = buffer.size();
		if (resizeBuffer) {
			sz = _impl::getSize(filename);
			buffer.resize(sz);
		}
		size_t szread = fread(buffer.data(), 1, sz, pFile.get());
		if (ferror(pFile.get())) return true;
		if (sz != szread) return true;
		return false;
	}

	inline HasError_t BIO_NODISCARD readToBuffer(
		BIO_IN FILE* f, size_t numBytesToReadFromF, bIO::byte_ptr buffer, size_t bufferMaxSize) noexcept
	{
		Expects(f);
		Expects(buffer);
		size_t numBytes = (numBytesToReadFromF < bufferMaxSize) ? numBytesToReadFromF : bufferMaxSize;
		size_t numBytesRead = fread(buffer, 1, numBytes, f);
		if (ferror(f)) return true;
		if (numBytesRead != numBytes) return true;
		return false;
	}
}

