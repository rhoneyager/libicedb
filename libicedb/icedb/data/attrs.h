#pragma once
#ifndef ICEDB_H_ATTRS
#define ICEDB_H_ATTRS

#include <stdint.h>
#include <stdbool.h>

#include "../defs.h"
#include "../fs/fs.h"

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


/** \brief A structure that describes an object attribute.
**/
struct ICEDB_ATTR {
	ICEDB_fs_hnd_p parent; ///< The parent (container) of the attribute. May be NULL, but if non-NULL, then the parent must still EXIST, or else undefined behavior may occur upon write.
	ICEDB_ATTR_DATA data; ///< The attribute data. Expressed as a union.
	ICEDB_DATA_TYPES type; ///< The type of data.
	size_t size; ///< The size of the data, in __bytes__. The number of values may be computed by size / sizeof(TYPE).
	bool hasFixedSize; ///< Is the data fixed-vidth or variable. If variable, then the entry in data must be NULL-terminated.
	const char* name; ///< The name of the attribute. A NULL-terminated string.
	void(*_free_fs_attr_p)(ICEDB_ATTR *); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	void(*_free_fs_attr_pp)(ICEDB_ATTR **); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	ICEDB_ATTR* next; ///< The next object in the list. End of list is denoted by NULL.
};

typedef ICEDB_ATTR* ICEDB_ATTR_p;

/** \brief Create an attribute
* \param parent is a pointer to the parent object (the object that stores the attribute's data). If no parent is specified, then the attribute cannot be stored.
* \param type is the type of the attribute.
* \param size is the size, in bytes, of the attribute.
* \param hasSize indicates if the attribute is fixed-size or variable.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new attribute object.
**/
DL_ICEDB ICEDB_ATTR_p ICEDB_ATTR_create(
	ICEDB_OPTIONAL ICEDB_fs_hnd_p parent,
	ICEDB_DATA_TYPES type,
	size_t size,
	bool hasSize,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
);

/** \brief Open an attribute
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the attribute. Must be null-terminated.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new copy of the attribute object.
**/
DL_ICEDB ICEDB_ATTR_p ICEDB_ATTR_open(
	ICEDB_fs_hnd_p parent,
	const char* name,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
);

/** \brief Delete an attribute attached to an object
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the attribute. Must be null-terminated.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new copy of the attribute object.
**/
DL_ICEDB ICEDB_ATTR_p ICEDB_ATTR_delete(
	ICEDB_fs_hnd_p parent,
	const char* name,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
);

/** \brief Close an attribute (and free data structures)
* \param attr is the attribute. Must be non-NULL.
* \param err is an error code
* \returns false if an error occurred, otherwise true.
**/
DL_ICEDB bool ICEDB_ATTR_close(
	ICEDB_ATTR_p attr,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
);

/** \brief Write an attribute back to the parent.
* \param attr is the attribute. Must be non-NULL.
* \param err is an error code
* \returns false if an error occurred, otherwise true.
**/
DL_ICEDB bool ICEDB_ATTR_write(
	ICEDB_ATTR_p attr,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
);

/** \brief Copy an attribute
*
* If an unattached attribute is created, call this function to attach the attribute to an fs object.
* All memory is copied into the fs object's context and is subject to its memory management.
* \param newparent is a pointer to the parent object (the object that stores the attribute's data). If NULL, then the attribute will be left dangling.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new copy of the attribute object.
**/
DL_ICEDB ICEDB_ATTR_p ICEDB_ATTR_copy(
	ICEDB_OPTIONAL ICEDB_fs_hnd_p newparent,
	ICEDB_ATTR_p attr,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err
);

/** \brief Returns the number of attributes for a filesystem handle.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param err is an error code.
* \returns The number of attributes. If an error occurs, returns 0.
**/
DL_ICEDB size_t ICEDB_ATTR_getNumAttrs(const ICEDB_fs_hnd_p p, ICEDB_OUT ICEDB_error_code* err);

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
DL_ICEDB const char* ICEDB_ATTR_getAttrName(
	const ICEDB_fs_hnd_p p,
	size_t attrnum,
	ICEDB_OPTIONAL size_t inPathSize,
	ICEDB_OUT size_t* outPathSize,
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator,
	ICEDB_OUT ICEDB_error_code* err);

/** \brief Returns whether an attribute exists with the specified name.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True if the attribute exists, false if not. If an error occurs, returns false.
**/
DL_ICEDB bool ICEDB_ATTR_attrExists(const ICEDB_fs_hnd_p p, const char* name, ICEDB_OUT ICEDB_error_code* err);

/** \brief Renames an attribute
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param oldname is the attribute's current name. Must be a NULL-terminated C-string.
* \param newname is the attribute's new name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True on success, false if an error occured.
**/
DL_ICEDB bool ICEDB_ATTR_renameAttr(ICEDB_fs_hnd_p p, const char* oldname, const char* newname, ICEDB_OUT ICEDB_error_code* err);

/** \brief Gets the type of an attribute
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns Attribute type on success, NULL (ICEDB_type_invalid) on error.
**/
DL_ICEDB ICEDB_DATA_TYPES ICEDB_ATTR_getAttrType(const ICEDB_fs_hnd_p p, const char* name, ICEDB_OUT ICEDB_error_code* err);

/** \brief Gets the size of an attribute, in bytes.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns Attribute size, in __bytes__. 0 on error.
**/
DL_ICEDB size_t ICEDB_ATTR_getAttrSize(const ICEDB_fs_hnd_p p, const char* name, ICEDB_OUT ICEDB_error_code* err);

/** \brief Determines whether an attribute is of variable length.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True if the length is variable. False if not or on error.
**/
DL_ICEDB bool ICEDB_ATTR_getAttrIsVariableSize(const ICEDB_fs_hnd_p p, const char* name, ICEDB_OUT ICEDB_error_code* err);

/** \brief Free the results of a ICEDB_fh_readAttr call
* \param p is a pointer to the ICEDB_attr*** structure that was populated. Must be non-NULL.
* \see ICEDB_fh_readAllAttrs
**/
DL_ICEDB void ICEDB_ATTR_freeAttrList(ICEDB_ATTR_p** const p);

/** \brief Read all attributes for a file handle.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param numAtts is filled with the number of attributes that were read.
* \param res is the target pointer that gets populated.
* \param err is an error code.
* \returns A pointer to the attributes structure, which must be manually freed when no longer needed. NULL if an error occurred.
* \see ICEDB_fh_freeAttrList
**/
DL_ICEDB ICEDB_ATTR_p** const ICEDB_ATTR_openAllAttrs(
	const ICEDB_fs_hnd_p p,
	size_t *numAtts,
	ICEDB_OUT ICEDB_ATTR_p** const res,
	ICEDB_OUT ICEDB_error_code* err);


#endif