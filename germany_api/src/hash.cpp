#include "../germany/defs.h"
#include "../germany/hash.h"
#include "../germany/MurmurHash3.h"
#include <sstream>
#include <string>

ICEDB_BEGIN_DECL_C
ICEDB_HASH_t HASH(const void *key, int len)
{
	ICEDB_HASH_t res;
	MurmurHash3_x64_128(key, len, ICEDB_HASHSEED, &res);
	return res;
}
ICEDB_END_DECL_C