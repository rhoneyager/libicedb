#include "../icedb/fs/fs.h"
#include "../icedb/fs/fs_backend.hpp"
#include "../icedb/fs/fs_dll_impl.hpp"
#include "../icedb/dlls/dllsImpl.hpp"
#include "../icedb/dlls/dlls.hpp"
#include "../icedb/dlls/dlls.h"
#include "../icedb/dlls/linking.h"
#include <string>
#include <vector>

namespace icedb {
	namespace fs {
		namespace impl {
			const int64_t magic = 234987234973820484;
			void verify_pointer_fs_p(ICEDB_FS_HANDLE_p p) {
				if (!p) ICEDB_DEBUG_RAISE_EXCEPTION();
				//if (p->magic != icedb::fs::impl::magic)ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->h) ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->i) ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->d) ICEDB_DEBUG_RAISE_EXCEPTION();
				//if (!p->h_dest) ICEDB_DEBUG_RAISE_EXCEPTION();
			}
			bool has_valid_fs_interface(interface_ICEDB_fs_plugin *i) {
				if (!i) ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!i->Bind_get_capabilities(i)) return false;
				if (!i->Bind_get_global_property(i)) return false;
				if (!i->Bind_set_global_property(i)) return false;
				if (!i->Bind_get_property(i)) return false;
				if (!i->Bind_set_property(i)) return false;
				if (!i->Bind_attr_insert(i)) return false;
				if (!i->Bind_attr_remove(i)) return false;
				if (!i->Bind_free_objattrs(i)) return false;
				if (!i->Bind_can_open_path(i)) return false;
				if (!i->Bind_copy(i)) return false;
				if (!i->Bind_create_hard_link(i)) return false;
				if (!i->Bind_create_sym_link(i)) return false;
				if (!i->Bind_destroy(i)) return false;
				if (!i->Bind_follow_sym_link(i)) return false;
				if (!i->Bind_get_open_flags(i)) return false;
				if (!i->Bind_move(i)) return false;
				if (!i->Bind_open_path(i)) return false;
				if (!i->Bind_path_exists(i)) return false;
				if (!i->Bind_path_info(i)) return false;
				if (!i->Bind_readobjattrs(i)) return false;
				if (!i->Bind_readobjs(i)) return false;
				if (!i->Bind_free_objs(i)) return false;
				if (!i->Bind_unlink(i)) return false;
				return true;
			}
		}
	}
}
ICEDB_BEGIN_DECL_C

using namespace icedb::fs::impl;

void ICEDB_file_handle_destroy(ICEDB_FS_HANDLE_p p) {
	verify_pointer_fs_p(p);
	p->h = nullptr; // h's destructor depends on h_dest's existence
	//p->h_dest = nullptr;
	p->i = nullptr;
	p->d = nullptr;
	delete p;
}

ICEDB_FS_HANDLE_p ICEDB_file_handle_create
(const char* path, const char* ftype, ICEDB_file_open_flags flags) {
	{
		ICEDB_FS_HANDLE_p res = nullptr;
		//res->h_dest = nullptr;

		std::vector<std::string> dlls = icedb::dll::query_interface("fs");
		for (const auto &f : dlls) {
			std::shared_ptr<ICEDB_DLL_BASE_HANDLE> dhnd(
				ICEDB_DLL_BASE_HANDLE_create(f.c_str()), ICEDB_DLL_BASE_HANDLE_destroy);
			{ // Scope ensures orderly destruction
				std::shared_ptr<interface_ICEDB_fs_plugin> iface(
					create_ICEDB_fs_plugin(dhnd.get()), destroy_ICEDB_fs_plugin);
				if (!icedb::fs::impl::has_valid_fs_interface(iface.get())) continue;
				if (iface->can_open_path(iface.get(), nullptr, path, ftype, flags)) {
					res = iface->open_path(iface.get(), nullptr, path, ftype, flags);
					break;
				}
			}
		}
		if (!res->i) goto error;
		goto done;
	error:
		if (res) delete res;
		res = nullptr;
		goto done;
	done:

		return res;
	}
}
ICEDB_error_code ICEDB_file_handle_move(ICEDB_FS_HANDLE_p p, const char* src, const char* dest) {
	verify_pointer_fs_p(p);
	if (!src || !dest) ICEDB_DEBUG_RAISE_EXCEPTION();
	if (p->c.can_move == false) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->move(p->i.get(), p, src, dest);
}
ICEDB_error_code ICEDB_file_handle_copy(ICEDB_FS_HANDLE_p p, const char* src, const char* dest, bool overwrite) {
	verify_pointer_fs_p(p);
	if (!src || !dest) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->copy(p->i.get(), p, src, dest, overwrite);
}
ICEDB_error_code ICEDB_file_handle_unlink(ICEDB_FS_HANDLE_p p, const char* path) {
	verify_pointer_fs_p(p);
	if (!path) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->unlink(p->i.get(), p, path);
}
ICEDB_error_code ICEDB_file_handle_create_hard_link(ICEDB_FS_HANDLE_p p, const char* src, const char* dest) {
	verify_pointer_fs_p(p);
	if (!src || !dest) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->create_hard_link(p->i.get(), p, src, dest);
}
ICEDB_error_code ICEDB_file_handle_create_sym_link(ICEDB_FS_HANDLE_p p, const char* src, const char* dest) {
	verify_pointer_fs_p(p);
	if (!src || !dest) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->create_sym_link(p->i.get(), p, src, dest);
}
ICEDB_error_code ICEDB_file_handle_follow_sym_link(ICEDB_FS_HANDLE_p p,
	const char* path, size_t out_mx_sz, size_t *szout, char** out) {
	verify_pointer_fs_p(p);
	if (!path || !out) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->follow_sym_link(p->i.get(), p, path, out_mx_sz, szout, out);
}

