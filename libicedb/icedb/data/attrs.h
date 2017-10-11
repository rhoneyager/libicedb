#pragma once
#ifndef ICEDB_H_ATTRS
#define ICEDB_H_ATTRS

#include <stdint.h>
#include <stdbool.h>

#include "../defs.h"
#include "../fs/fs.h"

ICEDB_BEGIN_DECL_C

/** @defgroup atts Attributes
* 
* @{
**/

/** \brief The attribute's data. Expressed using pointers to different possible data types.
**/
union ICEDB_attr_DATA {
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
	void* vt;
};

struct ICEDB_attr_ftable;

/** \brief A structure that describes an object attribute.
** \todo This will be made opaque.
**/
struct ICEDB_attr {
	const ICEDB_attr_ftable *funcs; ///< Function table
	ICEDB_fs_hnd* parent; ///< The parent (container) of the attribute. May be NULL, but if non-NULL, then the parent must still EXIST, or else undefined behavior may occur upon write.
	ICEDB_attr_DATA data; ///< The attribute data. Expressed as a union.
	ICEDB_DATA_TYPES type; ///< The type of data.
	size_t numDims; ///< The number of dimensions of the data.
	size_t *dims; ///< The dimensions of the data.
	size_t sizeBytes; ///< The size of the data, in __bytes__. 
	size_t sizeElems; ///< The size of the data, in number of elements.
	//bool hasFixedSize; ///< Is the data fixed-vidth or variable. If variable, then the entry in data must be NULL-terminated.
	char* name; ///< The name of the attribute. A NULL-terminated string.
	//void(*_free_fs_attr_p)(ICEDB_attr *); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	//void(*_free_fs_attr_pp)(ICEDB_attr **); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	//ICEDB_attr* next; ///< The next object in the list. End of list is denoted by NULL.
};

/** \brief Write an attribute back to the parent.
* \param attr is the attribute. Must be non-NULL.
* \param err is an error code
* \returns false if an error occurred, otherwise true.
**/
typedef bool(*ICEDB_attr_write_f)(ICEDB_attr* attr);
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
typedef ICEDB_attr*(*ICEDB_attr_copy_f)(
	const ICEDB_attr* attr,
	ICEDB_OPTIONAL ICEDB_fs_hnd* newparent,
	ICEDB_OPTIONAL const char* newname);
// Get the attribute name
//typedef const char*(*ICEDB_attr_getName_f)(const ICEDB_attr* attr);
/// Get the attribute's parent
//typedef ICEDB_fs_hnd*(*ICEDB_attr_getParent_f)(const ICEDB_attr* attr);
/// Get the attribute's type
//typedef ICEDB_DATA_TYPES(*ICEDB_attr_getType_f)(const ICEDB_attr* attr);
/// Does the attribute have a fixed size?
//typedef bool(*ICEDB_attr_hasFixedSize_f)(const ICEDB_attr* attr);
/// \brief Resize attribute
typedef bool(*ICEDB_attr_resize_f)(ICEDB_attr* attr, ICEDB_DATA_TYPES newType, size_t numDims, const size_t* dims);
// Get attribute data
//typedef const ICEDB_attr_DATA*(*ICEDB_attr_getData_f)(const ICEDB_attr* attr);
/// Set attribute data. Copies attribute's size from indata into the attribute.
typedef bool (*ICEDB_attr_setData_f)(ICEDB_attr* attr, const void* indata, size_t indataByteSize);

extern DL_ICEDB const ICEDB_attr_close_f ICEDB_attr_close;
extern DL_ICEDB const ICEDB_attr_write_f ICEDB_attr_write;
extern DL_ICEDB const ICEDB_attr_copy_f ICEDB_attr_copy;
//extern DL_ICEDB const ICEDB_attr_getName_f ICEDB_attr_getName;
//extern DL_ICEDB const ICEDB_attr_getParent_f ICEDB_attr_getParent;
//extern DL_ICEDB const ICEDB_attr_getType_f ICEDB_attr_getType;
//DL_ICEDB ICEDB_attr_hasFixedSize_f ICEDB_attr_hasFixedSize;
extern DL_ICEDB const ICEDB_attr_resize_f ICEDB_attr_resize;
//extern DL_ICEDB const ICEDB_attr_getData_f ICEDB_attr_getData;
extern DL_ICEDB const ICEDB_attr_setData_f ICEDB_attr_setData;

struct ICEDB_attr_ftable {
	ICEDB_attr_close_f close;
	ICEDB_attr_write_f write;
	ICEDB_attr_copy_f copy;
	//ICEDB_attr_getName_f getName; ///< TODO: Change def to do a string copy to new memory.
	//ICEDB_attr_getParent_f getParent;
	//ICEDB_attr_getType_f getType;
	//ICEDB_attr_getData_f getData;
	ICEDB_attr_resize_f resize;
	ICEDB_attr_setData_f setData;
};
extern DL_ICEDB const struct ICEDB_attr_ftable ICEDB_funcs_attr_obj;


/** @} */ // end of atts
ICEDB_END_DECL_C

#endif