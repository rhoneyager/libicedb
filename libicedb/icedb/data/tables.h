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

struct ICEDB_TBL_vtable;
struct ICEDB_TBL_container_vtable;

/** \brief A structure that describes an object table. 
* \todo Make this internal.
**/
struct ICEDB_TBL {
	ICEDB_TBL_vtable *_vptr; ///< Function table, for convenience.
	ICEDB_fs_hnd* obj; ///< The fs object represented by this table. MUST exist / be accessible.
	ICEDB_DATA_TYPES type; ///< The type of data.
	size_t numDims; ///< Number of dimensions of the data.
	size_t *dims; ///< The dimensions of the data.
	size_t reservedSize; ///< The reserved size of the data, in __bytes__. The number of values may be computed by size / sizeof(TYPE).
	const char* name; ///< The name of the table. A NULL-terminated string.
	//void(*_free_fs_attr_p)(ICEDB_ATTR *); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	//void(*_free_fs_attr_pp)(ICEDB_ATTR **); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	//ICEDB_ATTR* next; ///< The next object in the list. End of list is denoted by NULL.
};
//typedef ICEDB_TBL* ICEDB_TBL_p;

/** \brief Create a table
* \param parent is a pointer to the parent object (the object that stores the table's data).
* \param name is the name of the table, expressed as a NULL-terminated C-style string.
* \param type is the type of the table.
* \param numDims is the number of dimensions of the table.
* \param dims is an array, of size numDims, that lists the size of each dimension.
* \returns NULL if an error occurred, otherwise with a new attribute object.
**/
typedef struct ICEDB_TBL* (*ICEDB_TBL_create_f)(
	struct ICEDB_fs_hnd* parent,
	const char* name,
	ICEDB_DATA_TYPES type,
	size_t numDims,
	size_t *dims
);
DL_ICEDB ICEDB_TBL_create_f ICEDB_TBL_create;

/** \brief Open a table
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the table. Must be null-terminated.
* \returns NULL if an error occurred, otherwise with a new copy of the table object.
**/
typedef struct ICEDB_TBL* (*ICEDB_TBL_open_f)(
	struct ICEDB_fs_hnd* parent,
	const char* name
);
DL_ICEDB ICEDB_TBL_open_f ICEDB_TBL_open;

/** \brief Does a table with this name exist?
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the table. Must be null-terminated.
* \param err is an error code
* \returns True if exists, false if nonexistent or if an error occurred.
**/
typedef bool(*ICEDB_TBL_exists_f)(
	const struct ICEDB_fs_hnd* parent,
	const char* name,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
	);
DL_ICEDB ICEDB_TBL_exists_f ICEDB_TBL_exists;

/** \brief Get number of tables
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param err is an error code
* \returns The number of tables, and zero on error. Always check err.
**/
typedef size_t(*ICEDB_TBL_getNumTbls_f)(
	const ICEDB_fs_hnd* parent,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
	);
DL_ICEDB ICEDB_TBL_getNumTbls_f ICEDB_TBL_getNumTbls;

/** \brief Get the name of a table, by index
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param attrnum is the attribute number. Spans [0, numTbls). An error will occur if this is out of range.
* \param inPathSize is used if the output name array is already pre-allocated, in which case it represents the size (in __bytes__) of the array.
If dynamic allocation is instead requested, set this to NULL.
* \param outPathSize is a pointer to the number of bytes in the output array neede to represent the path. If the path is too large to fit into
a staticly-allocated array, then an error code will be emitted.
* \param bufPath is a pointer to the output path array.
* \param deallocator is a function that can deallocate bufPath once it is no longer needed. Use this only when inPathSize != 0.
* \param err is an error code.
* \returns bufPath on success, NULL on error.
**/
typedef const char*(*ICEDB_TBL_getTblName_f)(
	const ICEDB_fs_hnd* p,
	size_t attrnum,
	ICEDB_OPTIONAL size_t inPathSize,
	ICEDB_OUT size_t* outPathSize,
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator);
DL_ICEDB ICEDB_TBL_getTblName_f ICEDB_TBL_getTblName;

