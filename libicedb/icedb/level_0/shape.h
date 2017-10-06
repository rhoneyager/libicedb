#pragma once
#ifndef ICEDB_H_SHAPE
#define ICEDB_H_SHAPE
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/error/error.h"
#include "../fs/fs.h"
#include "../misc/hash.h"
//#include "../data/attrs.h"
//#include "../data/tables.h"

/// This is the public header file that defines the shape manipulation operations.

struct ICEDB_L0_SHAPE_VOL_SPARSE;
typedef ICEDB_L0_SHAPE_VOL_SPARSE* ICEDB_L0_SHAPE_VOL_SPARSE_p;

typedef ICEDB_L0_SHAPE_VOL_SPARSE ICEDB_SHAPE;
typedef ICEDB_L0_SHAPE_VOL_SPARSE_p ICEDB_SHAPE_p;

typedef size_t(*ICEDB_SHAPE_getSize_f)(const ICEDB_SHAPE_p);
typedef void(*ICEDB_SHAPE_setSize_f)(ICEDB_SHAPE_p, size_t);
typedef bool(*ICEDB_SHAPE_setStrAttr_f)(ICEDB_SHAPE_p, const char*);
typedef const char*(*ICEDB_SHAPE_getStrAttr_f)(const ICEDB_SHAPE_p);
typedef void(*ICEDB_SHAPE_getScattElemCoords_f)(const ICEDB_SHAPE_p, ICEDB_OUT float*);
typedef void(*ICEDB_SHAPE_setScattElemCoords_f)(ICEDB_SHAPE_p, const float*);
typedef bool(*ICEDB_SHAPE_tableExists_f)(const ICEDB_SHAPE_p, const char*);
typedef ICEDB_fs_hnd_p(*ICEDB_SHAPE_getParentPtr_f)(const ICEDB_SHAPE_p);
typedef bool(*ICEDB_SHAPE_destructor_f)(ICEDB_SHAPE_p, ICEDB_OUT ICEDB_error_code*);
typedef bool(*ICEDB_SHAPE_hash_f)(const ICEDB_SHAPE_p, ICEDB_OUT ICEDB_HASH_t*);
typedef bool(*ICEDB_SHAPE_idnum_f)(const ICEDB_SHAPE_p, ICEDB_OUT uint64_t*);
typedef bool(*ICEDB_SHAPE_copy_open_f)(const ICEDB_SHAPE_p, ICEDB_fs_hnd_p, ICEDB_OUT ICEDB_SHAPE_p, ICEDB_OUT ICEDB_error_code*);
typedef bool(*ICEDB_SHAPE_copy_f)(const ICEDB_SHAPE_p, ICEDB_fs_hnd_p, ICEDB_OUT ICEDB_error_code*);
//typedef ICEDB_ATTR_TYPES(*ICEDB_SHAPE_getTableType_f)(const ICEDB_SHAPE_p, const char*);

// Args are: ptr to shape, number of table dimensions (should be 2), size of each dimension, pointer to the data.
// Returns: a bool indicating success if true
//typedef bool(*ICEDB_SHAPE_getFloatTable_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, float* const*);
//typedef bool(*ICEDB_SHAPE_getDoubleTable_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, double**);
//typedef bool(*ICEDB_SHAPE_getUInt8Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, uint8_t**);
//typedef bool(*ICEDB_SHAPE_getInt8Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, int8_t**);
//typedef bool(*ICEDB_SHAPE_getUInt16Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, uint16_t**);
//typedef bool(*ICEDB_SHAPE_getInt16Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, int16_t**);
//typedef bool(*ICEDB_SHAPE_getUInt32Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, uint32_t**);
//typedef bool(*ICEDB_SHAPE_getInt32Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, int32_t**);
//typedef bool(*ICEDB_SHAPE_getUInt64Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, uint64_t**);
//typedef bool(*ICEDB_SHAPE_getInt64Table_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, int64_t**);
//typedef bool(*ICEDB_SHAPE_getCharTable_f)(const ICEDB_SHAPE_p, const char*, size_t*, size_t**, char**);

