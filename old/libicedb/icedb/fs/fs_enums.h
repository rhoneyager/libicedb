#pragma once
#ifndef ICEDB_H_FS_ENUMS
#define ICEDB_H_FS_ENUMS

/// The maximum length of a path.
#define ICEDB_FS_PATH_CONTENTS_PATH_MAX 32767

/// These flags are used when opening a path. They are mutually exclusive.
enum ICEDB_file_open_flags {
	ICEDB_flags_invalid = 0, ///< This should only be returned by a function to indicate an error. Never use otherwise.
	ICEDB_flags_none = 1, ///< No special options. Open path for read/write. If it does not exist, create it.
	ICEDB_flags_create = 2, ///< Create a new path. Fails if a path already exists. Read-write is implied.
	ICEDB_flags_truncate = 4, ///< Create a new path, replacing an old path if it already exists. Read-write is implied.
	ICEDB_flags_rw = 8, ///< Open an existing path for read-write access. Path must already exist.
	ICEDB_flags_readonly = 16 ///< Open an existing path for read-only access. Path must already exist.
};

/// These indicate the type of a path. This is, is the path a folder, a symbolic link, a regular file...
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

#endif