/** \brief Renames a table
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param oldname is the table's current name. Must be a NULL-terminated C-string.
* \param newname is the table's new name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True on success, false if an error occured.
**/
typedef bool(*ICEDB_TBL_renameTbl_f)(ICEDB_fs_hnd* p, const char* oldname, const char* newname);
DL_ICEDB ICEDB_TBL_renameTbl_f ICEDB_TBL_renameTbl;

/** \brief Delete a table
*
* Deletion fails if the table does not exist, if the table is opened elsewhere, or if the parent is read-only.
* \param parent is a pointer to the parent object (the object that stores the table's data). Must be non-NULL.
* \param name is the name of the table. Must be null-terminated.
* \returns false if an error occurred, otherwise true.
**/
typedef bool (*ICEDB_TBL_remove_f)(
	ICEDB_fs_hnd* parent,
	const char* name
);
DL_ICEDB ICEDB_TBL_remove_f ICEDB_TBL_remove;

/** \brief Close a table (and free data structures)
* \param tbl is the table. Must be non-NULL.
* \returns false if an error occurred, otherwise true.
**/
typedef bool (*ICEDB_TBL_close_f)(
	ICEDB_TBL* tbl
);
DL_ICEDB ICEDB_TBL_close_f ICEDB_TBL_close;


/** \brief Copy a table
* \param newparent is a pointer to the parent object (the object that stores the table's data).
* \param srctbl is the table that is copied
* \param newname is the name of the copied table. If NULL, then the name is the same.
* \returns NULL if an error occurred, otherwise with a new copy of the attribute object.
**/
typedef ICEDB_TBL* (*ICEDB_TBL_copy_f)(
	ICEDB_OPTIONAL ICEDB_fs_hnd* newparent,
	const ICEDB_TBL* srctbl,
	ICEDB_OPTIONAL const char* newname
);
DL_ICEDB ICEDB_TBL_copy_f ICEDB_TBL_copy;

/** \brief Returns the filesystem handle for the table. Used particularly with attribute operations.
* \param tbl is the table
* \returns NULL on error, otherwise a fs handle (that must be freed by the application)
**/
typedef ICEDB_fs_hnd* (*ICEDB_TBL_getParent_f)(ICEDB_TBL* tbl);
DL_ICEDB ICEDB_TBL_getParent_f ICEDB_TBL_getParent;

/** \brief Gets the type of a table
* \param p is the pointer to a table handle.
* \returns Attribute type on success, NULL (ICEDB_type_invalid) on error.
**/
typedef ICEDB_DATA_TYPES (*ICEDB_TBL_getType_f)(const ICEDB_TBL* p);
DL_ICEDB ICEDB_TBL_getType_f ICEDB_TBL_getType;

/** \brief Gets the number of dimensions for a table.
* \param p is the pointer to a table handle.
* \param err is an error code.
* \returns Number of dimensions. 0 on error. Always check err.
**/
typedef size_t (*ICEDB_TBL_getNumDims_f)(const ICEDB_TBL* p, ICEDB_OUT ICEDB_error_code* err);
DL_ICEDB ICEDB_TBL_getNumDims_f ICEDB_TBL_getNumDims;

/** \brief Gets the dimensions for a table.
* \param p is the pointer to a table handle.
* \param dims is a pointer to an array of type size_t and size numDims, where the dimensions of the table are stored.
* \returns True on success, false on error.
**/
typedef bool (*ICEDB_TBL_getDims_f)(const ICEDB_TBL* p, ICEDB_OUT size_t * dims);
DL_ICEDB ICEDB_TBL_getDims_f ICEDB_TBL_getDims;

/** \brief Resize table
* \param tbl is the pointer to the table
* \param newnumdims is the new number of dimensions
* \param newdims is a pointer to an array containing the new dimensions
* \param conserve indicates whether the data should be conserved on resize or not. May not always be possible.
   If impossible, then the function will return an error and the internal data should be untouched.
* \returns true on success, false on error.
**/
typedef bool (*ICEDB_TBL_resize_f)(ICEDB_TBL* tbl, size_t newnumdims, const size_t *newdims, bool conserve);
DL_ICEDB ICEDB_TBL_resize_f ICEDB_TBL_resize;

// Read values

typedef bool (*ICEDB_TBL_readSingle_f)(
	const ICEDB_TBL* p, 
	const size_t *index, 
	ICEDB_OUT void* out);
