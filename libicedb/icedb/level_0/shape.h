#pragma once
#ifndef ICEDB_H_SHAPE
#define ICEDB_H_SHAPE
#include "../../libicedb/icedb/defs.h"
//#include "../../libicedb/icedb/error/error.h"
#include "../fs/fs.h"
#include "../misc/hash.h"

ICEDB_BEGIN_DECL_C

/** @defgroup shps Shapes
*
* @{
**/
/// This is the public header file that defines the shape manipulation operations.

struct ICEDB_L0_SHAPE_VOL_SPARSE;
typedef ICEDB_L0_SHAPE_VOL_SPARSE ICEDB_shape;

typedef uint64_t(*ICEDB_shape_getNumPoints_f)(const ICEDB_shape*);
extern DL_ICEDB const ICEDB_shape_getNumPoints_f ICEDB_shape_getNumPoints;
typedef bool(*ICEDB_shape_setSize_f)(ICEDB_shape*, uint64_t);
typedef bool(*ICEDB_shape_setStrAttr_f)(ICEDB_shape*, const char*);
typedef const char*(*ICEDB_shape_getStrAttr_f)(const ICEDB_shape*);
typedef bool(*ICEDB_shape_getScattElemCoords_f)(const ICEDB_shape*, ICEDB_OUT float*);
typedef bool(*ICEDB_shape_setScattElemCoords_f)(ICEDB_shape*, const float*);
typedef bool(*ICEDB_shape_tableExists_f)(const ICEDB_shape*, const char*);
typedef ICEDB_fs_hnd*(*ICEDB_shape_getFSPtr_f)(const ICEDB_shape*);
extern DL_ICEDB const ICEDB_shape_getFSPtr_f ICEDB_shape_getFSself;
extern DL_ICEDB const ICEDB_shape_getFSPtr_f ICEDB_shape_getFSparent;
typedef bool(*ICEDB_shape_close_f)(ICEDB_shape*);
extern DL_ICEDB const ICEDB_shape_close_f ICEDB_shape_close;

typedef bool(*ICEDB_shape_hash_f)(const ICEDB_shape*, ICEDB_OUT ICEDB_HASH_t*);
typedef uint64_t(*ICEDB_shape_getID_f)(const ICEDB_shape*);
extern DL_ICEDB const ICEDB_shape_getID_f ICEDB_shape_getID;
typedef ICEDB_shape*(*ICEDB_shape_copy_open_f)(const ICEDB_shape* src, ICEDB_fs_hnd* hnd);
extern DL_ICEDB const ICEDB_shape_copy_open_f ICEDB_shape_copy_open;
typedef bool(*ICEDB_shape_copy_f)(const ICEDB_shape*, ICEDB_fs_hnd*);
extern DL_ICEDB const ICEDB_shape_copy_f ICEDB_shape_copy;
//typedef ICEDB_attr_TYPES(*ICEDB_shape_getTableType_f)(const ICEDB_shape*, const char*);

// Args are: ptr to shape, number of table dimensions (should be 2), size of each dimension, pointer to the data.
// Returns: a bool indicating success if true
//typedef bool(*ICEDB_shape_getFloatTable_f)(const ICEDB_shape*, const char*, size_t*, size_t**, float* const*);
//typedef bool(*ICEDB_shape_getDoubleTable_f)(const ICEDB_shape*, const char*, size_t*, size_t**, double**);
//typedef bool(*ICEDB_shape_getUInt8Table_f)(const ICEDB_shape*, const char*, size_t*, size_t**, uint8_t**);
//typedef bool(*ICEDB_shape_getInt8Table_f)(const ICEDB_shape*, const char*, size_t*, size_t**, int8_t**);
//typedef bool(*ICEDB_shape_getUInt16Table_f)(const ICEDB_shape*, const char*, size_t*, size_t**, uint16_t**);
//typedef bool(*ICEDB_shape_getInt16Table_f)(const ICEDB_shape*, const char*, size_t*, size_t**, int16_t**);
//typedef bool(*ICEDB_shape_getUInt32Table_f)(const ICEDB_shape*, const char*, size_t*, size_t**, uint32_t**);
//typedef bool(*ICEDB_shape_getInt32Table_f)(const ICEDB_shape*, const char*, size_t*, size_t**, int32_t**);
//typedef bool(*ICEDB_shape_getUInt64Table_f)(const ICEDB_shape*, const char*, size_t*, size_t**, uint64_t**);
//typedef bool(*ICEDB_shape_getInt64Table_f)(const ICEDB_shape*, const char*, size_t*, size_t**, int64_t**);
//typedef bool(*ICEDB_shape_getCharTable_f)(const ICEDB_shape*, const char*, size_t*, size_t**, char**);

