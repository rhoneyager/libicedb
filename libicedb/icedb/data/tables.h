#pragma once
#ifndef ICEDB_H_TABLES
#define ICEDB_H_TABLES

#include <stdint.h>
#include <stdbool.h>

#include "../defs.h"
#include "../fs/fs.h"

ICEDB_BEGIN_DECL_C

/** @defgroup tbls Tables
*
* @{
**/

struct ICEDB_tbl_ftable;

/** \brief A structure that describes an object table. 
* \todo Make this internal.
**/
struct ICEDB_tbl {
	const ICEDB_tbl_ftable *funcs; ///< Function table, for convenience.
	ICEDB_fs_hnd* self; ///< The fs object represented by this table. Used for getting table attributes. MUST exist / be accessible.
	ICEDB_fs_hnd* parent; ///< The fs object of the container of the table.
	ICEDB_DATA_TYPES type; ///< The type of data.
	size_t numDims; ///< Number of dimensions of the data.
	size_t *dims; ///< The dimensions of the data.
	size_t reservedSize; ///< The reserved size of the data, in __bytes__. The number of values may be computed by size / sizeof(TYPE).
	const char* name; ///< The name of the table. A NULL-terminated string.
	//void(*_free_fs_attr_p)(ICEDB_attr *); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	//void(*_free_fs_attr_pp)(ICEDB_attr **); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	//ICEDB_attr* next; ///< The next object in the list. End of list is denoted by NULL.
};



/** \brief Copy a table
* \param newparent is a pointer to the parent object (the object that stores the table's data).
* \param srctbl is the table that is copied
* \param newname is the name of the copied table. If NULL, then the name is the same.
* \returns NULL if an error occurred, otherwise with a new copy of the attribute object.
**/
typedef ICEDB_tbl* (*ICEDB_tbl_copy_f)(
	const ICEDB_tbl* srctbl,
	ICEDB_fs_hnd* newparent,
	ICEDB_OPTIONAL const char* newname
);
extern DL_ICEDB ICEDB_tbl_copy_f ICEDB_tbl_copy;

/** \brief Returns the filesystem handle for the table. Used particularly with attribute operations.
* \param tbl is the table
* \returns NULL on error, otherwise a fs handle (that must be freed by the application)
**/
//typedef ICEDB_fs_hnd* (*ICEDB_tbl_getSelfFS_f)(ICEDB_tbl* tbl);
//extern DL_ICEDB ICEDB_tbl_getSelfFS_f ICEDB_tbl_getSelfFS;

/** \brief Returns the filesystem handle for the table's parent. Used particularly with attribute operations.
* \param tbl is the table
* \returns NULL on error, otherwise a fs handle (that must be freed by the application)
**/
//typedef ICEDB_fs_hnd* (*ICEDB_tbl_getParent_f)(ICEDB_tbl* tbl);
//extern DL_ICEDB ICEDB_tbl_getParent_f ICEDB_tbl_getParent;

/** \brief Gets the type of a table
* \param p is the pointer to a table handle.
* \returns Attribute type on success, NULL (ICEDB_type_invalid) on error.
**/
//typedef ICEDB_DATA_TYPES (*ICEDB_tbl_getType_f)(const ICEDB_tbl* p);
//extern DL_ICEDB ICEDB_tbl_getType_f ICEDB_tbl_getType;

/** \brief Gets the number of dimensions for a table.
* \param p is the pointer to a table handle.
* \param err is an error code.
* \returns Number of dimensions. 0 on error. Always check err.
**/
//typedef size_t (*ICEDB_tbl_getNumDims_f)(const ICEDB_tbl* p, ICEDB_OUT ICEDB_error_code* err);
//extern DL_ICEDB ICEDB_tbl_getNumDims_f ICEDB_tbl_getNumDims;

/** \brief Gets the dimensions for a table.
* \param p is the pointer to a table handle.
* \param dims is a pointer to an array of type size_t and size numDims, where the dimensions of the table are stored.
* \returns True on success, false on error.
**/
//typedef bool (*ICEDB_tbl_getDims_f)(const ICEDB_tbl* p, ICEDB_OUT size_t * dims);
//extern DL_ICEDB ICEDB_tbl_getDims_f ICEDB_tbl_getDims;

