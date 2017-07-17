#pragma once
#ifndef ICEDB_H_HANDLE
#define ICEDB_H_HANDLE
#include "../defs.h"
#include "../error/error.h"

// Filesystem handles are opaque types. They are implemented using several different backends - 
// for folders, for compressed archives, for netcdf and hdf5 files, et cetera.
// To switch to a different backend, open a new handle pointing to a place along the old path. For example,
// you can open a directory, find a netcdf file, then open it, then find a shape structure, then view 
// its attributes. Actually reading the data (other than basic attributes) is the provenance of other
// classes. Those classes, however, might make use of the handles provided here, and the various fs dlls
// may provide such facilities.
struct ICEDB_FS_HANDLE;
typedef ICEDB_FS_HANDLE* ICEDB_FS_HANDLE_p;
struct ICEDB_FS_HANDLE_INNER; // Used internally for plugins.

enum ICEDB_file_open_flags {
	ICEDB_flags_none = 0,
	ICEDB_flags_create = 1,
	ICEDB_flags_truncate = 2,
	ICEDB_flags_rw = 4,
	ICEDB_flags_readonly = 8
};
enum ICEDB_path_types {
	ICEDB_type_nonexistant,
	ICEDB_type_normal_file,
	ICEDB_type_folder,
	ICEDB_type_symlink
};
enum ICEDB_attr_types {
	ICEDB_attr_type_int16,
	ICEDB_attr_type_uint16,
	ICEDB_attr_type_int32,
	ICEDB_attr_type_uint32,
	ICEDB_attr_type_c_str,
	ICEDB_attr_type_float,
	ICEDB_attr_type_double,
	ICEDB_attr_type_compound,
	ICEDB_attr_type_other
};

DL_ICEDB ICEDB_FS_HANDLE_p ICEDB_file_handle_create(const char* path, const char* ftype, ICEDB_file_open_flags);
DL_ICEDB void ICEDB_file_handle_destroy(ICEDB_FS_HANDLE_p);
DL_ICEDB ICEDB_FS_HANDLE_p ICEDB_file_handle_open_sub(ICEDB_FS_HANDLE_p base, const char* path, const char* ftype, ICEDB_file_open_flags);

DL_ICEDB void ICEDB_file_handle_move(ICEDB_FS_HANDLE_p, const char* src, const char* dest);
DL_ICEDB void ICEDB_file_handle_copy(ICEDB_FS_HANDLE_p, const char* src, const char* dest);
DL_ICEDB void ICEDB_file_handle_unlink(ICEDB_FS_HANDLE_p, const char* path);
DL_ICEDB bool ICEDB_file_handle_create_hard_link(ICEDB_FS_HANDLE_p, const char* src, const char* dest);
DL_ICEDB bool ICEDB_file_handle_create_sym_link(ICEDB_FS_HANDLE_p, const char* src, const char* dest);
DL_ICEDB const char* ICEDB_file_handle_follow_sym_link(ICEDB_FS_HANDLE_p, const char* path);

struct ICEDB_FS_PATH_CONTENTS {
	ICEDB_path_types p_type; /* Type of path - regular, dir, symlink */
	char p_name[65536]; /* path name */
	char p_obj_type[256]; /* Descriptive type of object - hdf5 file, shape, compressed archive, ... */
	ICEDB_FS_HANDLE_p* base_handle; /* Pointer to base container */
	const char* base_path;
	int idx; /* id */
};

DL_ICEDB bool ICEDB_file_handle_path_exists(ICEDB_FS_HANDLE_p, const char* path);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_path_info(ICEDB_FS_HANDLE_p, const char* path, ICEDB_FS_PATH_CONTENTS **res);
// Iterate / enumerate all one-level child objects
DL_ICEDB ICEDB_error_code ICEDB_file_handle_readobjs(struct ICEDB_FS_HANDLE_p, ICEDB_FS_PATH_CONTENTS **res);
DL_ICEDB void ICEDB_file_handle_rewind(struct ICEDB_FS_HANDLE_p);

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
DL_ICEDB ICEDB_error_code ICEDB_file_handle_readobjattrs(struct ICEDB_FS_HANDLE_p, ICEDB_FS_ATTR_CONTENTS **res);
DL_ICEDB void ICEDB_file_handle_attr_rewind(struct ICEDB_FS_HANDLE_p);
DL_ICEDB void ICEDB_file_handle_attr_remove(struct ICEDB_FS_HANDLE_p, const char* name);
DL_ICEDB ICEDB_error_code ICEDB_file_handle_attr_insert(struct ICEDB_FS_HANDLE_p, const char* name,
	const char* data, size_t sz, ICEDB_attr_types type);


// The interface functions for the handle dll interfaces are in a separate header file.

#endif
