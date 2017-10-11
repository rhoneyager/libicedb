#pragma once
#ifndef ICEDB_H_FS_PATH_FUNCS
#define ICEDB_H_FS_PATH_FUNCS

/** \brief A structure that provides information about a path.
*
* This structure is passed back by either ICEDB_fh_path_info or ICEDB_fh_readObjs.
* It lists an object's name, type, base path, and whether it is a regular file, a directory or a symbolic link.
**/
struct ICEDB_fs_path_contents {
	int idx; ///< id. A number for each distinct object in a path search. Should be same only for hardlinks.
	ICEDB_path_types p_type; ///< Type of path - regular, dir, symlink
	char *p_name; ///< path name
	char *p_obj_type; ///< Descriptive type of object - hdf5 file, shape, compressed archive, ...
	char *base_path; ///< The base path of an object
};

#endif