/** \brief Resize table
* \param tbl is the pointer to the table
* \param newnumdims is the new number of dimensions
* \param newdims is a pointer to an array containing the new dimensions
* \param conserve indicates whether the data should be conserved on resize or not. May not always be possible.
   If impossible, then the function will return an error and the internal data should be untouched.
* \returns true on success, false on error.
**/
//typedef bool (*ICEDB_tbl_resize_f)(ICEDB_tbl* tbl, size_t newnumdims, const size_t *newdims);
//extern DL_ICEDB ICEDB_tbl_resize_f ICEDB_tbl_resize;

// Read values

typedef bool (*ICEDB_tbl_readSingle_f)(
	const ICEDB_tbl* p, 
	const size_t *index, 
	ICEDB_OUT void* out);
extern DL_ICEDB ICEDB_tbl_readSingle_f ICEDB_tbl_readSingle;

typedef bool (*ICEDB_tbl_readMapped_f)(
	const ICEDB_tbl* p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const ptrdiff_t *imapp,
	ICEDB_OUT void* out);
extern DL_ICEDB ICEDB_tbl_readMapped_f ICEDB_tbl_readMapped;

typedef bool (*ICEDB_tbl_readArray_f)(
	const ICEDB_tbl* p,
	const size_t *start,
	const size_t *count,
	ICEDB_OUT void* out);
extern DL_ICEDB ICEDB_tbl_readArray_f ICEDB_tbl_readArray;

typedef bool (*ICEDB_tbl_readStride_f)(
	const ICEDB_tbl* p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	ICEDB_OUT void* out);
extern DL_ICEDB ICEDB_tbl_readStride_f ICEDB_tbl_readStride;

typedef bool (*ICEDB_tbl_readFull_f)(
	const ICEDB_tbl* p,
	ICEDB_OUT void* out);
extern DL_ICEDB ICEDB_tbl_readFull_f ICEDB_tbl_readFull;

// Write values

typedef bool (*ICEDB_tbl_writeSingle_f)(
	ICEDB_tbl* p,
	const size_t *index,
	const void* in);
extern DL_ICEDB ICEDB_tbl_writeSingle_f ICEDB_tbl_writeSingle;

typedef bool (*ICEDB_tbl_writeMapped_f)(
	ICEDB_tbl* p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const ptrdiff_t *imapp,
	const void* in);
extern DL_ICEDB ICEDB_tbl_writeMapped_f ICEDB_tbl_writeMapped;

typedef bool (*ICEDB_tbl_writeArray_f)(
	ICEDB_tbl* p,
	const size_t *start,
	const size_t *count,
	const void* in);
extern DL_ICEDB ICEDB_tbl_writeArray_f ICEDB_tbl_writeArray;

typedef bool (*ICEDB_tbl_writeStride_f)(
	ICEDB_tbl* p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const void* in);
extern DL_ICEDB ICEDB_tbl_writeStride_f ICEDB_tbl_writeStride;

typedef bool (*ICEDB_tbl_writeFull_f)(
	ICEDB_tbl* p,
	const void* in);
extern DL_ICEDB ICEDB_tbl_writeFull_f ICEDB_tbl_writeFull;


struct ICEDB_tbl_ftable {
	ICEDB_tbl_close_f close;
	ICEDB_tbl_copy_f copy;
	//ICEDB_tbl_getParent_f getParent;
	//ICEDB_tbl_getType_f getType;
	//ICEDB_tbl_getNumDims_f getNumDims;
	//ICEDB_tbl_getDims_f getDims;
	//ICEDB_tbl_resize_f resize;
	// These are handled by the underlying fs plugin
	ICEDB_tbl_readSingle_f readSingle;
	ICEDB_tbl_readMapped_f readMapped;
	ICEDB_tbl_readArray_f readArray;
	ICEDB_tbl_readStride_f readStride;
	ICEDB_tbl_readFull_f readFull;
	ICEDB_tbl_writeSingle_f writeSingle;
	ICEDB_tbl_writeMapped_f writeMapped;
	ICEDB_tbl_writeArray_f writeArray;
	ICEDB_tbl_writeStride_f writeStride;
	ICEDB_tbl_writeFull_f writeFull;
};

extern DL_ICEDB const struct ICEDB_tbl_ftable ICEDB_funcs_tbl_obj;


/** @} */ // end of tbls
ICEDB_END_DECL_C

#endif