DL_ICEDB ICEDB_TBL_readSingle_f ICEDB_TBL_readSingle;

typedef bool (*ICEDB_TBL_readMapped_f)(
	const ICEDB_TBL* p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const ptrdiff_t *imapp,
	ICEDB_OUT void* out);
DL_ICEDB ICEDB_TBL_readMapped_f ICEDB_TBL_readMapped;

typedef bool (*ICEDB_TBL_readArray_f)(
	const ICEDB_TBL* p,
	const size_t *start,
	const size_t *count,
	ICEDB_OUT void* out);
DL_ICEDB ICEDB_TBL_readArray_f ICEDB_TBL_readArray;

typedef bool (*ICEDB_TBL_readStride_f)(
	const ICEDB_TBL* p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	ICEDB_OUT void* out);
DL_ICEDB ICEDB_TBL_readStride_f ICEDB_TBL_readStride;

typedef bool (*ICEDB_TBL_readFull_f)(
	const ICEDB_TBL* p,
	ICEDB_OUT void* out);
DL_ICEDB ICEDB_TBL_readFull_f ICEDB_TBL_readFull;

// Write values

typedef bool (*ICEDB_TBL_writeSingle_f)(
	ICEDB_TBL* p,
	const size_t *index,
	const void* in);
DL_ICEDB ICEDB_TBL_writeSingle_f ICEDB_TBL_writeSingle;

typedef bool (*ICEDB_TBL_writeMapped_f)(
	ICEDB_TBL* p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const ptrdiff_t *imapp,
	const void* in);
DL_ICEDB ICEDB_TBL_writeMapped_f ICEDB_TBL_writeMapped;

typedef bool (*ICEDB_TBL_writeArray_f)(
	ICEDB_TBL* p,
	const size_t *start,
	const size_t *count,
	const void* in);
DL_ICEDB ICEDB_TBL_writeArray_f ICEDB_TBL_writeArray;

typedef bool (*ICEDB_TBL_writeStride_f)(
	ICEDB_TBL* p,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const void* in);
DL_ICEDB ICEDB_TBL_writeStride_f ICEDB_TBL_writeStride;

typedef bool (*ICEDB_TBL_writeFull_f)(
	ICEDB_TBL* p,
	const void* in);
DL_ICEDB ICEDB_TBL_writeFull_f ICEDB_TBL_writeFull;


struct ICEDB_TBL_vtable {
	ICEDB_TBL_close_f close;
	ICEDB_TBL_copy_f copy;
	ICEDB_TBL_getParent_f getParent;
	ICEDB_TBL_getType_f getType;
	ICEDB_TBL_getNumDims_f getNumDims;
	ICEDB_TBL_getDims_f getDims;
	ICEDB_TBL_resize_f resize;
	ICEDB_TBL_readSingle_f readSingle;
	ICEDB_TBL_readMapped_f readMapped;
	ICEDB_TBL_readArray_f readArray;
	ICEDB_TBL_readStride_f readStride;
	ICEDB_TBL_readFull_f readFull;
	ICEDB_TBL_writeSingle_f writeSingle;
	ICEDB_TBL_writeMapped_f writeMapped;
	ICEDB_TBL_writeArray_f writeArray;
	ICEDB_TBL_writeStride_f writeStride;
	ICEDB_TBL_writeFull_f writeFull;
};

/** \brief This acts as a container for all table functions that require a base fs object to act as a container.
*
* To get these functions, see ICEDB_TBL_getContainerFunctions.
* \see ICEDB_TBL_getContainerFunctions
**/
struct ICEDB_TBL_container_vtable {
	ICEDB_TBL_create_f create;
	ICEDB_TBL_open_f open;
	ICEDB_TBL_remove_f remove;
	ICEDB_TBL_getNumTbls_f count;
	ICEDB_TBL_getTblName_f getName;
	ICEDB_TBL_renameTbl_f rename;
	ICEDB_TBL_exists_f exists;

};

DL_ICEDB const ICEDB_TBL_container_vtable* ICEDB_TBL_getContainerFunctions(); ///< Returns a static ICEDB_TBL_container_vtable*. No need to free.


/** @} */ // end of tbls


#endif