#include "../icedb/defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#if defined(_WIN32)
#include <Windows.h>
#else
#include <signal.h>
#endif

ICEDB_SYMBOL_SHARED void ICEDB_DEBUG_RAISE_EXCEPTION_HANDLER_WC(const wchar_t* file, int line, const wchar_t* fsig)
{
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
	fwprintf_s(stderr, L"Exception raised in file %s, line %d, function %s.\n", file, line, fsig);
#else
	fwprintf(stderr, L"Exception raised in file %s, line %d, function %s.\n", file, line, fsig);
#endif
	
#ifdef _WIN32
	DebugBreak();
#else
    raise(SIGTRAP);
	exit(999);
#endif
}
ICEDB_SYMBOL_SHARED void ICEDB_DEBUG_RAISE_EXCEPTION_HANDLER_A(const char* file, int line, const char* fsig)
{
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
	fprintf_s(stderr, "Exception raised in file %s, line %d, function %s.\n", file, line, fsig);
#else
	fprintf(stderr, "Exception raised in file %s, line %d, function %s.\n", file, line, fsig);
#endif

#ifdef _WIN32
	DebugBreak();
#else
    raise(SIGTRAP);
	exit(999);
#endif
}