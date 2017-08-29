#pragma once
#ifndef ICEDB_H_FS_HANDLE
#define ICEDB_H_FS_HANDLE
#include "../defs.h"
#include "../error/error.h"

ICEDB_BEGIN_DECL_C

// Filesystem handles are opaque types. They are implemented using several different backends - 
// for folders, for compressed archives, for netcdf and hdf5 files, et cetera.
// To switch to a different backend, open a new handle pointing to a place along the old path. For example,
// you can open a directory, find a netcdf file, then open it, then find a shape structure, then view 
// its attributes. Actually reading the data (other than basic attributes) is the provenance of other
// classes. Those classes, however, might make use of the handles provided here, and the various fs dlls
// may provide such facilities.
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

/// Retreive all plugin handlers in a given registry
DL_ICEDB void ICEDB_fh_getHandlers(const char* registry, ICEDB_OUT size_t* const numPlugins, ICEDB_OUT char *** const pluginids);

/// Can a path be opened?
DL_ICEDB bool ICEDB_path_canOpen(
	const char* path,
	ICEDB_OPTIONAL const char* ftype,
	ICEDB_OPTIONAL const char* pluginid,
	ICEDB_OPTIONAL ICEDB_fs_hnd_p base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags,
	ICEDB_OUT size_t* const numHandlersThatCanOpen,
	ICEDB_OPTIONAL ICEDB_OUT char *** const pluginids,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code*);

/// Open a path and get a handle.
DL_ICEDB ICEDB_fs_hnd_p ICEDB_path_open(
	const char* path, ICEDB_OPTIONAL const char* ftype,
	ICEDB_OPTIONAL const char* pluginid, ICEDB_OPTIONAL ICEDB_fs_hnd_p base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code*);

/// Get the full path opened by the file handle.
DL_ICEDB const char* ICEDB_fh_get_name(ICEDB_fs_hnd_p, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code*);

/// Get the ICEDB_file_open_flags passed to the plugin when the handle was opened
DL_ICEDB ICEDB_file_open_flags ICEDB_fh_getOpenFlags(ICEDB_fs_hnd_p, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code*);

/// Close a file handle
DL_ICEDB bool ICEDB_fh_close(ICEDB_fs_hnd_p, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code*);

DL_ICEDB ICEDB_error_code ICEDB_fh_move(ICEDB_fs_hnd_p, const char* src, const char* dest);
DL_ICEDB ICEDB_error_code ICEDB_fh_copy(ICEDB_fs_hnd_p, const char* src, const char* dest, bool overwrite);
DL_ICEDB ICEDB_error_code ICEDB_fh_unlink(ICEDB_fs_hnd_p, const char* path);
DL_ICEDB ICEDB_error_code ICEDB_fh_create_hard_link(ICEDB_fs_hnd_p, const char* src, const char* dest);
DL_ICEDB ICEDB_error_code ICEDB_fh_create_sym_link(ICEDB_fs_hnd_p, const char* src, const char* dest);
DL_ICEDB ICEDB_error_code ICEDB_fh_follow_sym_link(ICEDB_fs_hnd_p,
	const char* path, size_t out_max_sz, size_t* szout, char** out);

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

enum ICEDB_attr_type {
	ICEDB_type_char, // NC_CHAR
	ICEDB_type_int8, // NC_BYTE
	ICEDB_type_uint8, // NC_UBYTE
	ICEDB_type_uint16, // NC_USHORT
	ICEDB_type_int16, // NC_SHORT
	ICEDB_type_uint32, // NC_UINT
	ICEDB_type_int32, // NC_INT (or NC_LONG)
	ICEDB_type_uint64, // NC_UINT64
	ICEDB_type_int64, // NC_INT64
	ICEDB_type_float, // NC_FLOAT
	ICEDB_type_double, // NC_DOUBLE
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



ICEDB_END_DECL_C
#endif
