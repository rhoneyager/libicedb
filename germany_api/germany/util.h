#pragma once
#ifndef ICEDB_H_UTIL
#define ICEDB_H_UTIL
#include "defs.h"
ICEDB_BEGIN_DECL_C

DL_ICEDB size_t ICEDB_COMPAT_strncpy_s(char *dest, size_t destSz, const char* src, size_t srcSz);
DL_ICEDB char* ICEDB_COMPAT_strdup_s(const char* src, size_t srcSz);

	/** Safe file stream printf **/
#ifdef ICEDB_DEFS_COMPILER_HAS_FPRINTF_S
#define ICEDB_COMPAT_fprintf_s fprintf_s
#else
#define ICEDB_COMPAT_fprintf_s fprintf
#endif

	/** Save file stream fputs **/
#ifdef ICEDB_DEFS_COMPILER_HAS_FPUTS_S
#define ICEDB_COMPAT_fputs_s fputs
#else
#define ICEDB_COMPAT_fputs_s fputs
#endif

#ifdef ICEDB_DEFS_COMPILER_HAS_STRNLEN_S
#define ICEDB_COMPAT_strnlen_s strnlen_s
#else
#define ICEDB_COMPAT_strnlen_s strnlen
#endif

ICEDB_END_DECL_C
#endif
