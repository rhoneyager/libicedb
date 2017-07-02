#include "../germany/mem.h"
#include <stdlib.h>

ICEDB_BEGIN_DECL_C
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
ICEDB_END_DECL_C
ICEDB_BEGIN_DECL
namespace icedb {
	ICEDB_SYMBOL_SHARED void* _malloc(size_t numBytes) {return ICEDB_malloc(numBytes);}
	ICEDB_SYMBOL_SHARED void _free(void* obj) { return ICEDB_free(obj); }
}
ICEDB_END_DECL