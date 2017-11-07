#pragma once
#ifndef ICEDB_H_UTIL
#define ICEDB_H_UTIL
#include "../defs.h"
#include "../dlls/linking.h"
#include <string.h>
// Gives errno_t in C11
#include <stddef.h>
#if defined(__cplusplus) || defined(c_plusplus)
#else
#include <uchar.h>
#endif

ICEDB_BEGIN_DECL_C

DL_ICEDB size_t ICEDB_COMPAT_strncpy_s(char *dest, size_t destSz, const char* src, size_t srcSz);
DL_ICEDB char* ICEDB_COMPAT_strdup_s(const char* src, size_t srcSz);

DL_ICEDB size_t ICEDB_COMPAT_wcsncpy_s(wchar_t *dest, size_t destSz, const wchar_t* src, size_t srcSz);
DL_ICEDB wchar_t* ICEDB_COMPAT_wcsdup_s(const wchar_t* src, size_t srcSz);

#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
#else
#define errno_t int
#endif
DL_ICEDB errno_t ICEDB_COMPAT_memcpy_s(void *dest, size_t destSz, const void* src, size_t count);

	/** Safe file stream printf **/
#ifdef ICEDB_DEFS_COMPILER_HAS_FPRINTF_S
#define ICEDB_COMPAT_fprintf_s fprintf_s
#define ICEDB_COMPAT_fwprintf_s fwprintf_s
#else
#define ICEDB_COMPAT_fprintf_s fprintf
#define ICEDB_COMPAT_fwprintf_s fwprintf
#endif

	/** Save file stream fputs **/
#ifdef ICEDB_DEFS_COMPILER_HAS_FPUTS_S
#define ICEDB_COMPAT_fputs_s fputs
#define ICEDB_COMPAT_fputws_s fputws
#else
#define ICEDB_COMPAT_fputs_s fputs
#define ICEDB_COMPAT_fputws_s fputws
#endif

#ifdef ICEDB_DEFS_COMPILER_HAS_STRNLEN_S
#define ICEDB_COMPAT_wcsnlen_s wcsnlen_s
#define ICEDB_COMPAT_strnlen_s strnlen_s
#else
#define ICEDB_COMPAT_wcsnlen_s wcsnlen_s
#define ICEDB_COMPAT_strnlen_s strnlen
#endif

ICEDB_END_DECL_C

#endif
