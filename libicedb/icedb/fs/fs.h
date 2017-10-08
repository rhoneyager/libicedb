#pragma once
#ifndef ICEDB_H_FS_HANDLE
#define ICEDB_H_FS_HANDLE
#include "../defs.h"
#include "../error/error.h"
#include "../misc/mem.h"

ICEDB_BEGIN_DECL_C

/** @defgroup fs Filesystem
 * Filesystem handles are opaque types. They are implemented using several different backends - 
 * for folders, for compressed archives, for netcdf and hdf5 files, et cetera.
 * To switch to a different backend, open a new handle pointing to a place along the old path. For example,
 * you can open a directory, find a netcdf file, then open it, then find a shape structure, then view 
 * its attributes. Actually reading the data (other than basic attributes) is the provenance of other
 * classes. Those classes, however, might make use of the handles provided here, and the various fs dlls
 * may provide such facilities.
 * @{
 **/
struct ICEDB_fs_hnd;
//typedef ICEDB_fs_hnd* ICEDB_fs_hnd_p;
//typedef const ICEDB_fs_hnd* ICEDB_fs_hnd_cp;

/// \brief These flags are used when opening a path. They are mutually exclusive.
enum ICEDB_file_open_flags {
	ICEDB_flags_invalid = 0,
	ICEDB_flags_none = 1, ///< No special options. Open path for read/write. If it does not exist, create it.
	ICEDB_flags_create = 2, ///< Create a new path. Fails if a path already exists. Read-write is implied.
	ICEDB_flags_truncate = 4, ///< Create a new path, replacing an old path if it already exists. Read-write is implied.
	ICEDB_flags_rw = 8, ///< Open an existing path for read-write access. Path must already exist.
	ICEDB_flags_readonly = 16 ///< Open an existing path for read-only access. Path must already exist.
};

/// These indicate the type of a path. This is, is the path a folder, a sybolic link, a regular file...
enum ICEDB_path_types {
	ICEDB_path_type_nonexistant, ///< Path does not exist
	ICEDB_path_type_unknown, ///< Path type is unhandled. May be a block device, a mapped memory page, etc.
	ICEDB_path_type_normal_file, ///< Path is a regular file
	ICEDB_path_type_folder, ///< Path is a directory or group
	ICEDB_path_type_symlink ///< Path is a symbolic link. Can check to see if it is dereferencable.
};

/// These indicate options for path iteration
enum ICEDB_path_iteration {
	ICEDB_path_iteration_base, ///< Only look at the base path
	ICEDB_path_iteration_one, ///< Only look at immediate children
	ICEDB_path_iteration_subtree, ///< Recurse through all children. Not base.
	ICEDB_path_iteration_recursive ///< Recurse through base and all chuldren
};

struct ICEDB_fs_container_vtable;

/** \brief Retrieve all plugin handlers in a given registry
 *
 * \param registry is the name of the plugin registry.
 * \param numPlugins is the number of entries in the registry that were found.
 * \param pluginids is the set of matching plugin id values. You provide a valid char*** pointer, and the function populates it. 
 *		The populated char** is null-terminated. Once you are finished using it, then it should be freed using the deallocator function.
 * \param deallocator is the function used to free the list of pluginids. It must be called to avoid memory leaks.
 **/
typedef void(*ICEDB_fs_getHandlers_f)(
	const char* registry,
	ICEDB_OUT size_t* const numPlugins,
	ICEDB_OUT char *** const pluginids,
	ICEDB_OUT ICEDB_free_charIPPP_f * const deallocator
	);
extern DL_ICEDB ICEDB_fs_getHandlers_f ICEDB_fs_getHandlers;

