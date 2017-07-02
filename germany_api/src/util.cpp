#define __STDC_WANT_LIB_EXT1__ 1
#include "../germany/util.h"
#include "../germany/mem.h"
#include <string.h>

ICEDB_SYMBOL_SHARED size_t ICEDB_COMPAT_strncpy_s(char * dest, size_t destSz, const char * src, size_t srcSz)
{
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
	char* res = (char*)ICEDB_malloc(sizeof(char)*(srcSz+1));
	ICEDB_COMPAT_strncpy_s(res, srcSz+1, src, srcSz+1);
	return res;
}
