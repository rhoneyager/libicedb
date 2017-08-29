#pragma once
#ifndef ICEDB_H_SHAPE
#define ICEDB_H_SHAPE
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/error/error.h"
#include <stdint.h>
#include <stdbool.h>

/// This is the public header file that defines the shape manipulation operations.

struct ICEDB_L0_SHAPE_VOL_SPARSE;
typedef ICEDB_L0_SHAPE_VOL_SPARSE* ICEDB_L0_SHAPE_VOL_SPARSE_p;

typedef ICEDB_L0_SHAPE_VOL_SPARSE ICEDB_SHAPE;
typedef ICEDB_L0_SHAPE_VOL_SPARSE_p ICEDB_SHAPE_p;


union ICEDB_ATTR_DATA {
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
enum ICEDB_ATTR_TYPES {
	ICEDB_TYPE_CHAR, // NC_CHAR
	ICEDB_TYPE_INT8, // NC_BYTE
	ICEDB_TYPE_UINT8, // NC_UBYTE
	ICEDB_TYPE_UINT16, // NC_USHORT
	ICEDB_TYPE_INT16, // NC_SHORT
	ICEDB_TYPE_UINT32, // NC_UINT
	ICEDB_TYPE_INT32, // NC_INT (or NC_LONG)
	ICEDB_TYPE_UINT64, // NC_UINT64
	ICEDB_TYPE_INT64, // NC_INT64
	ICEDB_TYPE_FLOAT, // NC_FLOAT
	ICEDB_TYPE_DOUBLE, // NC_DOUBLE
	// These have no corresponding NetCDF type. They never get saved by themselves, but contain pointers to things like string arrays, which are NetCDF objects.
	ICEDB_TYPE_INTMAX,
	ICEDB_TYPE_INTPTR,
	ICEDB_TYPE_UINTMAX,
	ICEDB_TYPE_UINTPTR
};
struct ICEDB_ATTR {
	ICEDB_ATTR_DATA data;
	ICEDB_ATTR_TYPES type;
	size_t size;
	bool hasSize; // if false, then the array is null-terminated. Most useful for character strings.
};

//typedef ICEDB_error_code(*ICEDB_SHAPE_read_f)(ICEDB_SHAPE_p, ICEDB_io_object_handle_p);
//typedef ICEDB_error_code(*ICEDB_SHAPE_write_f)(const ICEDB_SHAPE_p, ICEDB_io_object_handle_p);
typedef size_t(*ICEDB_SHAPE_getSize_f)(const ICEDB_SHAPE_p);
typedef void(*ICEDB_SHAPE_setDescription_f)(ICEDB_SHAPE_p, const char*);
typedef const char*(*ICEDB_SHAPE_getDescription_f)(const ICEDB_SHAPE_p);
typedef float*(*ICEDB_SHAPE_getScattElemCoords_f)(const ICEDB_SHAPE_p);
typedef bool(*ICEDB_SHAPE_tableExists_f)(const ICEDB_SHAPE_p, const char*);
typedef ICEDB_ATTR_TYPES(*ICEDB_SHAPE_getTableType_f)(const ICEDB_SHAPE_p, const char*);

// Args are: ptr to shape, number of table dimensions (should be 2), size of each dimension, pointer to the data.
// Returns: a bool indicating success if true
typedef bool(*ICEDB_SHAPE_getFloatTable_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, float* const*);
typedef bool(*ICEDB_SHAPE_getDoubleTable_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, double**);
typedef bool(*ICEDB_SHAPE_getUInt8Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, uint8_t**);
typedef bool(*ICEDB_SHAPE_getInt8Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, int8_t**);
typedef bool(*ICEDB_SHAPE_getUInt16Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, uint16_t**);
typedef bool(*ICEDB_SHAPE_getInt16Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, int16_t**);
typedef bool(*ICEDB_SHAPE_getUInt32Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, uint32_t**);
typedef bool(*ICEDB_SHAPE_getInt32Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, int32_t**);
typedef bool(*ICEDB_SHAPE_getUInt64Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, uint64_t**);
typedef bool(*ICEDB_SHAPE_getInt64Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, int64_t**);
typedef bool(*ICEDB_SHAPE_getCharTable_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, char**);

struct ICEDB_L0_SHAPE_VOL_SPARSE_vtable {
	//ICEDB_SHAPE_read_f read;
	//ICEDB_SHAPE_write_f write;
	ICEDB_SHAPE_getSize_f getNumPoints;
	ICEDB_SHAPE_setDescription_f setDescription;
	ICEDB_SHAPE_getDescription_f getDescription;
	ICEDB_SHAPE_getScattElemCoords_f getScattElemCoords;

};

struct ICEDB_L0_SHAPE_VOL_SPARSE {
	void *_p;
	struct ICEDB_L0_SHAPE_VOL_SPARSE_vtable *_vptrs;
};

DL_ICEDB ICEDB_SHAPE_p ICEDB_SHAPE_generate();
DL_ICEDB void ICEDB_SHAPE_destroy(ICEDB_SHAPE_p);


#endif
