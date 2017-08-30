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
	ICEDB_flags_none = 0, ///< No special options. Open path for read/write. If it does not exist, create it.
	ICEDB_flags_create = 1, ///< Create a new path. Fails if a path already exists. Read-write is implied.
	ICEDB_flags_truncate = 2, ///< Create a new path, replacing an old path if it already exists. Read-write is implied.
	ICEDB_flags_rw = 4, ///< Open an existing path for read-write access. Path must already exist.
	ICEDB_flags_readonly = 8 ///< Open an existing path for read-only access. Path must already exist.
};

/// These indicate the type of a path. This is, is the path a folder, a sybolic link, a regular file...
enum ICEDB_path_types {
	ICEDB_type_nonexistant, ///< Path does not exist
	ICEDB_type_unknown, ///< Path type is unhandled. May be a block device, a mapped memory page, etc.
	ICEDB_type_normal_file, ///< Path is a regular file
	ICEDB_type_folder, ///< Path is a directory or group
	ICEDB_type_symlink ///< Path is a symbolic link. Can check to see if it is dereferencable.
};

/** \brief Retreive all plugin handlers in a given registry
 *
 * \param registry is the name of the plugin registry.
 * \param numPlugins is the number of entries in the registry that were found.
 * \param pluginids is the set of matching plugin id values. You provide a valid char*** pointer, and the function populates it. 
 *		The populated char** is null-terminated. Once you are finished using it, then it should be freed using the deallocator function.
 * \param deallocator is the function used to free the list of pluginids. It must be called to avoid memory leaks.
 **/
DL_ICEDB void ICEDB_fh_getHandlers(
	const char* registry,
	ICEDB_OUT size_t* const numPlugins,
	ICEDB_OUT char *** const pluginids,
	ICEDB_OUT ICEDB_free_charIPPP_f * const deallocator
	);

/** \brief Can a path be opened?
	*
	* \param path is the path. Can be relative or absolute. If base_handle is provided, then the path is relative to the base_handle's current path.
	*	Otherwise, it is relative to the application's current working directory.
	* \see getCWD
	* \param ftype is the type of path to read. If not specified, then this is automatically detected.
	* \param pluginid is the plugin that is queried. Optional, as this can be automatically detected.
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
DL_ICEDB bool ICEDB_path_canOpen(
	const char* path,
	ICEDB_OPTIONAL const char* ftype,
	ICEDB_OPTIONAL const char* pluginid,
	ICEDB_OPTIONAL ICEDB_fs_hnd_p base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags,
	ICEDB_OUT size_t* const numHandlersThatCanOpen,
	ICEDB_OPTIONAL ICEDB_OUT char *** const pluginids,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPPP_f * const deallocator,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

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
DL_ICEDB ICEDB_fs_hnd_p ICEDB_path_open(
	const char* path, 
	ICEDB_OPTIONAL const char* ftype,
	ICEDB_OPTIONAL const char* pluginid, ICEDB_OPTIONAL ICEDB_fs_hnd_p base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code*);

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
DL_ICEDB const char* ICEDB_fh_get_name(
	ICEDB_fs_hnd_p p, 
	ICEDB_OPTIONAL size_t inPathSize, 
	ICEDB_OUT size_t* outPathSize, 
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

/** \brief Get the ICEDB_file_open_flags passed to the plugin when the handle was opened
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \param err is an error code.
	**/
