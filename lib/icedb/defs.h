#pragma once
#ifndef ICEDB_H_DEFS
#define ICEDB_H_DEFS

#include <stddef.h>
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
#define ICEDB_USING_SECURE_STRINGS 1
#define ICEDB_DEFS_COMPILER_HAS_FPRINTF_S
#define ICEDB_DEFS_COMPILER_HAS_FPUTS_S
#endif

#if defined(__cplusplus) || defined(c_plusplus)
# define ICEDB_BEGIN_DECL
# define ICEDB_END_DECL
#else
# define ICEDB_BEGIN_DECL
# define ICEDB_END_DECL
#endif

#if defined(__cplusplus) || defined(c_plusplus)
# define ICEDB_BEGIN_DECL_C     extern "C" {
# define ICEDB_END_DECL_C       }
# define ICEDB_CALL_C extern "C"
#else
# define ICEDB_BEGIN_DECL_C
# define ICEDB_END_DECL_C
# define ICEDB_CALL_C
#endif

#if defined(__cplusplus) || defined(c_plusplus)
# define ICEDB_BEGIN_DECL_CPP     
# define ICEDB_END_DECL_CPP
# define ICEDB_CALL_CPP
#else
# define ICEDB_BEGIN_DECL_CPP extern "CPP" {
# define ICEDB_END_DECL_CPP }
# define ICEDB_CALL_CPP extern "CPP"
#endif


ICEDB_BEGIN_DECL_C

/* Compiler and version diagnostics */

/* Detection of the operating system and compiler version. Used to declare symbol export / import. */
// OS definitions
/**
 * \defgroup OS Preprocessor macros that relate to OS detection.
 * @{
 **/

#ifdef __unix__
# ifdef __linux__
#  define ICEDB_OS_LINUX
# endif
# if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
#  define ICEDB_OS_UNIX
# endif
#endif
#if (defined(__APPLE__) || defined(__MACH__))
# define ICEDB_OS_MACOS
#endif
#ifdef _WIN32
# define ICEDB_OS_WINDOWS
#endif
#if !defined(ICEDB_OS_WINDOWS) && !defined(ICEDB_OS_UNIX) && !defined(ICEDB_OS_LINUX) && !defined(ICEDB_OS_MACOS)
# define ICEDB_OS_UNSUPPORTED
# pragma message("ICEDB defs.h warning: operating system is unrecognized.")
#endif

 /**@}*/
/* Declare the feature sets that are supported */
//#define ICEDB_FEATURE_GZIP 0 /* Auto-detection of gzip. Needed for hdf5 file storage. */


/**
 * \defgroup Function_Tagging Defines that relate to function tagging.
 * @{
 **/

 // Definitions to get function signatures.
 // In C99 and C++11, __func__ should work in all cases for getting the function name.
 // However, I want the entire function signature.
#if defined(_MSC_FULL_VER)
# define ICEDB_FUNCSIG __FUNCSIG__
#else
# define ICEDB_FUNCSIG __PRETTY_FUNCTION__
#endif

// Convenience definitions: 
// These are nonexistant defines that are used to help humans read the code.

/// Tags a parameter that gets modfied by a function.
#define ICEDB_OUT
/// Denotes an 'optional' parameter (one which can be replaced with a NULL or nullptr)
#define ICEDB_OPTIONAL

/**@}*/

/* Symbol export / import macros */

/**
 * \defgroup Symbols_Shared Defines that relate to symbol export / import.
 * @{
 *
 * \def ICEDB_COMPILER_EXPORTS_VERSION_UNKNOWN
 * Defined when we have no idea what the compiler is.
 *
 * \def ICEDB_COMPILER_EXPORTS_VERSION_A
 * Defined when the compiler is like MSVC.
 *
 * \def ICEDB_COMPILER_EXPORTS_VERSION_B
 * Defined when the compiler is like GNU, Intel, or Clang.
 */