/** \brief Can a path be opened?
	*
	* \param path is the path. Can be relative or absolute. If base_handle is provided, then the path is relative to the base_handle's current path.
	*	Otherwise, it is relative to the application's current working directory.
	* \see getCWD
	* \param pathtype is the type of path to read. If not specified, then this is automatically detected.
	* \param pluginid is specified if you want to see if a given plugin can open a particular path.
	* \param base_handle is a filesystem handle to some parent object, to provide a starting location
	*	for path searches. It could, for example, point inside an already-opened netCDF file.
	* \param flags are the file-open flags. Some plugins can read, but not write, and vice versa.
	* \param numHandlersThatCanOpen returns the number of plugins that can read the path.
	* \param pluginids is a list of plugins that can read the path. This list is ordered, and the ordering reflects 
	*	how well each plugin thinks that it can understand the path. The top scores come first. The list is null-terminated.
	* \param deallocator is a function that can deallocate pluginids once it is no longer needed.
	* \param err is an error code. Non-NULL if there is an error. NULL otherwise.
	* \see err.h
	* \returns true if the path can be opened by at least one plugin. False if either no plugin can open the path, or if an error occurred.
**/
typedef bool(*ICEDB_fs_canOpen_f)(
	const char* path,
	ICEDB_OPTIONAL const char* pathtype,
	ICEDB_OPTIONAL const char* pluginid,
	ICEDB_OPTIONAL const ICEDB_fs_hnd* base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags,
	ICEDB_OUT size_t* const numHandlersThatCanOpen,
	ICEDB_OPTIONAL ICEDB_OUT char *** const pluginids,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPPP_f * const deallocator,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);
extern DL_ICEDB ICEDB_fs_canOpen_f ICEDB_fs_canOpen;

/** \brief Open a path and get a handle.
	* 
	* \param path is the path. Can be relative or absolute. If base_handle is provided, then the path is relative to the base_handle's current path.
	*	Otherwise, it is relative to the application's current working directory.
	* \see getCWD
	* \param ftype is the type of path to read. If not specified, then this is automatically detected.
	* \param pluginid is the plugin that is queried. Optional, as this can be automatically detected.
	* \param base_handle is a filesystem handle to some parent object, to provide a starting location
	*	for path searches. It could, for example, point inside an already-opened netCDF file.
	* \param flags are the file-open flags. Some plugins can read, but not write, and vice versa.
	* \param err is an error code. Non-NULL if there is an error. NULL otherwise.
	* \see err.h
	* \returns a filesystem handle to the opened object. NULL if an error occurred.
**/
typedef ICEDB_fs_hnd*(*ICEDB_fs_open_f)(
	const char* path, 
	ICEDB_OPTIONAL const char* ftype,
	ICEDB_OPTIONAL const char* pluginid,
	ICEDB_OPTIONAL ICEDB_fs_hnd* base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags);
extern DL_ICEDB ICEDB_fs_open_f ICEDB_fs_open;

/** \brief Duplicate a handle
*
* \param handle is a filesystem handle to some object
* \returns a filesystem handle to the opened object. NULL if an error occurred.
**/
typedef ICEDB_fs_hnd*(*ICEDB_fs_clone_f)(
	ICEDB_fs_hnd* handle);
extern DL_ICEDB ICEDB_fs_clone_f ICEDB_fs_clone;


/** \brief Get the full path opened by the file handle.
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \param inPathSize is used if the output name array is already pre-allocated, in which case it represents the size (in bytes) of the array.
		If dynamic allocation is instead requested, set this to NULL.
	* \param outPathSize is a pointer to the number of bytes in the output array neede to represent the path. If the path is too large to fit into 
		a staticly-allocated array, then an error code will be emitted.
	* \param bufPath is a pointer to the output path array. Always will be null-terminated.
	* \param deallocator is a function that can deallocate bufPath once it is no longer needed. Use this only when inPathSize != 0.
	* \param err is an error code.
	* \returns bufPath on success, and NULL on error.
**/
typedef const char*(*ICEDB_fs_getPath_f)(
	const ICEDB_fs_hnd* p, 
	ICEDB_OPTIONAL size_t inPathSize, 
	ICEDB_OUT size_t* outPathSize, 
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator);
extern DL_ICEDB ICEDB_fs_getPath_f ICEDB_fs_getPath;

/** \brief Get the ICEDB_file_open_flags passed to the plugin when the handle was opened
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \param err is an error code.
	**/
typedef ICEDB_file_open_flags(*ICEDB_fs_getOpenFlags_f)(const ICEDB_fs_hnd* p);
extern DL_ICEDB ICEDB_fs_getOpenFlags_f ICEDB_fs_getOpenFlags;

