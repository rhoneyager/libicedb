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
#include <map>
#include <list>
#include <set>
#include "fs_win.hpp"
#include <windows.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi")

using namespace icedb::plugins::fs_win;
namespace icedb {
	namespace plugins {
		namespace fs_win {
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

			namespace dirpaths {
				const size_t numObjsInPage = 400;
				class c_dirpathspage {
				private:
					ICEDB_FS_PATH_CONTENTS objs[numObjsInPage];
					size_t numUsed;
					size_t cur;
					std::vector<bool> used;
				public:
					c_dirpathspage() : numUsed(0), cur(0) { used.resize(numObjsInPage, false); }
					~c_dirpathspage() {}
					size_t getNumFree() const { return numObjsInPage - numUsed; }
					inline bool empty() const { return (numUsed == 0) ? true : false; }
					inline bool full() const { return (numUsed == numObjsInPage) ? true : false; }
					inline bool isObjOwner(ICEDB_FS_PATH_CONTENTS *p) const {
						if ((p >= objs) && (p < objs + numObjsInPage)) return true;
						return false;
					}
					void findNextCur() {
						if (numUsed >= numObjsInPage) return;
						while (used[cur]) {
							cur++;
							if (cur >= numObjsInPage) cur = 0;
						}
					}
					ICEDB_FS_PATH_CONTENTS* pop() {
						if (!getNumFree()) return nullptr;
						ICEDB_FS_PATH_CONTENTS* res = objs + cur;
						used[cur] = true;
						numUsed++;
						findNextCur();
						return res;
					}
					void release(ICEDB_FS_PATH_CONTENTS* p) {
						size_t cur = (p - objs) / sizeof(ICEDB_FS_PATH_CONTENTS);
						used[cur] = false;
						numUsed--;
						findNextCur();
					}
				};
				class c_dirpaths {
				private:
					std::list<c_dirpathspage> pages;
					void compact() {
						pages.remove_if([](c_dirpathspage &p) {
							return p.empty();
						});
					}
					void addPage() {
						pages.push_back(c_dirpathspage());
						rt = pages.rbegin();
					}
					std::list<c_dirpathspage>::reverse_iterator rt;
					size_t releaseCounter;
				public:
					c_dirpaths() : releaseCounter(0) {}
					~c_dirpaths() {}
					ICEDB_FS_PATH_CONTENTS* pop() {
						if (!pages.size()) addPage();
						if (pages.rbegin()->full()) addPage();
						return pages.rbegin()->pop();
					}
					void release(ICEDB_FS_PATH_CONTENTS* p) {
						if (rt != pages.rend()) {
							if (rt->isObjOwner(p)) {
								rt->release(p);
								return;
							}
						}
						for (rt = pages.rbegin(); rt != pages.rend(); ++rt) {
							if (rt->isObjOwner(p)) {
								rt->release(p);
								break;
							}
						}
						releaseCounter++;
						if (releaseCounter > 10 * numObjsInPage) {
							compact();
							releaseCounter = 0;
						}
					}
				} obj_c_dirpaths;
				std::map<ICEDB_FS_PATH_CONTENTS*, std::shared_ptr<std::vector<ICEDB_FS_PATH_CONTENTS*> > > returned_paths;
			}
		}
	}
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
		if (p->open_flags & ICEDB_file_open_flags::ICEDB_flags_readonly) {
			ICEDB_error_context* err = i_error_context->error_context_create_impl(
				i_error_context.get(), ICEDB_ERRORCODES_READONLY, __FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
			return ICEDB_ERRORCODES_READONLY;
		}
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
		if (p->open_flags & ICEDB_file_open_flags::ICEDB_flags_readonly) {
			ICEDB_error_context* err = i_error_context->error_context_create_impl(
				i_error_context.get(), ICEDB_ERRORCODES_READONLY, __FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
			return ICEDB_ERRORCODES_READONLY;
		}
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
		if (p->open_flags & ICEDB_file_open_flags::ICEDB_flags_readonly) {
			ICEDB_error_context* err = i_error_context->error_context_create_impl(
				i_error_context.get(), ICEDB_ERRORCODES_READONLY, __FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
			return ICEDB_ERRORCODES_READONLY;
		}
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
		const wchar_t* from, ICEDB_FS_PATH_CONTENTS*** res) {
		if (!p || !res) hnd->_vtable->_raiseExcept(hnd,
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

		std::shared_ptr<std::vector<ICEDB_FS_PATH_CONTENTS*> > children(new std::vector<ICEDB_FS_PATH_CONTENTS*>);
		children->reserve(1000);
		do
		{
			ICEDB_FS_PATH_CONTENTS *child = dirpaths::obj_c_dirpaths.pop();
			child->base_handle = p;
			StrCpyNW(child->base_path, p->h->cwd.c_str(), ICEDB_FS_PATH_CONTENTS_PATH_MAX);
			child->idx = 0;
			StrCpyNW(child->p_name, ffd.cFileName, ICEDB_FS_PATH_CONTENTS_PATH_MAX);
			
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				child->p_type = ICEDB_path_types::ICEDB_type_folder;
				errno_t e = strncpy_s(child->p_obj_type, ICEDB_FS_PATH_CONTENTS_PATH_MAX, "folder", 7);
				if (e) { GenerateWinOSerror(); return ICEDB_ERRORCODES_OS; }
			} else if (ffd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
				child->p_type = ICEDB_path_types::ICEDB_type_symlink;
				errno_t e = strncpy_s(child->p_obj_type, ICEDB_FS_PATH_CONTENTS_PATH_MAX, "symlink", 8);
				if (e) { GenerateWinOSerror(); return ICEDB_ERRORCODES_OS; }
			} else {
				child->p_type = ICEDB_path_types::ICEDB_type_normal_file;
				errno_t e = strncpy_s(child->p_obj_type, ICEDB_FS_PATH_CONTENTS_PATH_MAX, "file", 5);
				if (e) { GenerateWinOSerror(); return ICEDB_ERRORCODES_OS; }
			}
			//child->next = nullptr;
			children->push_back(child);
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

		// Assemble and store a vector that points to the appropriate file paths. Modify the pointers into a linked list.
		if (children->size()) {
			//children
			dirpaths::returned_paths[children->at(0)] = children;
			*res = children->data();
		} else {
			*res = nullptr;
		}

		return ICEDB_ERRORCODES_NONE;
	}

	SHARED_EXPORT_ICEDB ICEDB_error_code fs_free_objs(ICEDB_FS_HANDLE_p p, ICEDB_FS_PATH_CONTENTS** pc) {
		if (!isValidHandle(p))hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		if (!pc) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);

		if (dirpaths::returned_paths[*pc]) {
			auto v = dirpaths::returned_paths[*pc];
			for (auto &i : *v) dirpaths::obj_c_dirpaths.release(i);
			dirpaths::returned_paths.erase(*pc);
		} else {
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		}
		return ICEDB_ERRORCODES_NONE;
	}


	SHARED_EXPORT_ICEDB size_t fs_can_open_path(ICEDB_FS_HANDLE_p, const wchar_t* p, const char* t, ICEDB_file_open_flags flags) {
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

	SHARED_EXPORT_ICEDB ICEDB_FS_HANDLE_p fs_open_path(
		ICEDB_FS_HANDLE_p p, const wchar_t* path, const char* typ, ICEDB_file_open_flags flags) {
		// Does the path exist? Consult flags.
		// Implicitly checks that p is a valid handle of the same plugin, if it is defined.
		bool exists = fs_path_exists(p, path);
		std::wstring spath;
		if (!exists && ((flags & ICEDB_file_open_flags::ICEDB_flags_create) || (flags & ICEDB_file_open_flags::ICEDB_flags_truncate))) {
			// Check if this is a folder or a file by looking at the last character of the path.
			bool isFld = false;
			if (spath.crbegin() != spath.crend()) {
				wchar_t c = *(spath.crbegin());
				if (c == '\\' || c == '/') isFld = true;
			}
			std::string styp = (typ) ? std::string(typ) : "";
			if (styp == "folder") isFld = true;
			// We can create folders. No knowledge of how to create files.
			if (!isFld) {
				ICEDB_error_context* err = i_error_context->error_context_create_impl(
					i_error_context.get(), ICEDB_ERRORCODES_UNIMPLEMENTED, __FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
				return nullptr;
			} else {
				std::wstring effpath = makeEffPath(p, path);
				bool res = CreateDirectoryW(effpath.c_str(), NULL);
				if (!res) GenerateWinOSerror();
				// Create the handle to the new folder.
				ICEDB_FS_HANDLE_p h = makeHandle();
				h->open_flags = flags;
				h->h->cwd = effpath;
				return h;
			}
		} else if (exists && (flags & ICEDB_file_open_flags::ICEDB_flags_truncate)) {
			// Not supported. No desire yet to truncate a folder.
			ICEDB_error_context* err = i_error_context->error_context_create_impl(
				i_error_context.get(), ICEDB_ERRORCODES_UNIMPLEMENTED, __FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
			return nullptr;
		} else if (exists && (
			(flags & ICEDB_file_open_flags::ICEDB_flags_rw) || (flags & ICEDB_file_open_flags::ICEDB_flags_none)
			|| (flags & ICEDB_file_open_flags::ICEDB_flags_readonly))) {
			std::wstring effpath = makeEffPath(p, path);
			// Create the handle to the new folder.
			ICEDB_FS_HANDLE_p h = makeHandle();
			h->open_flags = flags;
			h->h->cwd = effpath;
			return h;
		}

		return nullptr;
	}
}