#if defined(_MSC_FULL_VER)
# define ICEDB_COMPILER_EXPORTS_VERSION_A
#elif defined(__INTEL_COMPILER) || defined(__GNUC__) || defined(__MINGW32__) \
	|| defined(__clang__)
# define ICEDB_COMPILER_EXPORTS_VERSION_B
#else
# define ICEDB_COMPILER_EXPORTS_VERSION_UNKNOWN
#endif

 // Defaults for static libraries

/**
* \def ICEDB_SHARED_EXPORT
* \brief A tag used to tell the compiler that a symbol should be exported.
**/
/**
* \def ICEDB_SHARED_IMPORT
* \brief A tag used to tell the compiler that a symbol should be imported.
**/
/**
* \def ICEDB_HIDDEN
* \brief A tag used to tell the compiler that a symbol should not be listed,
* but it may be referenced from other code modules.
**/
/**
* \def ICEDB_PRIVATE
* \brief A tag used to tell the compiler that a symbol should not be listed,
* and it may not be referenced from other code modules.
**/

#if defined ICEDB_COMPILER_EXPORTS_VERSION_A
# define ICEDB_SHARED_EXPORT __declspec(dllexport)
# define ICEDB_SHARED_IMPORT __declspec(dllimport)
# define ICEDB_HIDDEN
# define ICEDB_PRIVATE
#elif defined ICEDB_COMPILER_EXPORTS_VERSION_B
# define ICEDB_SHARED_EXPORT __attribute__ ((visibility("default")))
# define ICEDB_SHARED_IMPORT __attribute__ ((visibility("default")))
# define ICEDB_HIDDEN __attribute__ ((visibility("hidden")))
# define ICEDB_PRIVATE __attribute__ ((visibility("internal")))
#else
# pragma message("ICEDB defs.h warning: compiler is unrecognized. Shared libraries may not export their symbols properly.")
# define ICEDB_SHARED_EXPORT
# define ICEDB_SHARED_IMPORT
# define ICEDB_HIDDEN
# define ICEDB_PRIVATE
#endif

/**
* \def ICEDB_DL
* \brief A preprocessor tag that indicates that a symbol is to be exported/imported.
*
* If (libname)_SHARED is defined, then the target library both
* exports and imports. If not defined, then it is a static library.
**/

#if icedb_SHARED
# if icedb_EXPORTING
#  define ICEDB_DL ICEDB_SHARED_EXPORT
# else
#  define ICEDB_DL ICEDB_SHARED_IMPORT
# endif
#else
# define ICEDB_DL
#endif

#if defined(_MSC_FULL_VER)
# pragma warning (disable: 4251) // needs to have dll-interface to be used by clients of class
# define ICEDB_DEPRECATED [[deprecated]]
#else
# define ICEDB_DEPRECATED
#endif

#ifndef GSL_THROW_ON_CONTRACT_VIOLATION
# define GSL_THROW_ON_CONTRACT_VIOLATION
#endif

/** @} **/

/* Thread local storage stuff */
#ifdef _MSC_FULL_VER
#define ICEDB_THREAD_LOCAL __declspec( thread )
#endif
#ifdef __GNUC__
#define ICEDB_THREAD_LOCAL __thread
#endif

// Compiler interface warning suppression
#if defined _MSC_FULL_VER
#pragma warning(push)
#pragma warning( disable : 4003 ) // Bug in boost with VS2016.3
#pragma warning( disable : 4251 ) // DLL interface
#pragma warning( disable : 4275 ) // DLL interface
#endif

// Errata:

#ifndef ICEDB_NO_ERRATA
// Boost bug with C++17 requires this define. See https://stackoverflow.com/questions/41972522/c2143-c2518-when-trying-to-compile-project-using-boost-multiprecision
#ifndef _HAS_AUTO_PTR_ETC
#define _HAS_AUTO_PTR_ETC	(!_HAS_CXX17)
#endif /* _HAS_AUTO_PTR_ETC */
#endif


ICEDB_END_DECL_C

#endif