/** \brief Close a file handle
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \returns an error code. Zero on success.
	**/
typedef ICEDB_error_code(*ICEDB_fs_close_f)(ICEDB_fs_hnd* p);
extern DL_ICEDB ICEDB_fs_close_f ICEDB_fs_close;

/** \brief Move an object
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \param src is the source path. Must be a valid c-string. Cannot be NULL.
	* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
	* \param err is an error code.
	* \returns Zero on success, nonzero on any error.
	**/
typedef ICEDB_error_code(*ICEDB_fs_move_f)(ICEDB_fs_hnd* p, const char* src, const char* dest);
extern DL_ICEDB ICEDB_fs_move_f ICEDB_fs_move;

/** \brief Copy an object
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns Zero on success, nonzero on any error.
**/
typedef ICEDB_error_code(*ICEDB_fs_copy_f)(ICEDB_fs_hnd* p, const char* src, const char* dest, bool overwrite);
extern DL_ICEDB ICEDB_fs_copy_f ICEDB_fs_copy;

/** \brief Unlink an object
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns Zero on success, nonzero on any error.
**/
typedef ICEDB_error_code(*ICEDB_fs_unlink_f)(ICEDB_fs_hnd* p, const char* path);
extern DL_ICEDB ICEDB_fs_unlink_f ICEDB_fs_unlink;

/** \brief Create a hard link
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns Zero on success, nonzero on any error.
**/
typedef ICEDB_error_code(*ICEDB_fs_createHardLink_f)(ICEDB_fs_hnd* p, const char* src, const char* dest);
extern DL_ICEDB ICEDB_fs_createHardLink_f ICEDB_fs_createHardLink;

/** \brief Create a symbolic link
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns Zero on success, nonzero on any error.
**/
typedef ICEDB_error_code(*ICEDB_fs_createSymLink_f)(ICEDB_fs_hnd* p, const char* src, const char* dest);
extern DL_ICEDB ICEDB_fs_createSymLink_f ICEDB_fs_createSymLink;

/** \brief Follow a symbolic link
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param path is the location of the symbolic link. Must be a valid c-string. Cannot be NULL.
* \param inPathSize is used if the output name array is already pre-allocated, in which case it represents the size (in bytes) of the array.
	If dynamic allocation is instead requested, set this to NULL.
* \param outPathSize is a pointer to the number of bytes in the output array neede to represent the path. If the path is too large to fit into 
	a staticly-allocated array, then an error code will be emitted.
* \param bufPath is a pointer to the output path array.
* \param deallocator is a function that can deallocate bufPath once it is no longer needed. Use this only when inPathSize != 0.
* \param err is an error code.
* \returns bufPath on success, and NULL on error.
**/
typedef const char*(*ICEDB_fs_followSymLink_f)(ICEDB_fs_hnd* p,
	const char* path, 
	ICEDB_OPTIONAL size_t inPathSize,
	ICEDB_OUT size_t* outPathSize,
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator);
extern DL_ICEDB ICEDB_fs_followSymLink_f ICEDB_fs_followSymLink;

/** \brief Does a path exist?
*
* \param p is the pointer to a file handle. It may be NULL.
* \param path is the location of the path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns True if the path exists, false if the path does not exist or if there is an error (such as when a parent path does not exist).
**/
typedef bool(*ICEDB_fs_pathExists_f)(const ICEDB_fs_hnd* p, const char* path, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);
extern DL_ICEDB ICEDB_fs_pathExists_f ICEDB_fs_pathExists;

/// The maximum length of a path.
#define ICEDB_FS_PATH_CONTENTS_PATH_MAX 32767

/** \brief A structure that provides information about a path.
*
* This structure is passed back by either ICEDB_fh_path_info or ICEDB_fh_readObjs. 
* It lists an object's name, type, base path, and whether it is a regular file, a directory or a symbolic link.
*
* \note The _free_fs_path_contents_p and _free_fs_path_contents_pp pointers should not be called directly. These will be made opaque in the future.
**/
struct ICEDB_fs_path_contents {
	int idx; ///< id. A number for each distinct object in a path search. Should be same only for hardlinks.
	ICEDB_path_types p_type; ///< Type of path - regular, dir, symlink
	char *p_name; ///< path name
	char *p_obj_type; ///< Descriptive type of object - hdf5 file, shape, compressed archive, ...
	char *base_path; ///< The base path of an object
	void(*_free_fs_path_contents_p)(ICEDB_fs_path_contents *); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	void(*_free_fs_path_contents_pp)(ICEDB_fs_path_contents **); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	ICEDB_fs_path_contents* next; ///< The next object in the list. End of list is denoted by NULL.
};

