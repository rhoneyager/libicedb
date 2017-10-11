#pragma once
#ifndef ICEDB_H_FS_TBL_BASE
#define ICEDB_H_FS_TBL_BASE


/** \brief Create a table
* \param parent is a pointer to the parent object (the object that stores the table's data).
* \param name is the name of the table, expressed as a NULL-terminated C-style string.
* \param type is the type of the table.
* \param numDims is the number of dimensions of the table.
* \param dims is an array, of size numDims, that lists the size of each dimension.
* \returns NULL if an error occurred, otherwise with a new attribute object.
**/
typedef struct ICEDB_tbl* (*ICEDB_tbl_create_f)(
	struct ICEDB_fs_hnd* parent,
	const char* name,
	ICEDB_DATA_TYPES type,
	size_t numDims,
	const size_t *dims
	);
extern DL_ICEDB ICEDB_tbl_create_f ICEDB_tbl_create;

/** \brief Open a table
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the table. Must be null-terminated.
* \returns NULL if an error occurred, otherwise with a new copy of the table object.
**/
typedef struct ICEDB_tbl* (*ICEDB_tbl_open_f)(
	struct ICEDB_fs_hnd* parent,
	const char* name
	);
extern DL_ICEDB ICEDB_tbl_open_f ICEDB_tbl_open;

/** \brief Does a table with this name exist?
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the table. Must be null-terminated.
* \param err is an error code
* \returns True if exists, false if nonexistent or if an error occurred.
**/
typedef bool(*ICEDB_tbl_exists_f)(
	const struct ICEDB_fs_hnd* parent,
	const char* name,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
	);
extern DL_ICEDB ICEDB_tbl_exists_f ICEDB_tbl_exists;

/** \brief Get number of tables
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param err is an error code
* \returns The number of tables, and zero on error. Always check err.
**/
typedef size_t(*ICEDB_tbl_getNumTbls_f)(
	const ICEDB_fs_hnd* parent,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
	);
extern DL_ICEDB ICEDB_tbl_getNumTbls_f ICEDB_tbl_getNumTbls;

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
typedef const char*(*ICEDB_tbl_getTblName_f)(
	const ICEDB_fs_hnd* p,
	size_t tblnum,
	ICEDB_OPTIONAL size_t inPathSize,
	ICEDB_OUT size_t* outPathSize,
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator);
extern DL_ICEDB ICEDB_tbl_getTblName_f ICEDB_tbl_getTblName;

/** \brief Renames a table
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param oldname is the table's current name. Must be a NULL-terminated C-string.
* \param newname is the table's new name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True on success, false if an error occured.
**/
typedef bool(*ICEDB_tbl_renameTbl_f)(ICEDB_fs_hnd* p, const char* oldname, const char* newname);
extern DL_ICEDB ICEDB_tbl_renameTbl_f ICEDB_tbl_renameTbl;

/** \brief Delete a table
*
* Deletion fails if the table does not exist, if the table is opened elsewhere, or if the parent is read-only.
* \param parent is a pointer to the parent object (the object that stores the table's data). Must be non-NULL.
* \param name is the name of the table. Must be null-terminated.
* \returns false if an error occurred, otherwise true.
**/
typedef bool(*ICEDB_tbl_remove_f)(
	ICEDB_fs_hnd* parent,
	const char* name
	);
extern DL_ICEDB ICEDB_tbl_remove_f ICEDB_tbl_remove;

/// Backend function to read table data
typedef bool(*ICEDB_fs_inner_tbl_readMapped_f)(
	const ICEDB_fs_hnd* parent,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const ptrdiff_t *imapp,
	ICEDB_OUT void* out);
extern DL_ICEDB ICEDB_fs_inner_tbl_readMapped_f ICEDB_fs_inner_tbl_readMapped;

/// Backend function to write table data
typedef bool(*ICEDB_fs_inner_tbl_writeMapped_f)(
	ICEDB_fs_hnd* parent,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const ptrdiff_t *imapp,
	const void* in);
extern DL_ICEDB ICEDB_fs_inner_tbl_writeMapped_f ICEDB_fs_inner_tbl_writeMapped;

/** \brief Close a table (and free data structures)
* \param tbl is the table. Must be non-NULL.
* \returns false if an error occurred, otherwise true.
**/
typedef bool(*ICEDB_tbl_close_f)(
	ICEDB_tbl* tbl
	);
extern DL_ICEDB ICEDB_tbl_close_f ICEDB_tbl_close;

/** \brief This acts as a container for all table functions that require a base fs object to act as a container.
**/
struct ICEDB_tbl_container_ftable {
	ICEDB_tbl_create_f create;
	ICEDB_tbl_open_f open;
	ICEDB_tbl_close_f close;
	ICEDB_tbl_remove_f remove;
	ICEDB_tbl_getNumTbls_f count;
	ICEDB_tbl_getTblName_f getName;
	ICEDB_tbl_renameTbl_f rename;
	ICEDB_tbl_exists_f exists;
	ICEDB_fs_inner_tbl_readMapped_f _inner_tbl_readMapped;
	ICEDB_fs_inner_tbl_writeMapped_f _inner_tbl_writeMapped;
};
extern DL_ICEDB const struct ICEDB_tbl_container_ftable ICEDB_funcs_tbl_container;


#endif