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
typedef ICEDB_fs_hnd* ICEDB_fs_hnd_p;

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
DL_ICEDB ICEDB_fs_getHandlers_f ICEDB_fs_getHandlers;

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
	ICEDB_OPTIONAL ICEDB_fs_hnd_p base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags,
	ICEDB_OUT size_t* const numHandlersThatCanOpen,
	ICEDB_OPTIONAL ICEDB_OUT char *** const pluginids,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPPP_f * const deallocator,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);
DL_ICEDB ICEDB_fs_canOpen_f ICEDB_fs_canOpen;

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
typedef ICEDB_fs_hnd_p(*ICEDB_fs_open_f)(
	const char* path, 
	ICEDB_OPTIONAL const char* ftype,
	ICEDB_OPTIONAL const char* pluginid,
	ICEDB_OPTIONAL ICEDB_fs_hnd_p base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags);
DL_ICEDB ICEDB_fs_open_f ICEDB_fs_open;

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
	ICEDB_fs_hnd_p p, 
	ICEDB_OPTIONAL size_t inPathSize, 
	ICEDB_OUT size_t* outPathSize, 
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator);
DL_ICEDB ICEDB_fs_getPath_f ICEDB_fs_getPath;

/** \brief Get the ICEDB_file_open_flags passed to the plugin when the handle was opened
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \param err is an error code.
	**/
typedef ICEDB_file_open_flags(*ICEDB_fs_getOpenFlags_f)(ICEDB_fs_hnd_p p);
DL_ICEDB ICEDB_fs_getOpenFlags_f ICEDB_fs_getOpenFlags;

/** \brief Close a file handle
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \returns an error code. Zero on success.
	**/
typedef ICEDB_error_code(*ICEDB_fs_close_f)(ICEDB_fs_hnd_p p);
DL_ICEDB ICEDB_fs_close_f ICEDB_fs_close;

/** \brief Move an object
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \param src is the source path. Must be a valid c-string. Cannot be NULL.
	* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
	* \param err is an error code.
	* \returns Zero on success, nonzero on any error.
	**/
typedef ICEDB_error_code(*ICEDB_fs_move_f)(ICEDB_fs_hnd_p p, const char* src, const char* dest);
DL_ICEDB ICEDB_fs_move_f ICEDB_fs_move;

/** \brief Copy an object
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns Zero on success, nonzero on any error.
**/
typedef ICEDB_error_code(*ICEDB_fs_copy_f)(ICEDB_fs_hnd_p p, const char* src, const char* dest, bool overwrite);
DL_ICEDB ICEDB_fs_copy_f ICEDB_fs_copy;

/** \brief Unlink an object
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns Zero on success, nonzero on any error.
**/
typedef ICEDB_error_code(*ICEDB_fs_unlink_f)(ICEDB_fs_hnd_p p, const char* path);
DL_ICEDB ICEDB_fs_unlink_f ICEDB_fs_unlink;

/** \brief Create a hard link
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns Zero on success, nonzero on any error.
**/
typedef ICEDB_error_code(*ICEDB_fs_createHardLink_f)(ICEDB_fs_hnd_p p, const char* src, const char* dest);
DL_ICEDB ICEDB_fs_createHardLink_f ICEDB_fs_createHardLink;

/** \brief Create a symbolic link
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns Zero on success, nonzero on any error.
**/
typedef ICEDB_error_code(*ICEDB_fs_createSymLink_f)(ICEDB_fs_hnd_p p, const char* src, const char* dest);
DL_ICEDB ICEDB_fs_createSymLink_f ICEDB_fs_createSymLink;

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
typedef const char*(*ICEDB_fs_followSymLink_f)(ICEDB_fs_hnd_p p,
	const char* path, 
	ICEDB_OPTIONAL size_t inPathSize,
	ICEDB_OUT size_t* outPathSize,
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator);
DL_ICEDB ICEDB_fs_followSymLink_f ICEDB_fs_followSymLink;

/** \brief Does a path exist?
*
* \param p is the pointer to a file handle. It may be NULL.
* \param path is the location of the path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns True if the path exists, false if the path does not exist or if there is an error (such as when a parent path does not exist).
**/
typedef bool(*ICEDB_fs_pathExists_f)(ICEDB_fs_hnd_p p, const char* path, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);
DL_ICEDB ICEDB_fs_pathExists_f ICEDB_fs_pathExists;

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
	ICEDB_fs_hnd_p p,
	const char* path,
	ICEDB_OUT ICEDB_fs_path_contents *res);
DL_ICEDB ICEDB_fs_pathInfo_f ICEDB_fs_pathInfo;

/** \brief Free a path structure
*
* \param pc is a pointer to the path information structure. Must be non-NULL.
* \see ICEDB_fh_path_info
**/
typedef void(*ICEDB_fs_pathInfoFree_f)(ICEDB_fs_path_contents *pc);
DL_ICEDB ICEDB_fs_pathInfoFree_f ICEDB_fs_pathInfoFree;

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
	ICEDB_fs_hnd_p p, 
	const char* path, 
	size_t *numObjs, 
	ICEDB_OUT ICEDB_fs_path_contents *** const res);
DL_ICEDB ICEDB_fs_readObjs_f ICEDB_fs_readObjs;

/** \brief Free the results of a ICEDB_fh_readObjs call
* \param p is a pointer to the path information structure that gets populated. Must be non-NULL. Must be freed with ICEDB_fh_freeObjs after use.
* \see ICEDB_fh_readObjs
**/
typedef void(*ICEDB_fs_freeObjs_f)(
	ICEDB_OUT ICEDB_fs_path_contents *** const p);
DL_ICEDB ICEDB_fs_freeObjs_f ICEDB_fs_freeObjs;

enum ICEDB_DATA_TYPES {
	ICEDB_TYPE_CHAR, // NC_CHAR
	ICEDB_TYPE_INT8, // NC_BYTE
	ICEDB_TYPE_UINT8, // NC_UBYTE
	ICEDB_TYPE_UINT16, // NC_USHORT
	ICEDB_TYPE_INT16, // NC_SHORT
	ICEDB_TYPE_UINT32, // NC_UINT
	ICEDB_TYPE_INT32, // NC_INT (or NC_LONG)
	ICEDB_TYPE_UINT64, // NC_UINT64
	ICEDB_TYPE_INT64, // NC_INT64
	ICEDB_TYPE_FLOAT, // NC_FLOAT
	ICEDB_TYPE_DOUBLE, // NC_DOUBLE
					   // These have no corresponding NetCDF type. They never get saved by themselves, but contain pointers to things like string arrays, which are NetCDF objects.
					   ICEDB_TYPE_INTMAX,
					   ICEDB_TYPE_INTPTR,
					   ICEDB_TYPE_UINTMAX,
					   ICEDB_TYPE_UINTPTR
};

struct ICEDB_fs_container_vtable {
	ICEDB_fs_getHandlers_f getHandlers;
	ICEDB_fs_canOpen_f canOpen;
	ICEDB_fs_open_f open;
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
};

DL_ICEDB const ICEDB_fs_container_vtable* ICEDB_fs_getContainerFunctions();

/** @} */ // end of fs

ICEDB_END_DECL_C
#endif
