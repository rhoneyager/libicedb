#include "mem.h"
#include <stdlib.h>

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