bool ICEDB_file_handle_path_exists(ICEDB_FS_HANDLE_p p, const char* path) {
	verify_pointer_fs_p(p);
	if (!path) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->path_exists(p->i.get(), p, path);
}
ICEDB_error_code ICEDB_file_handle_path_info(ICEDB_FS_HANDLE_p p, const char* path, ICEDB_FS_PATH_CONTENTS *res) {
	verify_pointer_fs_p(p);
	if (!path || !res) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->path_info(p->i.get(), p, path, res);
}
// Iterate / enumerate all one-level child objects
ICEDB_error_code ICEDB_file_handle_readobjs(ICEDB_FS_HANDLE_p p, const char* path, size_t *numObjs, ICEDB_FS_PATH_CONTENTS ***res) {
	verify_pointer_fs_p(p);
	if (!res || !numObjs) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->readobjs(p->i.get(), p, path, numObjs, res);
}
ICEDB_error_code ICEDB_file_handle_free_objs(ICEDB_FS_HANDLE_p p, ICEDB_FS_PATH_CONTENTS **res) {
	verify_pointer_fs_p(p);
	return p->i->free_objs(p->i.get(), p, res);
}

ICEDB_error_code ICEDB_file_handle_readobjattrs(ICEDB_FS_HANDLE_p p, ICEDB_FS_ATTR_CONTENTS **res) {
	verify_pointer_fs_p(p);
	if (!res) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->readobjattrs(p->i.get(), p, res);
}
ICEDB_error_code ICEDB_file_handle_attr_free_objattrs(ICEDB_FS_HANDLE_p p, ICEDB_FS_ATTR_CONTENTS **res) {
	verify_pointer_fs_p(p);
	return p->i->free_objattrs(p->i.get(), p, res);
}
ICEDB_error_code ICEDB_file_handle_attr_remove(ICEDB_FS_HANDLE_p p, const char* name) {
	verify_pointer_fs_p(p);
	if (!name) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->attr_remove(p->i.get(), p, name);
}
ICEDB_error_code ICEDB_file_handle_attr_insert(ICEDB_FS_HANDLE_p p, const char* name,
	const char* data, size_t sz, ICEDB_attr_types type) {
	verify_pointer_fs_p(p);
	if (!name || !data) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->attr_insert(p->i.get(), p, name, data, sz, type);
}


DL_ICEDB bool ICEDB_FS_PATH_CONTENTS_alloc(ICEDB_FS_PATH_CONTENTS* res) {
	if (!res) ICEDB_DEBUG_RAISE_EXCEPTION();
	res->idx = 0;
	//res->base_handle = icedb::plugins::fs_win::hndSelf;
	res->base_path = (char*)ICEDB_malloc(sizeof(char)*ICEDB_FS_PATH_CONTENTS_PATH_MAX);
	res->base_path[0] = '\0';
	res->p_name = (char*)ICEDB_malloc(sizeof(char)*ICEDB_FS_PATH_CONTENTS_PATH_MAX);
	res->p_name[0] = '\0';
	res->p_obj_type = (char*)ICEDB_malloc(sizeof(char)*ICEDB_FS_PATH_CONTENTS_PATH_MAX);
	res->p_obj_type[0] = '\0';
	res->p_type = ICEDB_path_types::ICEDB_type_nonexistant;
	return true;
}
DL_ICEDB bool ICEDB_FS_PATH_CONTENTS_free(ICEDB_FS_PATH_CONTENTS* res) {
	if (!res) ICEDB_DEBUG_RAISE_EXCEPTION();
	ICEDB_free(res->base_path);
	ICEDB_free(res->p_name);
	ICEDB_free(res->p_obj_type);
	return true;
}

DL_ICEDB const char* ICEDB_file_handle_get_name(ICEDB_FS_HANDLE_p p) {
	if (!p) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->pluginName;
}



ICEDB_END_DECL_C