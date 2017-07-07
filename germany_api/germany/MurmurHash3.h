//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_
#include "defs.h"

ICEDB_BEGIN_DECL_C

	//-----------------------------------------------------------------------------
	// Platform-specific functions and macros

	// Microsoft Visual Studio

#if defined(_MSC_VER)
#include <stdint.h>
	//#include <cstdint>
	//typedef unsigned char uint8_t;
	//typedef unsigned long uint32_t;
	//typedef unsigned __int64 uint64_t;

	// Other compilers

#else   // defined(_MSC_VER)

#include <stdint.h>

#endif // !defined(_MSC_VER)

	//-----------------------------------------------------------------------------

	DL_ICEDB void MurmurHash3_x86_32(const void * key, int len, uint32_t seed, void * out);

	DL_ICEDB void MurmurHash3_x86_128(const void * key, int len, uint32_t seed, void * out);

	DL_ICEDB void MurmurHash3_x64_128(const void * key, int len, uint32_t seed, void * out);

	//-----------------------------------------------------------------------------
	ICEDB_END_DECL_C

#endif // _MURMURHASH3_H_
