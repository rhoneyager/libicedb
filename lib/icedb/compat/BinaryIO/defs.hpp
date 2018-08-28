#pragma once
#ifndef BIO_DEFS_HPP
#define BIO_DEFS_HPP

#ifndef __cplusplus
#error "This code requires a C++ compiler to work!"
#endif

#ifndef __STDC_WANT_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif
#include <cstddef>

// Annoylingly, the value of the __cplusplus macro is inconsistently
// set on compilers before 2017. There is no good way to do feature detection.

#if defined(__cplusplus) || defined(c_plusplus)
# define BIO_BEGIN_DECL
# define BIO_END_DECL
#else
# define BIO_BEGIN_DECL
# define BIO_END_DECL
#endif

#if defined(__cplusplus) || defined(c_plusplus)
# define BIO_BEGIN_DECL_C     extern "C" {
# define BIO_END_DECL_C       }
# define BIO_CALL_C extern "C"
#else
# define BIO_BEGIN_DECL_C
# define BIO_END_DECL_C
# define BIO_CALL_C
#endif

#if defined(__cplusplus) || defined(c_plusplus)
# define BIO_BEGIN_DECL_CPP     
# define BIO_END_DECL_CPP
# define BIO_CALL_CPP
#else
# define BIO_BEGIN_DECL_CPP extern "CPP" {
# define BIO_END_DECL_CPP }
# define BIO_CALL_CPP extern "CPP"
#endif

BIO_BEGIN_DECL_C

/* Compiler and version diagnostics */

/* Detection of the operating system and compiler version. Used to declare symbol export / import. */

/* Declare the feature sets that are supported */
//#define BIO_FEATURE_GZIP 0 /* Auto-detection of gzip. Needed for hdf5 file storage. */


/* Symbol export / import macros */
#if defined _MSC_FULL_VER
#define BIO_COMPILER_EXPORTS_VERSION_A
#elif defined __INTEL_COMPILER
#define BIO_COMPILER_EXPORTS_VERSION_B
#elif defined __GNUC__
#define BIO_COMPILER_EXPORTS_VERSION_B
#elif defined __MINGW32__
#define BIO_COMPILER_EXPORTS_VERSION_B
#elif defined __clang__
#define BIO_COMPILER_EXPORTS_VERSION_B
#else
#define BIO_COMPILER_EXPORTS_VERSION_UNKNOWN
#endif

// Defaults for static libraries
#define BIO_SHARED_EXPORT
#define BIO_SHARED_IMPORT
#define BIO_HIDDEN
#define BIO_PRIVATE

#if defined BIO_COMPILER_EXPORTS_VERSION_A
#undef BIO_SHARED_EXPORT
#undef BIO_SHARED_IMPORT
#define BIO_SHARED_EXPORT __declspec(dllexport)
#define BIO_SHARED_IMPORT __declspec(dllimport)
#elif defined BIO_COMPILER_EXPORTS_VERSION_B
#undef BIO_SHARED_EXPORT
#undef BIO_SHARED_IMPORT
#undef BIO_HIDDEN
#undef BIO_PRIVATE
#define BIO_SHARED_EXPORT __attribute__ ((visibility("default")))
#define BIO_SHARED_IMPORT __attribute__ ((visibility("default")))
#define BIO_HIDDEN __attribute__ ((visibility("hidden")))
#define BIO_PRIVATE __attribute__ ((visibility("internal")))
#else
#pragma message("BinaryIO defs.hpp warning: compiler is unrecognized")
#endif

// OS definitions
#ifdef __unix__
#ifdef __linux__
#define BIO_OS_LINUX
#endif
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__) || defined(__APPLE__)
#define BIO_OS_UNIX
#endif
#endif
#ifdef _WIN32
#define BIO_OS_WINDOWS
#endif
#if !defined(_WIN32) && !defined(BIO_OS_UNIX) && !defined(BIO_OS_LINUX)
#define BIO_OS_UNSUPPORTED
#endif


// If SHARED_(libname) is defined, then the target library both 
// exprts and imports. If not defined, then it is a static library.

// Macros defined as EXPORTING_(libname) are internal to the library.
// They indicate that SHARED_EXPORT_SDBR should be used.
// If EXPORTING_ is not defined, then SHARED_IMPORT_SDBR should be used.

#if SHARED_BinaryIO
# if EXPORTING_BinaryIO
#  define BIO_DL BIO_SHARED_EXPORT
# else
#  define BIO_DL BIO_SHARED_IMPORT
# endif
#else
# define BIO_DL BIO_SHARED_EXPORT
#endif
/* Symbol export / import macros */
#define BIO_SYMBOL_SHARED BIO_DL
#define BIO_SYMBOL_PRIVATE BIO_HIDDEN


