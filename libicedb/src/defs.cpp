#include "../icedb/defs.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <Windows.h>
#endif

ICEDB_SYMBOL_SHARED void ICEDB_DEBUG_RAISE_EXCEPTION_HANDLER(const char* file, int line, const char* fsig)
{
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
	fprintf_s(stderr, "Exception raised in file %s, line %d, function %s.\n", file, line, fsig);
#else
	fprintf(stderr, "Exception raised in file %s, line %d, function %s.\n", file, line, fsig);
#endif
	
#ifdef _WIN32
	DebugBreak();
#else
	exit(999);
#endif
}
