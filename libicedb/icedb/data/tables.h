#pragma once
#ifndef ICEDB_H_TABLES
#define ICEDB_H_TABLES

#include <stdint.h>
#include <stdbool.h>

#include "../defs.h"
#include "../fs/fs.h"

/** @defgroup tbls Tables
*
* @{
**/

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
typedef ICEDB_TBL* ICEDB_TBL_p;

/** \brief Create a table
* \param parent is a pointer to the parent object (the object that stores the table's data).
* \param name is the name of the table, expressed as a NULL-terminated C-style string.
* \param type is the type of the table.
* \param numDims is the number of dimensions of the table.
* \param dims is an array, of size numDims, that lists the size of each dimension.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new attribute object.
**/
DL_ICEDB ICEDB_TBL_p ICEDB_TBL_create(
	ICEDB_fs_hnd_p parent,
	const char* name,
	ICEDB_DATA_TYPES type,
	size_t numDims,
	size_t *dims,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
);

/** \brief Open a table
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the table. Must be null-terminated.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new copy of the table object.
**/
DL_ICEDB ICEDB_TBL_p ICEDB_TBL_open(
	ICEDB_fs_hnd_p parent,
	const char* name,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
);

/** \brief Delete a table
*
* Deletion fails if the table does not exist, if the table is opened elsewhere, or if the parent is read-only.
* \param parent is a pointer to the parent object (the object that stores the table's data). Must be non-NULL.
* \param name is the name of the table. Must be null-terminated.
* \param err is an error code
* \returns false if an error occurred, otherwise true.
**/
DL_ICEDB bool ICEDB_TBL_delete(
	ICEDB_fs_hnd_p parent,
	const char* name,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
);

/** \brief Close a table (and free data structures)
* \param tbl is the table. Must be non-NULL.
* \param err is an error code
* \returns false if an error occurred, otherwise true.
**/
DL_ICEDB bool ICEDB_TBL_close(
	ICEDB_TBL_p tbl,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
);


/** \brief Copy a table
* \param newparent is a pointer to the parent object (the object that stores the table's data).
* \param tbl is the table that is copied
* \param newname is the name of the copied table. If NULL, then the name is the same.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new copy of the attribute object.
**/
DL_ICEDB ICEDB_TBL_p ICEDB_TBL_copy(
	ICEDB_OPTIONAL ICEDB_fs_hnd_p newparent,
	ICEDB_TBL_p tbl,
	ICEDB_OPTIONAL const char* newname,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
);

/** \brief Returns the filesystem handle for the table. Used particularly with attribute operations.
* \param tbl is the table
* \param err is an error code on failure.
* \returns NULL on error, otherwise a fs handle (that must be freed by the application)
**/
DL_ICEDB ICEDB_fs_hnd_p ICEDB_TBL_getHandle(ICEDB_TBL_p tbl, ICEDB_OUT ICEDB_error_code* err);

/** \brief Gets the type of a table
* \param p is the pointer to a table handle.
* \param err is an error code.
* \returns Attribute type on success, NULL (ICEDB_type_invalid) on error.
**/
DL_ICEDB ICEDB_DATA_TYPES ICEDB_TBL_getType(const ICEDB_TBL_p p, ICEDB_OUT ICEDB_error_code* err);

/** \brief Gets the number of dimensions for a table.
* \param p is the pointer to a table handle.
* \param err is an error code.
* \returns Number of dimensions. 0 on error.
**/
DL_ICEDB size_t ICEDB_TBL_getNumDims(const ICEDB_TBL_p p, ICEDB_OUT ICEDB_error_code* err);

/** \brief Gets the dimensions for a table.
* \param p is the pointer to a table handle.
* \param dims is a pointer to an array of type size_t and size numDims, where the dimensions of the table are stored.
* \param err is an error code.
* \returns True on success, false on error.
**/
DL_ICEDB bool ICEDB_TBL_getDims(const ICEDB_TBL_p p, ICEDB_OUT size_t * dims, ICEDB_OUT ICEDB_error_code* err);

/** \brief Resize table
* \param p is the pointer to the table
* \param newnumdims is the new number of dimensions
* \param newdims is a pointer to an array containing the new dimensions
* \param conserve indicates whether the data should be conserved on resize or not. May not always be possible.
   If impossible, then the function will return an error and the internal data should be untouched.
* \param err is an error code.
* \returns true on success, false on error.
**/
DL_ICEDB bool ICEDB_TBL_resize(ICEDB_TBL_p p, size_t newnumdims, const size_t *newdims, bool conserve, ICEDB_OUT ICEDB_error_code* err);

// Read values

DL_ICEDB bool ICEDB_TBL_readSingle(
	const ICEDB_TBL_p p, 
	const size_t *index, 
	ICEDB_OUT void* out, 
	ICEDB_OUT ICEDB_error_code* err);

DL_ICEDB bool ICEDB_TBL_readMapped(
	const ICEDB_TBL_p p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const ptrdiff_t *imapp,
	ICEDB_OUT void* out,
	ICEDB_OUT ICEDB_error_code* err);

DL_ICEDB bool ICEDB_TBL_readArray(
	const ICEDB_TBL_p p,
	const size_t *start,
	const size_t *count,
	ICEDB_OUT void* out,
	ICEDB_OUT ICEDB_error_code* err);

DL_ICEDB bool ICEDB_TBL_readStride(
	const ICEDB_TBL_p p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	ICEDB_OUT void* out,
	ICEDB_OUT ICEDB_error_code* err);

DL_ICEDB bool ICEDB_TBL_readFull(
	const ICEDB_TBL_p p,
	ICEDB_OUT void* out,
	ICEDB_OUT ICEDB_error_code* err);

// Write values

DL_ICEDB bool ICEDB_TBL_writeSingle(
	ICEDB_TBL_p p,
	const size_t *index,
	const void* in,
	ICEDB_OUT ICEDB_error_code* err);

DL_ICEDB bool ICEDB_TBL_writeMapped(
	ICEDB_TBL_p p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const ptrdiff_t *imapp,
	const void* in,
	ICEDB_OUT ICEDB_error_code* err);

DL_ICEDB bool ICEDB_TBL_writeArray(
	ICEDB_TBL_p p,
	const size_t *start,
	const size_t *count,
	const void* in,
	ICEDB_OUT ICEDB_error_code* err);

DL_ICEDB bool ICEDB_TBL_writeStride(
	ICEDB_TBL_p p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const void* in,
	ICEDB_OUT ICEDB_error_code* err);

DL_ICEDB bool ICEDB_TBL_writeFull(
	ICEDB_TBL_p p,
	const void* in,
	ICEDB_OUT ICEDB_error_code* err);

/** @} */ // end of tbls


#endif