DL_ICEDB ICEDB_file_open_flags ICEDB_fh_getOpenFlags(ICEDB_fs_hnd_p p, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

/** \brief Close a file handle
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \param err is an error code.
	**/
DL_ICEDB bool ICEDB_fh_close(ICEDB_fs_hnd_p p, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code*);

/** \brief Move an object
	*
	* \param p is the pointer to the file handle. It must be a valid pointer.
	* \param src is the source path. Must be a valid c-string. Cannot be NULL.
	* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
	* \param err is an error code.
	* \returns True on success, false on any error.
	**/
DL_ICEDB bool ICEDB_fh_move(ICEDB_fs_hnd_p p, const char* src, const char* dest, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

/** \brief Copy an object
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns True on success, false on any error.
**/
DL_ICEDB bool ICEDB_fh_copy(ICEDB_fs_hnd_p p, const char* src, const char* dest, bool overwrite, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

/** \brief Unlink an object
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns True on success, false on any error.
**/
DL_ICEDB bool ICEDB_fh_unlink(ICEDB_fs_hnd_p p, const char* path, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

/** \brief Create a hard link
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns True on success, false on any error.
**/
DL_ICEDB bool ICEDB_fh_create_hard_link(ICEDB_fs_hnd_p p, const char* src, const char* dest, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

/** \brief Create a symbolic link
*
* \param p is the pointer to the file handle. It must be a valid pointer.
* \param src is the source path. Must be a valid c-string. Cannot be NULL.
* \param dest is the destination path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns True on success, false on any error.
**/
DL_ICEDB bool ICEDB_fh_create_sym_link(ICEDB_fs_hnd_p p, const char* src, const char* dest, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

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
DL_ICEDB const char* ICEDB_fh_follow_sym_link(ICEDB_fs_hnd_p p,
	const char* path, 
	ICEDB_OPTIONAL size_t inPathSize,
	ICEDB_OUT size_t* outPathSize,
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

/** \brief Does a path exist?
*
* \param p is the pointer to a file handle. It may be NULL.
* \param path is the location of the path. Must be a valid c-string. Cannot be NULL.
* \param err is an error code.
* \returns True if the path exists, false if the path does not exist or if there is an error (such as when a parent path does not exist).
**/
DL_ICEDB bool ICEDB_fh_path_exists(ICEDB_fs_hnd_p p, const char* path, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

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
DL_ICEDB ICEDB_fs_path_contents* ICEDB_fh_path_info(
	ICEDB_fs_hnd_p p,
	const char* path,
	ICEDB_OUT ICEDB_fs_path_contents *res,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

/** \brief Free a path structure
*
* \param pc is a pointer to the path information structure. Must be non-NULL.
* \see ICEDB_fh_path_info
**/
DL_ICEDB void ICEDB_fh_path_info_free(ICEDB_fs_path_contents *pc);

/** \brief Enumerate all one-level child objects
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param path is the location of the path. Must be a valid c-string. It may be NULL, in which case p must be non-NULL.
* \param numObjs is a pointer to store number of objects that were found and included in res.
* \param res is a pointer to the path information structure that gets populated. Must be non-NULL. Must be freed with ICEDB_fh_freeObjs after use.
* \param err is an error code.
* \see ICEDB_fh_freeObjs
* \returns A pointer to the path information structure (same as res). Returned for convenience. Returns NULL if an error occurred (see err).
**/
DL_ICEDB ICEDB_fs_path_contents *** const ICEDB_fh_readObjs(
	ICEDB_fs_hnd_p p, 
	const char* path, 
	size_t *numObjs, 
	ICEDB_OUT ICEDB_fs_path_contents *** const res,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err);

/** \brief Free the results of a ICEDB_fh_readObjs call
* \param p is a pointer to the path information structure that gets populated. Must be non-NULL. Must be freed with ICEDB_fh_freeObjs after use.
* \see ICEDB_fh_readObjs
**/
DL_ICEDB void ICEDB_fh_freeObjs(
	ICEDB_OUT ICEDB_fs_path_contents *** const p);


/// This is a union allowing attribute data to be accessed in different ways. Data are allocated in 64-bit blocks. Alignment depends on actual type.
union ICEDB_attr_data {
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

/** \brief These are the filesystem attribute types
*
* Most of these are equivalent to the NetCDF types. Four special types (ICEDB_type_intmax, ICEDB_type_intptr, ICEDB_type_uintmax and ICEDB_type_uintptr)
* are internal to this library. They never get saved by themselves, but contain pointers to things like string arrays, which are represented using other NetCDF objects.
**/
enum ICEDB_attr_type {
	ICEDB_type_invalid, ///< Indicates an error
	ICEDB_type_char, ///< NC_CHAR
	ICEDB_type_int8, ///< NC_BYTE
	ICEDB_type_uint8, ///< NC_UBYTE
	ICEDB_type_uint16, ///< NC_USHORT
	ICEDB_type_int16, ///< NC_SHORT
	ICEDB_type_uint32, ///< NC_UINT
	ICEDB_type_int32, ///< NC_INT (or NC_LONG)
	ICEDB_type_uint64, ///< NC_UINT64
	ICEDB_type_int64, ///< NC_INT64
	ICEDB_type_float, ///< NC_FLOAT
	ICEDB_type_double, ///< NC_DOUBLE
					   // These have no corresponding NetCDF type. They never get saved by themselves, but contain pointers to things like string arrays, which are NetCDF objects.
					   ICEDB_type_intmax,
					   ICEDB_type_intptr,
					   ICEDB_type_uintmax,
					   ICEDB_type_uintptr
};

/** \brief A structure that describes an object attribute. 
**/
struct ICEDB_attr {
	ICEDB_attr_data data; ///< The attribute data. Expressed as a union.
	ICEDB_attr_type type; ///< The type of data.
	size_t size; ///< The size of the data, in __bytes__. The number of values may be computed by size / sizeof(TYPE).
	bool hasFixedSize; ///< Is the data fixed-vidth or variable. If variable, then the entry in data must be NULL-terminated.
	const char* name; ///< The name of the attribute. A NULL-terminated string.
	void(*_free_fs_attr_p)(ICEDB_attr *); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	void(*_free_fs_attr_pp)(ICEDB_attr **); ///< A destructor funcion (used across module boundaries). Will be hidden in the future.
	ICEDB_attr* next; ///< The next object in the list. End of list is denoted by NULL.
};

/** \brief Returns the number of attributes for a filesystem handle.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param err is an error code.
* \returns The number of attributes. If an error occurs, returns 0.
**/
DL_ICEDB size_t ICEDB_fh_getNumAttrs(const ICEDB_fs_hnd_p p, ICEDB_OUT ICEDB_error_code* err);

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
DL_ICEDB const char* ICEDB_fh_getAttrName(
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
DL_ICEDB bool ICEDB_fh_attrExists(const ICEDB_fs_hnd_p p, const char* name, ICEDB_OUT ICEDB_error_code* err);

/** \brief Removes an attribute
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True if the removal is successful, false if an error occured.
**/
DL_ICEDB bool ICEDB_fh_removeAttr(ICEDB_fs_hnd_p p, const char* name, ICEDB_OUT ICEDB_error_code* err);

/** \brief Renames an attribute
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param oldname is the attribute's current name. Must be a NULL-terminated C-string.
* \param newname is the attribute's new name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True on success, false if an error occured.
**/
DL_ICEDB bool ICEDB_fh_renameAttr(ICEDB_fs_hnd_p p, const char* oldname, const char* newname, ICEDB_OUT ICEDB_error_code* err);

/** \brief Gets the type of an attribute
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns Attribute type on success, NULL (ICEDB_type_invalid) on error.
**/
DL_ICEDB ICEDB_attr_type ICEDB_fh_getAttrType(const ICEDB_fs_hnd_p p, const char* name, ICEDB_OUT ICEDB_error_code* err);

/** \brief Gets the size of an attribute, in bytes.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns Attribute size, in __bytes__. 0 on error.
**/
DL_ICEDB size_t ICEDB_fh_getAttrSize(const ICEDB_fs_hnd_p p, const char* name, ICEDB_OUT ICEDB_error_code* err);

/** \brief Determines whether an attribute is of variable length.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns True if the length is variable. False if not or on error.
**/
DL_ICEDB bool ICEDB_fh_getAttrIsVariableSize(const ICEDB_fs_hnd_p p, const char* name, ICEDB_OUT ICEDB_error_code* err);

/** \brief Free the results of a ICEDB_fh_readAttr call
* \param p is a pointer to the ICEDB_attr structure that was populated. Must be non-NULL.
* \see ICEDB_fh_readAttr
**/
DL_ICEDB void ICEDB_fh_freeAttr(ICEDB_attr* attr);

/** \brief Free the results of a ICEDB_fh_readAttr call
* \param p is a pointer to the ICEDB_attr*** structure that was populated. Must be non-NULL.
* \see ICEDB_fh_readAllAttrs
**/
DL_ICEDB void ICEDB_fh_freeAttrList(ICEDB_attr*** const p);

/** \brief Read an attribute, by name.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param name is the attribute name. Must be a NULL-terminated C-string.
* \param err is an error code.
* \returns A pointer to an attribute structure, which must be manually freed when no longer needed. NULL if an error occurred.
* \see ICEDB_fh_freeAttr
**/
DL_ICEDB ICEDB_attr* ICEDB_fh_readAttr(const ICEDB_fs_hnd_p p, const char* name, ICEDB_OUT ICEDB_error_code* err);

/** \brief Read all attributes for a file handle.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param numAtts is filled with the number of attributes that were read.
* \param res is the target pointer that gets populated.
* \param err is an error code.
* \returns A pointer to the attributes structure, which must be manually freed when no longer needed. NULL if an error occurred.
* \see ICEDB_fh_freeAttrList
**/
DL_ICEDB ICEDB_attr *** const ICEDB_fh_readAllAttrs(
	const ICEDB_fs_hnd_p p,
	size_t *numAtts,
	ICEDB_OUT ICEDB_attr *** const res,
	ICEDB_OUT ICEDB_error_code* err);

/** \brief Insert an attribute.
* \param p is the pointer to a file handle. It may be NULL, in which case path must be non-NULL.
* \param newattr is the new attribute.
* \param err is an error code.
* \returns True on success. False if an error occurred.
**/
DL_ICEDB bool ICEDB_fh_insertAttr(ICEDB_fs_hnd_p p, const ICEDB_attr* newattr, ICEDB_OUT ICEDB_error_code* err);

/** @} */ // end of fs

ICEDB_END_DECL_C
#endif