/** \brief Get information about a path.
*
* The path is constructed from an optional base path found in p, and the sub-path (path).
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param path is the location of the path. Must be a valid c-string. It may be NULL, in which case p must be non-NULL.
* \param res is a pointer to the path information structure that gets populated. Must be non-NULL. Must be freed with ICEDB_fh_path_info_free after use.
* \param err is an error code.
* \see ICEDB_fh_path_info_free
* \returns A pointer to the path information structure (same as res). Returned for convenience. Returns NULL if an error occurred (see err).
**/
typedef ICEDB_fs_path_contents*(*ICEDB_fs_pathInfo_f)(
	const ICEDB_fs_hnd* p,
	const char* path,
	ICEDB_OUT ICEDB_fs_path_contents *res);
extern DL_ICEDB ICEDB_fs_pathInfo_f ICEDB_fs_pathInfo;

/** \brief Free a path structure
*
* \param pc is a pointer to the path information structure. Must be non-NULL.
* \see ICEDB_fh_path_info
**/
typedef void(*ICEDB_fs_pathInfoFree_f)(ICEDB_fs_path_contents *pc);
extern DL_ICEDB ICEDB_fs_pathInfoFree_f ICEDB_fs_pathInfoFree;

/** \brief Enumerate all one-level child objects
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param path is the location of the path. Must be a valid c-string. It may be NULL, in which case p must be non-NULL.
* \param numObjs is a pointer to store number of objects that were found and included in res.
* \param res is a pointer to the path information structure that gets populated. Must be non-NULL. Must be freed with ICEDB_fh_freeObjs after use.
* \param err is an error code.
* \see ICEDB_fh_freeObjs
* \returns A pointer to the path information structure (same as res). Returned for convenience. Returns NULL if an error occurred (see err).
**/
typedef ICEDB_fs_path_contents *** const(*ICEDB_fs_readObjs_f)(
	const ICEDB_fs_hnd* p, 
	const char* path, 
	size_t *numObjs, 
	ICEDB_OUT ICEDB_fs_path_contents *** const res);
extern DL_ICEDB ICEDB_fs_readObjs_f ICEDB_fs_readObjs;

/** \brief Free the results of a ICEDB_fh_readObjs call
* \param p is a pointer to the path information structure that gets populated. Must be non-NULL. Must be freed with ICEDB_fh_freeObjs after use.
* \see ICEDB_fh_readObjs
**/
typedef void(*ICEDB_fs_freeObjs_f)(
	ICEDB_OUT ICEDB_fs_path_contents *** const p);
extern DL_ICEDB ICEDB_fs_freeObjs_f ICEDB_fs_freeObjs;

enum ICEDB_DATA_TYPES {
	ICEDB_TYPE_NOTYPE, ///< Signifies no type / an error
	ICEDB_TYPE_CHAR, ///< Equiv. to NC_CHAR
	ICEDB_TYPE_INT8, ///< Equiv. to NC_BYTE
	ICEDB_TYPE_UINT8, ///< Equiv. to NC_UBYTE
	ICEDB_TYPE_UINT16, ///< Equiv. to NC_USHORT
	ICEDB_TYPE_INT16, ///< Equiv. to NC_SHORT
	ICEDB_TYPE_UINT32, ///< Equiv. to NC_UINT
	ICEDB_TYPE_INT32, ///< Equiv. to NC_INT (or NC_LONG)
	ICEDB_TYPE_UINT64, ///< Equiv. to NC_UINT64
	ICEDB_TYPE_INT64, ///< Equiv. to NC_INT64
	ICEDB_TYPE_FLOAT, ///< Equiv. to NC_FLOAT
	ICEDB_TYPE_DOUBLE, ///< Equiv. to NC_DOUBLE
					   // These have no corresponding NetCDF type. They never get saved by themselves, but contain pointers to things like string arrays, which are NetCDF objects.
					   ICEDB_TYPE_INTMAX,
					   ICEDB_TYPE_INTPTR,
					   ICEDB_TYPE_UINTMAX,
					   ICEDB_TYPE_UINTPTR
};

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
	size_t *dims);
