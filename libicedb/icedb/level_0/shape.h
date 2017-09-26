#pragma once
#ifndef ICEDB_H_SHAPE
#define ICEDB_H_SHAPE
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/error/error.h"

/// This is the public header file that defines the shape manipulation operations.

struct ICEDB_L0_SHAPE_VOL_SPARSE;
typedef ICEDB_L0_SHAPE_VOL_SPARSE* ICEDB_L0_SHAPE_VOL_SPARSE_p;

typedef ICEDB_L0_SHAPE_VOL_SPARSE ICEDB_SHAPE;
typedef ICEDB_L0_SHAPE_VOL_SPARSE_p ICEDB_SHAPE_p;




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
