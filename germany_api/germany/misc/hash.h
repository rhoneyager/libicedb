#pragma once
#include "../defs.h"
#include <cstdint>
#include <string>

ICEDB_BEGIN_DECL_C
//typedef uint64_t ICEDB_HASH_t;
struct ICEDB_HASH_t {
	uint64_t low, high;
};
#define ICEDB_HASHSEED 2487211

/// Wrapper function that calculates the hash of an object (key) with length (len).
DL_ICEDB ICEDB_HASH_t HASH(const void *key, int len);

ICEDB_END_DECL_C
