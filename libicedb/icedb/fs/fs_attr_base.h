#pragma once
#ifndef ICEDB_H_FS_ATTR_BASE
#define ICEDB_H_FS_ATTR_BASE



// Base attribute manipulation functions go here
struct ICEDB_attr;
/** \brief Create an attribute
* \param parent is a pointer to the parent object (the object that stores the attribute's data). If no parent is specified, then the attribute cannot be stored.
* \param type is the type of the attribute.
* \param size is the size, in bytes, of the attribute.
* \param hasSize indicates if the attribute is fixed-size or variable.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new attribute object.
**/
typedef ICEDB_attr*(*ICEDB_attr_create_f)(
	ICEDB_OPTIONAL ICEDB_fs_hnd* parent,
	const char* name,
	ICEDB_DATA_TYPES type,
	size_t numDims,
	const size_t *dims);
/** \brief Open an attribute
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the attribute. Must be null-terminated.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new copy of the attribute object.
**/
typedef ICEDB_attr*(*ICEDB_attr_open_f)(
	ICEDB_fs_hnd* parent,
	const char* name);
/** \brief Delete an attribute attached to an object
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the attribute. Must be null-terminated.
* \param err is an error code
* \returns false if an error occurred, otherwise true.
**/
typedef bool(*ICEDB_attr_remove_f)(ICEDB_fs_hnd* parent, const char* name);
/** \brief Returns the number of attributes for a filesystem handle.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param err is an error code.
* \returns The number of attributes. If an error occurs, returns 0.
**/
typedef size_t(*ICEDB_attr_getNumAttrs_f)(const ICEDB_fs_hnd* p, ICEDB_OUT ICEDB_error_code* err);
/** \brief Get the name of an attribute
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param attrnum is the attribute number. Spans [0, numAttrs). An error will occur if this is out of range.
* \param inPathSize is used if the output name array is already pre-allocated, in which case it represents the size (in __bytes__) of the array.
If dynamic allocation is instead requested, set this to NULL.
* \param outPathSize is a pointer to the number of bytes in the output array neede to represent the path. If the path is too large to fit into
a staticly-allocated array, then an error code will be emitted.
* \param bufPath is a pointer to the output path array.
* \param deallocator is a function that can deallocate bufPath once it is no longer needed. Use this only when inPathSize != 0.
* \param err is an error code.
* \returns bufPath on success, NULL on error.
**/
typedef const char*(*ICEDB_attr_getName_f)(
	const ICEDB_fs_hnd* p,
	size_t attrnum,
	ICEDB_OPTIONAL size_t inPathSize,
	ICEDB_OUT size_t* outPathSize,
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator);
/** \brief Returns whether an attribute exists with the specified name.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True if the attribute exists, false if not. If an error occurs, returns false.
**/
typedef bool(*ICEDB_attr_exists_f)(const ICEDB_fs_hnd* p, const char* name, ICEDB_OUT ICEDB_error_code* err);
/** \brief Renames an attribute
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param oldname is the attribute's current name. Must be a NULL-terminated C-string.
* \param newname is the attribute's new name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True on success, false if an error occured.
**/
typedef bool(*ICEDB_attr_rename_f)(ICEDB_fs_hnd* p, const char* oldname, const char* newname);
/** \brief Free the results of a ICEDB_fh_readAttr call
* \param p is a pointer to the ICEDB_attr*** structure that was populated. Must be non-NULL.
* \see ICEDB_fh_readAllAttrs
**/
typedef bool(*ICEDB_attr_freeAttrList_f)(ICEDB_attr*** const p);
/** \brief Read all attributes for a file handle.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param numAtts is filled with the number of attributes that were read.
* \param res is the target pointer that gets populated.
* \param err is an error code.
* \returns A pointer to the attributes structure, which must be manually freed when no longer needed. NULL if an error occurred.
* \see ICEDB_fh_freeAttrList
**/
typedef ICEDB_attr*** const(*ICEDB_attr_openAllAttrs_f)(
	const ICEDB_fs_hnd* p, size_t *numAtts, ICEDB_OUT ICEDB_attr*** const res);
/// Backend function to actually write the attribute's data.
typedef bool(*ICEDB_attr_fs_write_backend_f)(
	ICEDB_fs_hnd* p, const char* name,
	ICEDB_DATA_TYPES type,
	size_t numDims,
	size_t *dims,
	void* data
	);
/** \brief Close an attribute (and free data structures)
* \param attr is the attribute. Must be non-NULL.
* \returns false if an error occurred, otherwise true.
**/
typedef bool(*ICEDB_attr_close_f)(ICEDB_attr* attr);
/** \brief This acts as a container for all attribute functions that require a base fs object to act as a container.
**/
struct ICEDB_attr_container_ftable {
	ICEDB_attr_close_f close;
	ICEDB_attr_create_f create;
	ICEDB_attr_open_f open;
	ICEDB_attr_remove_f remove;
	ICEDB_attr_getNumAttrs_f count;
	ICEDB_attr_getName_f getName;
	ICEDB_attr_exists_f exists;
	ICEDB_attr_rename_f rename;
	ICEDB_attr_freeAttrList_f freeAttrList;
	ICEDB_attr_openAllAttrs_f openAllAttrs;
	ICEDB_attr_fs_write_backend_f writeAttrData; ///< \note In fs.cpp
};
extern DL_ICEDB const struct ICEDB_attr_container_ftable ICEDB_funcs_attr_container;


#endif