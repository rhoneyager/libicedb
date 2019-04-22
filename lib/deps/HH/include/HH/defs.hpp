#pragma once
#ifndef HH_H_DEFS
# define HH_H_DEFS

# ifndef __STDC_WANT_LIB_EXT1__
#  define __STDC_WANT_LIB_EXT1__ 1
# endif
# include <cstddef>

# ifdef _MSC_FULL_VER
#  define HH_DEBUG_FSIG __FUNCSIG__
# endif
# if defined(__GNUC__) || defined(__clang__)
#  if defined(__PRETTY_FUNCTION__)
#   define HH_DEBUG_FSIG __PRETTY_FUNCTION__
#  elif defined(__func__)
#   define HH_DEBUG_FSIG __func__
#  elif defined(__FUNCTION__)
#   define HH_DEBUG_FSIG __FUNCTION__
#  else
#   define HH_DEBUG_FSIG ""
#  endif
# endif

/* Global error codes. */
# if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
#  define HH_USING_SECURE_STRINGS 1
#  define HH_DEFS_COMPILER_HAS_FPRINTF_S
#  define HH_DEFS_COMPILER_HAS_FPUTS_S
# else
#  define _CRT_SECURE_NO_WARNINGS
# endif

// Compiler interface warning suppression
# if defined _MSC_FULL_VER
#  include <CppCoreCheck/Warnings.h>
#  pragma warning(disable: CPPCORECHECK_DECLARATION_WARNINGS)
#  pragma warning(push)
#  pragma warning( disable : 4003 ) // Bug in boost with VS2016.3
#  pragma warning( disable : 4251 ) // DLL interface
#  pragma warning( disable : 4275 ) // DLL interface
// Avoid unnamed objects. Buggy in VS / does not respect attributes telling the compiler to ignore the check.
#  pragma warning( disable : 26444 ) 
# endif

/// Pointer to an object that is modfied by a function
# define HH_OUT
/// Denotes an 'optional' parameter (one which can be replaced with a NULL or nullptr)
# define HH_OPTIONAL

// Errata:

# ifndef HH_NO_ERRATA
// Boost bug with C++17 requires this define. See https://stackoverflow.com/questions/41972522/c2143-c2518-when-trying-to-compile-project-using-boost-multiprecision
#  ifndef _HAS_AUTO_PTR_ETC
#   define _HAS_AUTO_PTR_ETC	(!_HAS_CXX17)
#  endif /* _HAS_AUTO_PTR_ETC */
# endif

// C++ language standard __cplusplus
# if defined _MSC_FULL_VER
#  if(_MSVC_LANG  >= 201703L )
#   define HH_CPP17
#  elif(_MSVC_LANG >= 201402L)
#   define HH_CPP14
#  endif
# else
#  if(__cplusplus >= 201703L )
#   define HH_CPP17
#  elif(__cplusplus >= 201402L)
#   define HH_CPP14
#  endif
# endif

// Use the [[nodiscard]] attribute
# ifdef HH_CPP17
#  define HH_NODISCARD [[nodiscard]]
# else
#  ifdef _MSC_FULL_VER
#   define HH_NODISCARD _Check_return_
#  else
#   define HH_NODISCARD __attribute__ ((warn_unused_result))
#  endif
# endif

// Use the [[maybe_unused]] attribute
# ifdef HH_CPP17
#  ifdef _MSC_FULL_VER
#   define HH_MAYBE_UNUSED [[maybe_unused]] [[gsl::suppress(es.84)]] [[gsl::suppress(expr.84)]] [[gsl::suppress(26444)]]
#  else
#   define HH_MAYBE_UNUSED [[maybe_unused]]
#  endif
# else
#  ifdef _MSC_FULL_VER
#   define HH_MAYBE_UNUSED [[gsl::suppress(es.84)]]
#  else
#   define HH_MAYBE_UNUSED __attribute__((unused))
#  endif
# endif

// Error inheritance
# ifndef HH_ERROR_INHERITS_FROM
#  define HH_ERROR_INHERITS_FROM std::exception
# endif

// Closing include guard
#endif
