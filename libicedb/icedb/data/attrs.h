#pragma once
#ifndef ICEDB_H_ATTRS
#define ICEDB_H_ATTRS

#include <stdint.h>
#include <stdbool.h>

#include "../defs.h"
#include "../fs/fs.h"

/** @defgroup atts Attributes
* 
* @{
**/

/** \brief The attribute's data. Expressed using pointers to different possible data types.
**/
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

struct ICEDB_ATTR_ftable;

/** \brief A structure that describes an object attribute.
** \todo This will be made opaque.
**/
struct ICEDB_ATTR {
	ICEDB_ATTR_ftable *funcs; ///< Function table
	ICEDB_fs_hnd* parent; ///< The parent (container) of the attribute. May be NULL, but if non-NULL, then the parent must still EXIST, or else undefined behavior may occur upon write.
	ICEDB_ATTR_DATA data; ///< The attribute data. Expressed as a union.
	ICEDB_DATA_TYPES type; ///< The type of data.
	size_t numDims; ///< The number of dimensions of the data.
	size_t *dims; ///< The dimensions of the data.
	size_t sizeBytes; ///< The size of the data, in __bytes__. 
	size_t sizeElems; ///< The size of the data, in number of elements.
	bool hasFixedSize; ///< Is the data fixed-vidth or variable. If variable, then the entry in data must be NULL-terminated.
	const char* name; ///< The name of the attribute. A NULL-terminated string.
	void(*_free_fs_attr_p)(ICEDB_ATTR *); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	void(*_free_fs_attr_pp)(ICEDB_ATTR **); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	ICEDB_ATTR* next; ///< The next object in the list. End of list is denoted by NULL.
};

//typedef ICEDB_ATTR* ICEDB_ATTR_p;

/** \brief Close an attribute (and free data structures)
* \param attr is the attribute. Must be non-NULL.
* \returns false if an error occurred, otherwise true.
**/
typedef bool(*ICEDB_ATTR_close_f)(ICEDB_ATTR* attr);
/** \brief Write an attribute back to the parent.
* \param attr is the attribute. Must be non-NULL.
* \param err is an error code
* \returns false if an error occurred, otherwise true.
**/
typedef bool(*ICEDB_ATTR_write_f)(ICEDB_ATTR* attr);
/** \brief Copy an attribute
*
* If an unattached attribute is created, call this function to attach the attribute to an fs object.
* All memory is copied into the fs object's context and is subject to its memory management.
* \param newparent is a pointer to the parent object (the object that stores the attribute's data). If NULL, then the attribute will be left dangling.
* \param attr is the attribute to be copied
* \param newname is the new name of the copied attribute. If NULL, then the name is the same.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new copy of the attribute object.
**/
typedef ICEDB_ATTR*(*ICEDB_ATTR_copy_f)(
	ICEDB_OPTIONAL ICEDB_fs_hnd* newparent,
	const ICEDB_ATTR* attr,
	ICEDB_OPTIONAL const char* newname);
/// Get the attribute name
typedef const char*(*ICEDB_ATTR_getName_f)(const ICEDB_ATTR* attr);
/// Get the attribute's parent
typedef ICEDB_fs_hnd*(*ICEDB_ATTR_getParent_f)(const ICEDB_ATTR* attr);
/// Get the attribute's type
typedef ICEDB_DATA_TYPES(*ICEDB_ATTR_getType_f)(const ICEDB_ATTR* attr);
/// Does the attribute have a fixed size?
typedef bool(*ICEDB_ATTR_hasFixedSize_f)(const ICEDB_ATTR* attr);
/// Resize attribute
typedef bool(*ICEDB_ATTR_resize_f)(ICEDB_ATTR* attr, size_t newSize);
/// Get attribute data
typedef const ICEDB_ATTR_DATA*(*ICEDB_ATTR_getData_f)(const ICEDB_ATTR* attr);
/// Set attribute data. Copies attribute's size from indata into the attribute.
typedef bool (*ICEDB_ATTR_setData_f)(ICEDB_ATTR* attr, const void* indata);

DL_ICEDB ICEDB_ATTR_close_f ICEDB_ATTR_close;
DL_ICEDB ICEDB_ATTR_write_f ICEDB_ATTR_write;
DL_ICEDB ICEDB_ATTR_copy_f ICEDB_ATTR_copy;
DL_ICEDB ICEDB_ATTR_getName_f ICEDB_ATTR_getName;
DL_ICEDB ICEDB_ATTR_getParent_f ICEDB_ATTR_getParent;
DL_ICEDB ICEDB_ATTR_getType_f ICEDB_ATTR_getType;
//DL_ICEDB ICEDB_ATTR_hasFixedSize_f ICEDB_ATTR_hasFixedSize;
//DL_ICEDB ICEDB_ATTR_resize_f ICEDB_ATTR_setSizeObjects;
//DL_ICEDB ICEDB_ATTR_resize_f ICEDB_ATTR_setSizeBytes;
DL_ICEDB ICEDB_ATTR_getData_f ICEDB_ATTR_getData;
DL_ICEDB ICEDB_ATTR_setData_f ICEDB_ATTR_setData;