/// This is a convenient container for holding the functions that can operate on a shape.
struct ICEDB_L0_SHAPE_VOL_SPARSE_ftable {
	ICEDB_shape_close_f close; ///< Removes shape from memory and performs clean-up tasks.
	ICEDB_shape_getFSPtr_f getFSself; ///< Get the underlying (low-level) filesystem object.
	//ICEDB_shape_getFSPtr_f getFSparent; ///< Get the underlying (low-level) filesystem object.
	//ICEDB_shape_setStrAttr_f setDescription; ///< Set the description.
	//ICEDB_shape_getStrAttr_f getDescription; ///< Get the description.
	//ICEDB_shape_getScattElemCoords_f getScattElemCoords; ///< Get the scattering element coordinates. Coordinates are in row-major form, in the order of x1, y1, z1, x2, y2, z2, ...
	ICEDB_shape_getNumPoints_f getNumPoints; ///< Get the number of scattering elements.
	//ICEDB_shape_setSize_f setNumPoints; ///< Resize the number of scattering elements. This operation is destructive. It removes all existing stored tables.
	//ICEDB_shape_setScattElemCoords_f setScattElemCoords; ///< Set the scattering elements.
	//ICEDB_shape_hash_f getHash; ///< Get a unique identifier for this shape.
	ICEDB_shape_getID_f getID; ///< Get a unique identifier for this shape.
	ICEDB_shape_copy_open_f copy_open; ///< Copy a shape to a new location. Return a pointer to the new, opened shape.
	ICEDB_shape_copy_f copy; ///< Copy a shape to a new location.
	/// \todo Get scattering element types
	/// \todo Set scattering element types
	/// \todo Add support for manipulating other tables here, without getting the backend fs object.
	/// \todo Add support for manipulating attributes here, without getting the backend fs object.
};
extern DL_ICEDB const struct ICEDB_L0_SHAPE_VOL_SPARSE_ftable ICEDB_funcs_shp_obj;

struct _ICEDB_L0_SHAPE_VOL_SPARSE_impl;

/// Represents a shape using a sparse-matrix form.
struct ICEDB_L0_SHAPE_VOL_SPARSE {
	ICEDB_fs_hnd *fsSelf; // , *fsParent;
	//_ICEDB_L0_SHAPE_VOL_SPARSE_impl *_p; ///< An opaque pointer containing private implementation details.
	const struct ICEDB_L0_SHAPE_VOL_SPARSE_ftable *funcs;
};

extern DL_ICEDB const ICEDB_shape_close_f ICEDB_shape_close;

/**
* \brief Create a new shape object.
*
* This function creates a new shape _at_ the location specified by objBackend.
* The fs backend always allows you to create a subpath.
* \param objBackend is the storage backend used to store this shape.
**/
typedef ICEDB_shape*(*ICEDB_shape_generate_f)(ICEDB_fs_hnd* objBackend);
extern DL_ICEDB const ICEDB_shape_generate_f ICEDB_shape_generate;

/** \brief This is a convenience function to open a single shape directly from a file.
* \param filename is the name of the file. Commonly shape.dat.
* \param flags are the i/o flags (i.e. read-only, exclusive, ...)
* \param err is the error code, set upon error.
* \returns A pointer to the loaded shape. NULL on error.
**/
typedef ICEDB_shape*(*ICEDB_shape_open_single_file_f)(const char* filename, ICEDB_file_open_flags flags);
extern DL_ICEDB const ICEDB_shape_open_single_file_f ICEDB_shape_open_single_file;

/** \brief This is a convenience function to open all shapes under a path. 
* \param path is the base path.
* \param pit specifies the type of iteration scheme used when looking for shapes.
* \param flags are the i/o flags (i.e. read-only, exclusive, ...)
* \param numShapes is the number of shapes that were loaded.
* \param shapes is a pointer to a null-terminated array of loaded shapes. Must be freed once the array is no longer used. Each member should NOT be manually freed.
* \returns shapes.
**/
typedef ICEDB_shape*** const (*ICEDB_shape_open_path_all_f)(
	const char* path,
	ICEDB_path_iteration pit,
	ICEDB_file_open_flags flags,
	ICEDB_OUT size_t * numShapes);
extern DL_ICEDB const ICEDB_shape_open_path_all_f ICEDB_shape_open_path_all;

/** \brief Free the results of a ICEDB_shape_open_path_all call
* \param p is a pointer to the list of shapes. 
* \see ICEDB_shape_open_path_all
**/
typedef void(*ICEDB_shape_open_path_all_free_f)(
	ICEDB_shape*** const shapes);
extern DL_ICEDB const ICEDB_shape_open_path_all_free_f ICEDB_shape_open_path_all_free;

struct ICEDB_shp_ftable {
	ICEDB_shape_generate_f generate;
	ICEDB_shape_open_single_file_f openPathSingle;
	ICEDB_shape_open_path_all_f openPathAll;
	ICEDB_shape_open_path_all_free_f openPathAllFree;
};
extern DL_ICEDB const struct ICEDB_shp_ftable ICEDB_funcs_fs_shp;


/** @} */ // end of shps
ICEDB_END_DECL_C

#endif