/// This is a convenient container for holding the functions that can operate on a shape.
struct ICEDB_L0_SHAPE_VOL_SPARSE_vtable {
	ICEDB_SHAPE_destructor_f _destructor; ///< Removes shape from memory and performs clean-up tasks. Do not call directly.
	ICEDB_SHAPE_getParentPtr_f getParent; ///< Get the underlying (low-level) filesystem object.
	ICEDB_SHAPE_setStrAttr_f setDescription; ///< Set the description.
	//ICEDB_SHAPE_getStrAttr_f getDescription; ///< Get the description.
	//ICEDB_SHAPE_getScattElemCoords_f getScattElemCoords; ///< Get the scattering element coordinates. Coordinates are in row-major form, in the order of x1, y1, z1, x2, y2, z2, ...
	ICEDB_SHAPE_getSize_f getNumPoints; ///< Get the number of scattering elements.
	ICEDB_SHAPE_setSize_f setNumPoints; ///< Resize the number of scattering elements. This operation is destructive. It removes all existing stored tables.
	//ICEDB_SHAPE_setScattElemCoords_f setScattElemCoords; ///< Set the scattering elements.
	//ICEDB_SHAPE_hash_f getHash; ///< Get a unique identifier for this shape.
	ICEDB_SHAPE_idnum_f getID; ///< Get a unique identifier for this shape.
	ICEDB_SHAPE_copy_open_f copy_open; ///< Copy a shape to a new location. Return a pointer to the new, opened shape.
	ICEDB_SHAPE_copy_f copy; ///< Copy a shape to a new location.
	/// \todo Get scattering element types
	/// \todo Set scattering element types
	/// \todo Add support for manipulating other tables here, without getting the backend fs object.
	/// \todo Add support for manipulating attributes here, without getting the backend fs object.
};

struct _ICEDB_L0_SHAPE_VOL_SPARSE_impl;

/// Represents a shape using a sparse-matrix form.
struct ICEDB_L0_SHAPE_VOL_SPARSE {
	_ICEDB_L0_SHAPE_VOL_SPARSE_impl *_p; ///< An opaque pointer containing private implementation details.
	struct ICEDB_L0_SHAPE_VOL_SPARSE_vtable *_vptrs;
};

/**
* \brief Create a new shape object.
* \param backend is the storage backend used to store this shape. May be NULL, in which case a temporary backend is created.
**/
DL_ICEDB ICEDB_SHAPE_p ICEDB_SHAPE_generate(ICEDB_OPTIONAL ICEDB_fs_hnd_p backend);
/// Remove shape from memory and perform cleanup tasks.
DL_ICEDB void ICEDB_SHAPE_destroy(ICEDB_SHAPE_p);

/** \brief This is a convenience function to open a single shape directly from a file.
* \param filename is the name of the file. Commonly shape.dat.
* \param flags are the i/o flags (i.e. read-only, exclusive, ...)
* \param err is the error code, set upon error.
* \returns A pointer to the loaded shape. NULL on error (see err).
**/
DL_ICEDB ICEDB_SHAPE_p ICEDB_SHAPE_open_single_file(const char* filename, ICEDB_file_open_flags flags, ICEDB_OUT ICEDB_error_code* err);

/** \brief This is a convenience function to open all shapes under a path. 
* \param path is the base path.
* \param pit specifies the type of iteration scheme used when looking for shapes.
* \param flags are the i/o flags (i.e. read-only, exclusive, ...)
* \param numShapes is the number of shapes that were loaded.
* \param shapes is a pointer to an array of loaded shapes. Must be freed once the array is no longer used. Each member should also be freed.
* \param err is the error code, set upon error.
* \returns A pointer to the loaded shape. NULL on error (see err).
**/
DL_ICEDB bool ICEDB_SHAPE_open_path_all(
	const char* path,
	ICEDB_path_iteration pit,
	ICEDB_file_open_flags flags,
	ICEDB_OUT size_t * numShapes,
	ICEDB_OUT ICEDB_SHAPE_p** const shapes,
	ICEDB_OUT ICEDB_error_code* err);

/** \brief Free the results of a ICEDB_SHAPE_open_path_all call
* \param p is a pointer to the list of shapes. 
* \see ICEDB_SHAPE_open_path_all
**/
DL_ICEDB void ICEDB_SHAPE_open_path_all_free(
	ICEDB_OUT ICEDB_SHAPE_p ** const shapes);


#endif
