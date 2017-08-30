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
 **/
struct ICEDB_fs_hnd;
typedef ICEDB_fs_hnd* ICEDB_fs_hnd_p;

enum ICEDB_file_open_flags {
	ICEDB_flags_none = 0,
	ICEDB_flags_create = 1,
	ICEDB_flags_truncate = 2,
	ICEDB_flags_rw = 4,
	ICEDB_flags_readonly = 8
};
enum ICEDB_path_types {
	ICEDB_type_nonexistant,
	ICEDB_type_unknown,
	ICEDB_type_normal_file,
	ICEDB_type_folder,
	ICEDB_type_symlink
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

#define ICEDB_FS_PATH_CONTENTS_PATH_MAX 32767
struct ICEDB_FS_PATH_CONTENTS {
	int idx; /* id */
	ICEDB_path_types p_type; /* Type of path - regular, dir, symlink */
	char *p_name; /* path name */
	char *p_obj_type; /* Descriptive type of object - hdf5 file, shape, compressed archive, ... */
	char *base_path;
};
DL_ICEDB bool ICEDB_FS_PATH_CONTENTS_alloc(ICEDB_FS_PATH_CONTENTS*);
DL_ICEDB bool ICEDB_FS_PATH_CONTENTS_free(ICEDB_FS_PATH_CONTENTS*);

DL_ICEDB bool ICEDB_fh_path_exists(ICEDB_fs_hnd_p, const char* path);
DL_ICEDB ICEDB_error_code ICEDB_fh_path_info(ICEDB_fs_hnd_p, const char* path, ICEDB_FS_PATH_CONTENTS *res);
// Iterate / enumerate all one-level child objects
DL_ICEDB ICEDB_error_code ICEDB_fh_readobjs(ICEDB_fs_hnd_p, const char* path, size_t *numObjs, ICEDB_FS_PATH_CONTENTS ***res);
DL_ICEDB ICEDB_error_code ICEDB_fh_free_objs(ICEDB_fs_hnd_p, ICEDB_FS_PATH_CONTENTS **);


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


struct ICEDB_ATTR {
	ICEDB_attr_data data;
	ICEDB_attr_type type;
	size_t size;
	bool hasSize;
	const char* name;
};

DL_ICEDB size_t ICEDB_fh_getNumAttrs(const ICEDB_fs_hnd_p, ICEDB_OUT ICEDB_error_code*);
DL_ICEDB const char* ICEDB_fh_getAttrName(const ICEDB_fs_hnd_p, size_t attrnum, ICEDB_OUT ICEDB_error_code*);
DL_ICEDB bool ICEDB_fh_attrExists(const ICEDB_fs_hnd_p, const char* name, ICEDB_OUT ICEDB_error_code*);
DL_ICEDB void ICEDB_fh_removeAttr(ICEDB_fs_hnd_p, const char* name, ICEDB_OUT ICEDB_error_code*);
DL_ICEDB void ICEDB_fh_renameAttr(ICEDB_fs_hnd_p, const char* oldname, const char* newname, ICEDB_OUT ICEDB_error_code*);

DL_ICEDB ICEDB_attr_type ICEDB_fh_getAttrType(const ICEDB_fs_hnd_p, const char* name, ICEDB_OUT ICEDB_error_code*);
DL_ICEDB size_t ICEDB_fh_getAttrSize(const ICEDB_fs_hnd_p, const char* name, ICEDB_OUT ICEDB_error_code*);

DL_ICEDB void ICEDB_fh_freeAttr(const ICEDB_fs_hnd_p, ICEDB_ATTR* attr, ICEDB_OUT ICEDB_error_code*);
DL_ICEDB ICEDB_ATTR* ICEDB_fh_readAttr(const ICEDB_fs_hnd_p, const char* name, ICEDB_OUT ICEDB_error_code*);
DL_ICEDB void ICEDB_fh_insertAttr(ICEDB_fs_hnd_p, const ICEDB_ATTR* in, ICEDB_OUT ICEDB_error_code*);

/** @} */ // end of fs

ICEDB_END_DECL_C
#endif
