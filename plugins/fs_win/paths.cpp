#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/fs/fs.h"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/error/error_context.h"
#include "../../libicedb/icedb/error/error_contextInterface.hpp"
#include "../../libicedb/icedb/error/error.h"
#include "../../libicedb/icedb/error/errorInterface.hpp"
#include "../../libicedb/icedb/error/errorCodes.h"
#include "../../libicedb/icedb/misc/util.h"
#include <string>
#include <cwchar>
#include <set>
#include "fs_win.hpp"
#include <windows.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi")

using namespace icedb::plugins::fs_win;
std::wstring makeEffPath(ICEDB_FS_HANDLE_p p, const wchar_t* path) {
	std::wstring effpath;
	if (p) {
		if (!isValidHandle(p))
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		effpath = p->h->cwd;
		effpath.append(L"\\");
	}
	if (path)
		effpath.append(path);
	if (!effpath.length())
		hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
	return effpath;
}
void GenerateWinOSerror(DWORD winerrnum) {
	if (!winerrnum) winerrnum = GetLastError();
	ICEDB_error_context* err = i_error_context->error_context_create_impl(
		i_error_context.get(), ICEDB_ERRORCODES_OS, __FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
	const int errStrSz = 250;
	char winErrString[errStrSz] = "";
	snprintf(winErrString, errStrSz, "%u", winerrnum);
	i_error_context->error_context_add_string2(i_error_context.get(), err, "Win-Error-Code", winErrString);
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, winerrnum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), winErrString, errStrSz, NULL);
	i_error_context->error_context_add_string2(i_error_context.get(), err, "Win-Error-String", winErrString);
}
extern "C" {
	
	SHARED_EXPORT_ICEDB bool fs_path_exists(ICEDB_FS_HANDLE_p p, const wchar_t* path) {
		// The base filesystem plugins do not require the path handle. If it is not specified, then
		// just assube that the base directory is specified in path. If it is specified, then 
		// prepend p's base directory to path.
		std::wstring effpath = makeEffPath(p, path);
		bool res = PathFileExistsW(effpath.data());
		return res;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_path_info(ICEDB_FS_HANDLE_p p, const wchar_t* path, ICEDB_FS_PATH_CONTENTS* data) {
		std::wstring effpath = makeEffPath(p, path);
		if (!data) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		
		data->base_handle = p;
		wcsncpy_s(data->base_path, p->h->cwd.data(), ICEDB_FS_PATH_CONTENTS_PATH_MAX);
		data->idx = -1;
		if (path)
			wcsncpy_s(data->p_name, ICEDB_FS_PATH_CONTENTS_PATH_MAX, path, ICEDB_FS_PATH_CONTENTS_PATH_MAX);
		else wcsncpy_s(data->p_name, ICEDB_FS_PATH_CONTENTS_PATH_MAX, L"", ICEDB_FS_PATH_CONTENTS_PATH_MAX);

		data->p_type = ICEDB_path_types::ICEDB_type_unknown;
		if (fs_path_exists(p, path)) {
			DWORD winatts = GetFileAttributesW(effpath.data());
			if (FILE_ATTRIBUTE_DIRECTORY & winatts) {
				data->p_type = ICEDB_path_types::ICEDB_type_folder;
				strncpy_s(data->p_obj_type, ICEDB_FS_PATH_CONTENTS_PATH_MAX, "folder", ICEDB_FS_PATH_CONTENTS_PATH_MAX);
			} else if (FILE_ATTRIBUTE_REPARSE_POINT & winatts) {
				data->p_type = ICEDB_path_types::ICEDB_type_symlink;
				strncpy_s(data->p_obj_type, ICEDB_FS_PATH_CONTENTS_PATH_MAX, "symlink", ICEDB_FS_PATH_CONTENTS_PATH_MAX);
			}else {
				data->p_type = ICEDB_path_types::ICEDB_type_normal_file;
				strncpy_s(data->p_obj_type, ICEDB_FS_PATH_CONTENTS_PATH_MAX, "file", ICEDB_FS_PATH_CONTENTS_PATH_MAX);
			}
		} else {
			data->p_type = ICEDB_path_types::ICEDB_type_nonexistant;
			strncpy_s(data->p_obj_type, ICEDB_FS_PATH_CONTENTS_PATH_MAX, "", ICEDB_FS_PATH_CONTENTS_PATH_MAX);
		}
		return 0;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_copy(ICEDB_FS_HANDLE_p p,
		const wchar_t* from, const wchar_t* to, bool overwrite) {
		std::wstring effpathFrom = makeEffPath(p, from);
		std::wstring effpathTo = makeEffPath(p, to);

		bool opres = CopyFileW(effpathFrom.data(), effpathTo.data(), !overwrite);
		if (!opres) {
			GenerateWinOSerror();
			return ICEDB_ERRORCODES_OS;
		}
		return ICEDB_ERRORCODES_NONE;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_move(ICEDB_FS_HANDLE_p p,
		const wchar_t* from, const wchar_t* to, bool overwrite) {
		std::wstring effpathFrom = makeEffPath(p, from);
		std::wstring effpathTo = makeEffPath(p, to);
		DWORD flags = (overwrite) ? MOVEFILE_REPLACE_EXISTING : 0;

		bool opres = MoveFileExW(effpathFrom.data(), effpathTo.data(), flags);
		if (!opres) {
			GenerateWinOSerror();
			return ICEDB_ERRORCODES_OS;
		}
		return ICEDB_ERRORCODES_NONE;
	}
	
	SHARED_EXPORT_ICEDB ICEDB_error_code fs_unlink(ICEDB_FS_HANDLE_p p, const wchar_t* path) {
		std::wstring effpathFrom = makeEffPath(p, path);
		bool opres = DeleteFileW(effpathFrom.data());
		if (!opres) {
			GenerateWinOSerror();
			return ICEDB_ERRORCODES_OS;
		}
		return ICEDB_ERRORCODES_NONE;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_create_sym_link(ICEDB_FS_HANDLE_p,
		const wchar_t*, const wchar_t*) {
		// Always throws, since symlinks are not supported on this fs.
		hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		return ICEDB_ERRORCODES_OS;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_follow_sym_link(ICEDB_FS_HANDLE_p,
		const wchar_t*, size_t, size_t*, wchar_t**) {
		// Always throws, since symlinks are not supported on this fs.
		hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		return ICEDB_ERRORCODES_OS;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_create_hard_link(ICEDB_FS_HANDLE_p p,
		const wchar_t* from, const wchar_t* to) {
		std::wstring effpathFrom = makeEffPath(p, from);
		std::wstring effpathTo = makeEffPath(p, to);

		bool opres = CreateHardLinkW(effpathTo.data(), effpathFrom.data(), NULL);
		if (!opres) {
			DWORD winerrnum = GetLastError();
			ICEDB_error_context* err = i_error_context->error_context_create_impl(
				i_error_context.get(), ICEDB_ERRORCODES_OS, __FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
			const int errStrSz = 250;
			char winErrString[errStrSz] = "";
			snprintf(winErrString, errStrSz, "%u", winerrnum);
			i_error_context->error_context_add_string2(i_error_context.get(), err, "Win-Error-Code", winErrString);
			FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, winerrnum,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), winErrString, errStrSz, NULL);
			i_error_context->error_context_add_string2(i_error_context.get(), err, "Win-Error-String", winErrString);

			return ICEDB_ERRORCODES_OS;
		}
		return ICEDB_ERRORCODES_NONE;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_readobjattrs(ICEDB_FS_HANDLE_p, ICEDB_FS_ATTR_CONTENTS**) {
		// Always throws, since attributes are not supported on this plugin.
		hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		return ICEDB_ERRORCODES_OS;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_free_objattrs(ICEDB_FS_HANDLE_p) {
		// Always throws, since attributes are not supported on this plugin.
		hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		return ICEDB_ERRORCODES_OS;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_attr_remove(ICEDB_FS_HANDLE_p, const char*) {
		// Always throws, since attributes are not supported on this plugin.
		hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		return ICEDB_ERRORCODES_OS;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_attr_insert(ICEDB_FS_HANDLE_p, 
		const char*, const char*, size_t, ICEDB_attr_types) {
		// Always throws, since attributes are not supported on this plugin.
		hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		return ICEDB_ERRORCODES_OS;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_readobjs(ICEDB_FS_HANDLE_p p,
		const wchar_t* from, ICEDB_FS_PATH_CONTENTS** res) {
		if (!p) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		std::wstring effpath = makeEffPath(p, from);
		if (!PathFileExistsW(effpath.data())) {
			ICEDB_error_context* err = i_error_context->error_context_create_impl(
				i_error_context.get(), ICEDB_ERRORCODES_NONEXISTENT_PATH, __FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);

			{
				size_t origsize = wcslen(effpath.data()) + 1;

				const size_t newsize = origsize * 4;
				size_t convertedChars = 0;
				std::unique_ptr<char[]> nstring(new char[newsize]);
				wcstombs_s(&convertedChars, nstring.get(), origsize, effpath.data(), _TRUNCATE);
				// Destination string was always null-terminated!
				std::string res(nstring.get());
				i_error_context->error_context_add_string2(i_error_context.get(), err, "Path", res.c_str());
			}

			return ICEDB_ERRORCODES_NONEXISTENT_PATH;
		}
		DWORD winatts = GetFileAttributesW(effpath.data());
		if (FILE_ATTRIBUTE_DIRECTORY & winatts)
			effpath.append(L"\\*");
		WIN32_FIND_DATAW ffd;
		HANDLE hFind = FindFirstFileW(effpath.data(), &ffd);
		if (INVALID_HANDLE_VALUE == hFind)
		{
			GenerateWinOSerror();
			return ICEDB_ERRORCODES_OS;
		}

		//std::set<ICEDB_FS_PATH_CONTENTS> children;
		do
		{
			// Allocate a bunch of these, and put them in a vector.
			// The vector gets staticly allocated - it persists. We create a structure of the right size and
			// then copy the pointers over. Also need to update the deallocation function to release
			// the allocated vector.
			ICEDB_FS_PATH_CONTENTS child;
			child.base_handle;
			child.base_path;
			child.idx = 0;
			child.p_name;
			child.p_obj_type;
			child.p_type;
			child.next = nullptr;
			//struct ICEDB_FS_PATH_CONTENTS {
			//	ICEDB_path_types p_type; /* Type of path - regular, dir, symlink */
			//	wchar_t p_name[ICEDB_FS_PATH_CONTENTS_PATH_MAX]; /* path name */
			//	char p_obj_type[ICEDB_FS_PATH_CONTENTS_PATH_MAX]; /* Descriptive type of object - hdf5 file, shape, compressed archive, ... */
			//	ICEDB_FS_HANDLE_p base_handle; /* Pointer to base container */
			//	wchar_t base_path[ICEDB_FS_PATH_CONTENTS_PATH_MAX];
			//	int idx; /* id */
			//};
			//children.emplace(std::wstring(ffd.cFileName));
			//if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		} while (FindNextFileW(hFind, &ffd) != 0);
		DWORD winerrnum = GetLastError();
		if (winerrnum != ERROR_NO_MORE_FILES)
		{
			GenerateWinOSerror(winerrnum);
			return ICEDB_ERRORCODES_OS;
		}
		if (!FindClose(hFind)) {
			GenerateWinOSerror();
			return ICEDB_ERRORCODES_OS;
		}
		return dwError;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_free_objs(ICEDB_FS_HANDLE_p p, ICEDB_FS_PATH_CONTENTS** pc) {
		if (!isValidHandle(p))hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		if (!pc) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		ICEDB_FS_PATH_CONTENTS *it = *pc;
		while (it) {
			ICEDB_FS_PATH_CONTENTS *ot = it;
			++it;
			delete ot;
		}
		delete pc;
	}


	SHARED_EXPORT_ICEDB size_t fs_can_open_path(const wchar_t* p, const char* t, ICEDB_file_open_flags flags) {
		// Can open directories and any file.
		// Overall priority is quite low, as more specialized plugins are more useful here. Still,
		// this can be used for viewing directory contents and querying file metadata / attributes.
		const size_t valid_pri = 10;

		if (!fs_path_exists(nullptr, p)) return 0;
		ICEDB_FS_PATH_CONTENTS finfo;
		fs_path_info(nullptr, p, &finfo);

		// This plugin currently does not resolve symbolic links.]
		// If file type is a file or a directory, then return valid_pri.
		if (finfo.p_type == ICEDB_path_types::ICEDB_type_folder) return valid_pri;
		if (finfo.p_type == ICEDB_path_types::ICEDB_type_normal_file) return valid_pri;

		return 0;
	}
}