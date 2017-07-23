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
struct ICEDB_FS_HANDLE;
typedef ICEDB_FS_HANDLE* ICEDB_FS_HANDLE_p;

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
enum ICEDB_attr_types {
	ICEDB_attr_type_int16,
	ICEDB_attr_type_uint16,
	ICEDB_attr_type_int32,
	ICEDB_attr_type_uint32,
	ICEDB_attr_type_int64,
	ICEDB_attr_type_uint64,
	ICEDB_attr_type_c_str,
	ICEDB_attr_type_float,
	ICEDB_attr_type_double,
	ICEDB_attr_type_compound,
	ICEDB_attr_type_other
};

DL_ICEDB ICEDB_FS_HANDLE_p ICEDB_file_handle_create(
	const wchar_t* path, const char* ftype, ICEDB_file_open_flags flags);
DL_ICEDB void ICEDB_file_handle_destroy(ICEDB_FS_HANDLE_p);
//DL_ICEDB ICEDB_FS_HANDLE_p ICEDB_file_handle_open_sub(
//	ICEDB_FS_HANDLE_p base, const char* path, const char* ftype, ICEDB_file_open_flags);

DL_ICEDB ICEDB_error_code ICEDB_file_handle_move(ICEDB_FS_HANDLE_p, const wchar_t* src, const wchar_t* dest);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_copy(ICEDB_FS_HANDLE_p, const wchar_t* src, const wchar_t* dest, bool overwrite);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_unlink(ICEDB_FS_HANDLE_p, const wchar_t* path);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_create_hard_link(ICEDB_FS_HANDLE_p, const wchar_t* src, const wchar_t* dest);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_create_sym_link(ICEDB_FS_HANDLE_p, const wchar_t* src, const wchar_t* dest);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_follow_sym_link(ICEDB_FS_HANDLE_p,
	const wchar_t* path, size_t out_max_sz, size_t* szout, wchar_t** out);

#define ICEDB_FS_PATH_CONTENTS_PATH_MAX 32767
struct ICEDB_FS_PATH_CONTENTS {
	ICEDB_path_types p_type; /* Type of path - regular, dir, symlink */
	wchar_t p_name[ICEDB_FS_PATH_CONTENTS_PATH_MAX]; /* path name */
	char p_obj_type[ICEDB_FS_PATH_CONTENTS_PATH_MAX]; /* Descriptive type of object - hdf5 file, shape, compressed archive, ... */
	ICEDB_FS_HANDLE_p base_handle; /* Pointer to base container */
	wchar_t base_path[ICEDB_FS_PATH_CONTENTS_PATH_MAX];
	int idx; /* id */
};

DL_ICEDB bool ICEDB_file_handle_path_exists(ICEDB_FS_HANDLE_p, const wchar_t* path);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_path_info(ICEDB_FS_HANDLE_p, const wchar_t* path, ICEDB_FS_PATH_CONTENTS *res);
// Iterate / enumerate all one-level child objects
DL_ICEDB ICEDB_error_code ICEDB_file_handle_readobjs(ICEDB_FS_HANDLE_p, ICEDB_FS_PATH_CONTENTS **res);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_rewind(ICEDB_FS_HANDLE_p);

// There are a few special subpaths / extended attributes (xattrs) / alternate data streams that may exist
// depending on the path type and underlying file system. Attributes exist in hdf5 and netcdf files, too.
struct ICEDB_FS_ATTR_CONTENTS {
	ICEDB_FS_HANDLE_p* base_handle;
	char a_name[256];
	union { // Endianness? - Only using char (1-byte) arrays for now.
		// Atts are small and converions should not pose a problem.
		char c[256];
		//int16_t i16[128];
		//int32_t i32[64];
		//uint16_t u16[128];
		//uint32_t u32[64];
		//float f[8];
		//double d[4];
	} a_val;
	size_t a_size;
	int err_attr; // Nonzero if the attribute cannot be read by the plugin. Usually when too long.
	int idx;
	ICEDB_attr_types a_type;
};
DL_ICEDB ICEDB_error_code ICEDB_file_handle_readobjattrs(ICEDB_FS_HANDLE_p, ICEDB_FS_ATTR_CONTENTS **res);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_attr_rewind(ICEDB_FS_HANDLE_p);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_attr_remove(ICEDB_FS_HANDLE_p, const char* name);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_attr_insert(ICEDB_FS_HANDLE_p, const char* name,
	const char* data, size_t sz, ICEDB_attr_types type);

ICEDB_END_DECL_C
#endif
