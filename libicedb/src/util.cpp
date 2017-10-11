//#define __STDC_WANT_LIB_EXT1__ 1 // now defined in defs.h
#include "../icedb/defs.h"
#include "../icedb/misc/util.h"
#include "../icedb/misc/mem.h"
#include <string.h>
#include <wchar.h>

ICEDB_SYMBOL_SHARED errno_t ICEDB_COMPAT_memcpy_s(
	void * dest, size_t destSz,
	const void * src, size_t srcSz)
{
	/** \brief Safe memory array copy.
	\returns the number of bytes actually written.
	\param dest is the pointer to the destination. Always null terminated.
	\param destSz is the size of the destination buffer.
	\param src is the pointer to the source. Bytes from src are copied
	until srcSz. 
	\param srcSz is the max size of the source buffer. 
	**/
	if (!dest || !src)ICEDB_DEBUG_RAISE_EXCEPTION();
#if ICEDB_USING_SECURE_STRINGS
	return memcpy_s(dest, destSz, src, srcSz);
#else
	if (srcSz <= destSz) {
		memcpy(dest, src, srcSz);
	} else {
		memcpy(dest, src, destSz);
	}
#endif
	return 0;
}

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
	} else {
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
	char* res = (char*)ICEDB_malloc(sizeof(char)*(srcSz+1));
	ICEDB_COMPAT_strncpy_s(res, srcSz+1, src, srcSz+1);
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