struct ICEDB_ATTR_ftable {
	ICEDB_ATTR_close_f close;
	ICEDB_ATTR_write_f write;
	ICEDB_ATTR_copy_f copy;
	ICEDB_ATTR_getName_f getName;
	ICEDB_ATTR_getParent_f getParent;
	ICEDB_ATTR_getType_f getType;
	//ICEDB_ATTR_hasFixedSize_f hasFixedSize;
	ICEDB_ATTR_getData_f getData;
	ICEDB_ATTR_setData_f setData;
};
DL_ICEDB const ICEDB_ATTR_ftable* ICEDB_ATTR_getAttrFunctions(); ///< Return a static ICEDB_ATTR_vtable*. No need to free.

/** \brief Create an attribute
* \param parent is a pointer to the parent object (the object that stores the attribute's data). If no parent is specified, then the attribute cannot be stored.
* \param type is the type of the attribute.
* \param size is the size, in bytes, of the attribute.
* \param hasSize indicates if the attribute is fixed-size or variable.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new attribute object.
**/
typedef ICEDB_ATTR*(*ICEDB_ATTR_create_f)(
	ICEDB_OPTIONAL ICEDB_fs_hnd* parent, 
	const char* name, 
	ICEDB_DATA_TYPES type, 
	size_t numDims,
	size_t *dims,
	bool hasFixedSize);
/** \brief Open an attribute
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the attribute. Must be null-terminated.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new copy of the attribute object.
**/
typedef ICEDB_ATTR*(*ICEDB_ATTR_open_f)(
	const ICEDB_fs_hnd* parent,
	const char* name);
/** \brief Delete an attribute attached to an object
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the attribute. Must be null-terminated.
* \param err is an error code
* \returns false if an error occurred, otherwise true.
**/
typedef bool(*ICEDB_ATTR_remove_f)(ICEDB_fs_hnd* parent, const char* name);
/** \brief Returns the number of attributes for a filesystem handle.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param err is an error code.
* \returns The number of attributes. If an error occurs, returns 0.
**/
typedef size_t(*ICEDB_ATTR_getNumAttrs_f)(const ICEDB_fs_hnd* p, ICEDB_OUT ICEDB_error_code* err);
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
typedef const char*(*ICEDB_ATTR_getAttrName_f)(
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
typedef bool(*ICEDB_ATTR_attrExists_f)(const ICEDB_fs_hnd* p, const char* name, ICEDB_OUT ICEDB_error_code* err);
/** \brief Renames an attribute
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param oldname is the attribute's current name. Must be a NULL-terminated C-string.
* \param newname is the attribute's new name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True on success, false if an error occured.
**/
typedef bool(*ICEDB_ATTR_renameAttr_f)(ICEDB_fs_hnd* p, const char* oldname, const char* newname);
/** \brief Gets the type of an attribute
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns Attribute type on success, NULL (ICEDB_type_invalid) on error.
**/
typedef ICEDB_DATA_TYPES(*ICEDB_ATTR_getAttrType_f)(const ICEDB_fs_hnd* p, const char* name, ICEDB_OUT ICEDB_error_code* err);
/** \brief Gets the size of an attribute, in bytes.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns Attribute size, in __bytes__. 0 on error.
**/
//typedef size_t(*ICEDB_ATTR_getAttrSize_f)(const ICEDB_fs_hnd* p, const char* name, ICEDB_OUT ICEDB_error_code* err);
/** \brief Determines whether an attribute is of variable length.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True if the length is variable. False if not or on error.
**/
//typedef bool(*ICEDB_ATTR_getAttrIsVariableSize_f)(const ICEDB_fs_hnd* p, const char* name, ICEDB_OUT ICEDB_error_code* err);
/** \brief Free the results of a ICEDB_fh_readAttr call
* \param p is a pointer to the ICEDB_attr*** structure that was populated. Must be non-NULL.
* \see ICEDB_fh_readAllAttrs
**/
typedef bool(*ICEDB_ATTR_freeAttrList_f)(ICEDB_ATTR*** const p);
/** \brief Read all attributes for a file handle.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param numAtts is filled with the number of attributes that were read.
* \param res is the target pointer that gets populated.
* \param err is an error code.
* \returns A pointer to the attributes structure, which must be manually freed when no longer needed. NULL if an error occurred.
* \see ICEDB_fh_freeAttrList
**/
typedef ICEDB_ATTR*** const(*ICEDB_ATTR_openAllAttrs_f)(
	const ICEDB_fs_hnd* p, size_t *numAtts, ICEDB_OUT ICEDB_ATTR*** const res);

/** \brief This acts as a container for all attribute functions that require a base fs object to act as a container.
*
* To get these functions, see ICEDB_ATTR_getFunctions.
* \see ICEDB_ATTR_getFunctions
**/
struct ICEDB_ATTR_container_ftable {
	ICEDB_ATTR_create_f create;
	ICEDB_ATTR_open_f open;
	ICEDB_ATTR_remove_f remove;
	ICEDB_ATTR_close_f close;
	ICEDB_ATTR_getNumAttrs_f count;
	ICEDB_ATTR_getAttrName_f getName;
	ICEDB_ATTR_attrExists_f exists;
	ICEDB_ATTR_renameAttr_f rename;
	ICEDB_ATTR_getAttrType_f getType;
	//ICEDB_ATTR_getAttrIsVariableSize_f hasVariableSize;
	ICEDB_ATTR_freeAttrList_f freeAttrList;
	ICEDB_ATTR_openAllAttrs_f openAllAttrs;
};

DL_ICEDB const ICEDB_ATTR_container_ftable* ICEDB_ATTR_getContainerFunctions(); ///< Returns a static ICEDB_ATTR_container_vtable*. No need to free.

/** @} */ // end of atts

#endif