#define __STDC_WANT_LIB_EXT1__ 1
#include "../icedb/util.h"
#include "../icedb/util.hpp"
#include <cstring>
#include <cwchar>
#if defined(_WIN32)
#include <Windows.h>
#else
#include <signal.h>
#endif

ICEDB_BEGIN_DECL_C

ICEDB_SYMBOL_SHARED size_t ICEDB_COMPAT_strncpy_s(
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
	if (!dest || !src)ICEDB_DEBUG_RAISE_EXCEPTION();
#if ICEDB_USING_SECURE_STRINGS
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

ICEDB_SYMBOL_SHARED char * ICEDB_COMPAT_strdup_s(const char * src, size_t srcSz)
{
	/** \brief Safe char array initialization and copy. Null appended at end (added to srcSz). **/
	if (!src) ICEDB_DEBUG_RAISE_EXCEPTION();
	char* res = (char*)ICEDB_malloc(sizeof(char)*(srcSz + 1));
	ICEDB_COMPAT_strncpy_s(res, srcSz + 1, src, srcSz + 1);
	return res;
}


ICEDB_SYMBOL_SHARED size_t ICEDB_COMPAT_wcsncpy_s(
	wchar_t * dest, size_t destSz,
	const wchar_t * src, size_t srcSz)
{
	/** \brief Safe char array copy.
	\returns the number of characters actually written.
	\param dest is the pointer to the destination. Always null terminated.
	\param destSz is the size of the destination buller, including the trailing null character.
	\param src is the pointer to the source. Characters from src are copied either until the
	first null character or until srcSz. Note that null termination comes later.
	\param srcSz is the max size of the source buffer.
	**/
	if (!dest || !src)ICEDB_DEBUG_RAISE_EXCEPTION();
#if ICEDB_USING_SECURE_STRINGS
	wcsncpy_s(dest, destSz, src, srcSz);
#else
	if (srcSz <= destSz) {
		wcsncpy(dest, src, srcSz);
	}
	else {
		wcsncpy(dest, src, destSz);
	}
#endif
	dest[destSz - 1] = 0;
	for (size_t i = 0; i < destSz; ++i) if (dest[i] == '\0') return i;
	return 0; // Should never be reached
}

ICEDB_SYMBOL_SHARED wchar_t * ICEDB_COMPAT_wcsdup_s(const wchar_t * src, size_t srcSz)
{
	/** \brief Safe char array initialization and copy. Null appended at end (added to srcSz). **/
	if (!src) ICEDB_DEBUG_RAISE_EXCEPTION();
	wchar_t* res = (wchar_t*)ICEDB_malloc(sizeof(wchar_t)*(srcSz + 1));
	ICEDB_COMPAT_wcsncpy_s(res, srcSz + 1, src, srcSz + 1);
	return res;
}


ICEDB_SYMBOL_SHARED void * ICEDB_malloc(size_t numBytes)
{
	void* res = malloc(numBytes);
	if (!res) ICEDB_DEBUG_RAISE_EXCEPTION();
	return res;
}

ICEDB_SYMBOL_SHARED void ICEDB_free(void * obj)
{
	if (!obj) ICEDB_DEBUG_RAISE_EXCEPTION();
	free(obj);
}
ICEDB_SYMBOL_SHARED void ICEDB_DEBUG_RAISE_EXCEPTION_HANDLER_WC(const wchar_t* file, int line, const wchar_t* fsig)
{
	ICEDB_COMPAT_fwprintf_s(stderr, L"Exception raised in file %s, line %d, function %s.\n", file, line, fsig);

#ifdef _WIN32
	DebugBreak();
#else
	raise(SIGTRAP);
	exit(999);
#endif
}
ICEDB_SYMBOL_SHARED void ICEDB_DEBUG_RAISE_EXCEPTION_HANDLER_A(const char* file, int line, const char* fsig)
{
	ICEDB_COMPAT_fprintf_s(stderr, "Exception raised in file %s, line %d, function %s.\n", file, line, fsig);

#ifdef _WIN32
	DebugBreak();
#else
	raise(SIGTRAP);
	exit(999);
#endif
}
ICEDB_END_DECL_C


ICEDB_BEGIN_DECL
namespace icedb {
	ICEDB_SYMBOL_SHARED void* _malloc(size_t numBytes) { return ICEDB_malloc(numBytes); }
	ICEDB_SYMBOL_SHARED void _free(void* obj) { return ICEDB_free(obj); }
}
ICEDB_END_DECL