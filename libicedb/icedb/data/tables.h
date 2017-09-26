#pragma once
#ifndef ICEDB_H_TABLES
#define ICEDB_H_TABLES

#include <stdint.h>
#include <stdbool.h>

#include "../defs.h"
#include "../fs/fs.h"

/*
union ICEDB_TBL_DATA {
	int8_t* i8t;
	int16_t* i16t;
	int32_t* i32t;
	int64_t* i64t;
	intmax_t* imaxt;
	intptr_t* iptrt;
	uint8_t* ui8t;
	uint16_t* ui16t;
	uint32_t* ui32t;
	uint64_t* ui64t;
	uintmax_t* uimaxt;
	uintptr_t* uiptrt;
	float* ft;
	double* dt;
	char* ct;
};
*/

/** \brief A structure that describes an object table. **/
struct ICEDB_TBL {
	ICEDB_fs_hnd_p obj; ///< The fs object represented by this table. MUST exist / be accessible.
	ICEDB_DATA_TYPES type; ///< The type of data.
	size_t numDims; ///< Number of dimensions of the data.
	size_t *dims; ///< The dimensions of the data.
	size_t reservedSize; ///< The reserved size of the data, in __bytes__. The number of values may be computed by size / sizeof(TYPE).
	const char* name; ///< The name of the table. A NULL-terminated string.
	//void(*_free_fs_attr_p)(ICEDB_ATTR *); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	//void(*_free_fs_attr_pp)(ICEDB_ATTR **); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	//ICEDB_ATTR* next; ///< The next object in the list. End of list is denoted by NULL.
};



#endif