/** \brief Open an attribute
* \param parent is a pointer to the parent object (the object that stores the attribute's data). Must be non-NULL.
* \param name is the name of the attribute. Must be null-terminated.
* \param err is an error code
* \returns NULL if an error occurred, otherwise with a new copy of the attribute object.
**/
typedef ICEDB_attr*(*ICEDB_attr_open_f)(
	const ICEDB_fs_hnd* parent,
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
typedef const char*(*ICEDB_attr_getAttrName_f)(
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
typedef bool(*ICEDB_attr_attrExists_f)(const ICEDB_fs_hnd* p, const char* name, ICEDB_OUT ICEDB_error_code* err);
/** \brief Renames an attribute
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param oldname is the attribute's current name. Must be a NULL-terminated C-string.
* \param newname is the attribute's new name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True on success, false if an error occured.
**/
typedef bool(*ICEDB_attr_renameAttr_f)(ICEDB_fs_hnd* p, const char* oldname, const char* newname);
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
typedef bool(*ICEDB_attr_fs_write_backend)(
	ICEDB_fs_hnd* p, const char* name,
	ICEDB_DATA_TYPES type,
	size_t numDims,
	size_t *dims,
	void* data
	);
/** \brief This acts as a container for all attribute functions that require a base fs object to act as a container.
**/
struct ICEDB_attr_container_ftable {
	ICEDB_attr_create_f create;
	ICEDB_attr_open_f open;
	ICEDB_attr_remove_f remove;
	ICEDB_attr_getNumAttrs_f count;
	ICEDB_attr_getAttrName_f getName;
	ICEDB_attr_attrExists_f exists;
	ICEDB_attr_renameAttr_f rename;
	ICEDB_attr_freeAttrList_f freeAttrList;
	ICEDB_attr_openAllAttrs_f openAllAttrs;
	ICEDB_attr_fs_write_backend writeAttrData;
};

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
	size_t *dims
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

/** \brief This acts as a container for all table functions that require a base fs object to act as a container.
**/
struct ICEDB_tbl_container_ftable{
	ICEDB_tbl_create_f create;
	ICEDB_tbl_open_f open;
	ICEDB_tbl_remove_f remove;
	ICEDB_tbl_getNumTbls_f count;
	ICEDB_tbl_getTblName_f getName;
	ICEDB_tbl_renameTbl_f rename;
	ICEDB_tbl_exists_f exists;
};

struct ICEDB_fs_container_ftable {
	ICEDB_fs_getHandlers_f getHandlers;
	ICEDB_fs_canOpen_f canOpen;
	ICEDB_fs_open_f open;
	ICEDB_fs_clone_f clone;
	ICEDB_fs_getPath_f getPath;
	ICEDB_fs_getOpenFlags_f getOpenFlags;
	ICEDB_fs_close_f close;
	ICEDB_fs_move_f move;
	ICEDB_fs_copy_f copy;
	ICEDB_fs_unlink_f unlink;
	ICEDB_fs_createHardLink_f createHardLink;
	ICEDB_fs_createSymLink_f createSymLink;
	ICEDB_fs_followSymLink_f followSymLink;
	ICEDB_fs_pathExists_f pathExists;
	ICEDB_fs_pathInfo_f pathInfo;
	ICEDB_fs_pathInfoFree_f pathInfoFree;
	ICEDB_fs_readObjs_f readObjs;
	ICEDB_fs_freeObjs_f freeObjs;
	const struct ICEDB_attr_container_ftable attrs;
	const struct ICEDB_tbl_container_ftable tbls;
};

extern DL_ICEDB const struct ICEDB_fs_container_ftable ICEDB_funcs_fs;


/** @} */ // end of fs

ICEDB_END_DECL_C
#endif