#ifdef _MSC_FULL_VER
#define BIO_DEBUG_FSIG __FUNCSIG__
#endif
#if defined(__GNUC__) || defined(__clang__)
#if defined(__PRETTY_FUNCTION__)
#define BIO_DEBUG_FSIG __PRETTY_FUNCTION__
#elif defined(__func__)
#define BIO_DEBUG_FSIG __func__
#elif defined(__FUNCTION__)
#define BIO_DEBUG_FSIG __FUNCTION__
#else
#define BIO_DEBUG_FSIG ""
#endif
#endif

#define BIO_WIDEN2(x) L ## x
#define BIO_WIDEN(x) BIO_WIDEN2(x)
/* Global exception raising code (invokes debugger) */
//BIO_SYMBOL_SHARED void BIO_DEBUG_RAISE_EXCEPTION_HANDLER_A(const char*, int, const char*);
//BIO_SYMBOL_SHARED void BIO_DEBUG_RAISE_EXCEPTION_HANDLER_WC(const wchar_t*, int, const wchar_t*);
//#define BIO_DEBUG_RAISE_EXCEPTION() BIO_DEBUG_RAISE_EXCEPTION_HANDLER_WC( BIO_WIDEN(__FILE__), (int)__LINE__, BIO_WIDEN(BIO_DEBUG_FSIG));

//#define BIO_DEBUG_RAISE_EXCEPTION_HANDLER BIO_DEBUG_RAISE_EXCEPTION_HANDLER_A
/* Global error codes. */
//#define BIO_GLOBAL_ERROR_TODO 999
#if (defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)) && (__STDC_WANT_LIB_EXT1__==1)
# define BIO_SECURE_FOPEN
# define BIO_USING_SECURE_STRINGS 1
# define BIO_DEFS_COMPILER_HAS_FPRINTF_S
# define BIO_DEFS_COMPILER_HAS_FPUTS_S
#else
# define _CRT_SECURE_NO_WARNINGS
#endif

/* Thread local storage stuff */
#ifdef _MSC_FULL_VER
#define BIO_THREAD_LOCAL __declspec( thread )
#endif
#ifdef __GNUC__
#define BIO_THREAD_LOCAL __thread
#endif

// Compiler interface warning suppression
#if defined _MSC_FULL_VER
//#pragma warning(push)
//#pragma warning( disable : 4003 ) // Bug in boost with VS2016.3
//#pragma warning( disable : 4251 ) // DLL interface
//#pragma warning( disable : 4275 ) // DLL interface
#endif

/// Pointer to an object that is modfied by a function
#define BIO_OUT
/// Denotes an 'optional' parameter (one which can be replaced with a NULL or nullptr)
#define BIO_OPTIONAL

BIO_END_DECL_C

// Errata:

//#ifndef BIO_NO_ERRATA
// Boost bug with C++17 requires this define. See https://stackoverflow.com/questions/41972522/c2143-c2518-when-trying-to-compile-project-using-boost-multiprecision
//#ifndef _HAS_AUTO_PTR_ETC
//#define _HAS_AUTO_PTR_ETC	(!_HAS_CXX17)
//#endif /* _HAS_AUTO_PTR_ETC */
//#endif

#if defined(__cplusplus)
# if __cplusplus >= 201703L
#  if __has_cpp_attribute(gnu::warn_unused_result)
//#   define BIO_NODISCARD [[gnu::warn_unused_result]]
#   define BIO_NODISCARD __attribute__ ((warn_unused_result))
#  elif __has_cpp_attribute(nodiscard)
#   define BIO_NODISCARD [[nodiscard]]
#  else
#   define BIO_NODISCARD
#  endif
# else
#  define BIO_NODISCARD
# endif
# else
#  define BIO_NODISCARD
#endif

#if __cplusplus >= 201703L
# if __has_include("Eigen/Dense")
#  define BIO_USING_EIGEN 1
# endif
#else
# define BIO_USING_EIGEN 0
#endif

namespace bIO {
	typedef bool HasError_t;
//#if __cplusplus >= 201703L
//	typedef std::byte byte;
//#else
	typedef unsigned char byte;
//#endif
}

#if 0
#if __cplusplus >= 201703L
# if __has_include("_bio_builddefs.h")
#  include "_bio_builddefs.h"
# else
#  define BIO_ENABLE_COMPRESS_BZIP2 0
#  define BIO_ENABLE_COMPRESS_GZIP 0
#  define BIO_ENABLE_COMPRESS_ZLIB 0
# endif
#else
# include "_bio_builddefs.h"
#endif
#endif

#endif
