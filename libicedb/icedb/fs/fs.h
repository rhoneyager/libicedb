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
struct ICEDB_fs_container_vtable;

#include "fs_enums.h"
#include "fs_attr_base.h"
#include "fs_tbl_base.h"
#include "fs_path_functions.h"

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
typedef bool(*ICEDB_fs_canOpenPath_f)(
	const char* path,
	ICEDB_OPTIONAL const char* pathtype,
	ICEDB_OPTIONAL const char* pluginid,
	ICEDB_OPTIONAL const ICEDB_fs_hnd* base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags,
	ICEDB_OUT size_t* const numHandlersThatCanOpen,
	ICEDB_OPTIONAL ICEDB_OUT char *** const pluginids,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPPP_f * const deallocator,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);
extern DL_ICEDB ICEDB_fs_canOpenPath_f ICEDB_fs_canOpenPath;

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
typedef ICEDB_fs_hnd*(*ICEDB_fs_openPath_f)(
	const char* path, 
	ICEDB_OPTIONAL const char* ftype,
	ICEDB_OPTIONAL const char* pluginid,
	ICEDB_OPTIONAL ICEDB_fs_hnd* base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags);
extern DL_ICEDB ICEDB_fs_openPath_f ICEDB_fs_openPath;

/** \brief Duplicate a handle
*
* \param handle is a filesystem handle to some object
* \returns a filesystem handle to the opened object. NULL if an error occurred.
**/
typedef ICEDB_fs_hnd*(*ICEDB_fs_cloneHandle_f)(
	ICEDB_fs_hnd* handle);
extern DL_ICEDB ICEDB_fs_cloneHandle_f ICEDB_fs_clone;


/** \brief Get the full path opened by the file handle.
	*
	* \param handle is the pointer to the file handle. It must be a valid pointer.
	* \param inPathSize is used if the output name array is already pre-allocated, in which case it represents the size (in bytes) of the array.
		If dynamic allocation is instead requested, set this to NULL.
	* \param outPathSize is a pointer to the number of bytes in the output array neede to represent the path. If the path is too large to fit into 
		a staticly-allocated array, then an error code will be emitted.
	* \param bufPath is a pointer to the output path array. Always will be null-terminated.
	* \param deallocator is a function that can deallocate bufPath once it is no longer needed. Use this only when inPathSize != 0.
	* \param err is an error code.
	* \returns bufPath on success, and NULL on error.
**/
typedef const char*(*ICEDB_fs_getPathFromHandle_f)(
	const ICEDB_fs_hnd* handle, 
	ICEDB_OPTIONAL size_t inPathSize, 
	ICEDB_OUT size_t* outPathSize, 
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator);
extern DL_ICEDB ICEDB_fs_getPathFromHandle_f ICEDB_fs_getPath;

/** \brief Get the ICEDB_file_open_flags passed to the plugin when the handle was opened
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \param err is an error code.
	**/
typedef ICEDB_file_open_flags(*ICEDB_fs_getHandleIOflags_f)(const ICEDB_fs_hnd* p);
extern DL_ICEDB ICEDB_fs_getHandleIOflags_f ICEDB_fs_getOpenFlags;

/** \brief Close a file handle
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \returns an error code. Zero on success.
	**/
typedef ICEDB_error_code(*ICEDB_fs_closeHandle_f)(ICEDB_fs_hnd* p);
extern DL_ICEDB ICEDB_fs_closeHandle_f ICEDB_fs_close;

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
typedef bool(*ICEDB_fs_doesPathExist_f)(const ICEDB_fs_hnd* p, const char* path, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);
extern DL_ICEDB ICEDB_fs_doesPathExist_f ICEDB_fs_pathExists;

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


typedef ICEDB_fs_path_contents *** const ICEDB_fs_objectList_t;
/** \brief Enumerate all one-level child objects
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param path is the location of the path. Must be a valid c-string. It may be NULL, in which case p must be non-NULL.
* \param numObjs is a pointer to store number of objects that were found and included in res.
* \param res is a pointer to the path information structure that gets populated. Must be non-NULL. Must be freed with ICEDB_fh_freeObjs after use.
* \param err is an error code.
* \see ICEDB_fh_freeObjs
* \returns A pointer to the path information structure (same as res). Returned for convenience. Returns NULL if an error occurred (see err).
**/
typedef ICEDB_fs_objectList_t(*ICEDB_fs_getAllObjects_f)(
	const ICEDB_fs_hnd* p, 
	const char* path, 
	size_t *numObjs, 
	ICEDB_OUT ICEDB_fs_objectList_t res);
extern DL_ICEDB ICEDB_fs_getAllObjects_f ICEDB_fs_readObjs;

/** \brief Free the results of a ICEDB_fh_readObjs call
* \param p is a pointer to the path information structure that gets populated. Must be non-NULL. Must be freed with ICEDB_fh_freeObjs after use.
* \see ICEDB_fh_readObjs
**/
typedef void(*ICEDB_fs_freeObjs_f)(
	ICEDB_fs_objectList_t p);
extern DL_ICEDB ICEDB_fs_freeObjs_f ICEDB_fs_freeObjs;


struct ICEDB_fs_container_ftable {
	ICEDB_fs_getHandlers_f getHandlers;
	ICEDB_fs_canOpenPath_f canOpen;
	ICEDB_fs_openPath_f open;
	ICEDB_fs_cloneHandle_f clone;
	ICEDB_fs_getPathFromHandle_f getPath;
	ICEDB_fs_getHandleIOflags_f getOpenFlags;
	ICEDB_fs_closeHandle_f close;
	ICEDB_fs_move_f move;
	ICEDB_fs_copy_f copy;
	ICEDB_fs_unlink_f unlink;
	ICEDB_fs_createHardLink_f createHardLink;
	ICEDB_fs_createSymLink_f createSymLink;
	ICEDB_fs_followSymLink_f followSymLink;
	ICEDB_fs_doesPathExist_f pathExists;
	ICEDB_fs_pathInfo_f pathInfo;
	ICEDB_fs_pathInfoFree_f pathInfoFree;
	ICEDB_fs_getAllObjects_f readObjs;
	ICEDB_fs_freeObjs_f freeObjs;
	const struct ICEDB_attr_container_ftable attrs;
	const struct ICEDB_tbl_container_ftable tbls;
};

extern DL_ICEDB const struct ICEDB_fs_container_ftable ICEDB_funcs_fs;


/** @} */ // end of fs

ICEDB_END_DECL_C
#endif
