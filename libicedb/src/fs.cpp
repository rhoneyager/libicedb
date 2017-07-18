#include "../icedb/fs/fs.h"
#include "../icedb/fs/fs_backend.hpp"
#include "../icedb/fs/fs_dll_impl.hpp"
#include "../icedb/dlls/dllsImpl.hpp"
#include "../icedb/dlls/dlls.hpp"
#include "../icedb/dlls/dlls.h"
#include <string>
#include <vector>

namespace icedb {
	namespace fs {
		namespace impl {
			const int64_t magic = 234987234973820484;
			void verify_pointer_fs_p(ICEDB_FS_HANDLE_p p) {
				if (!p) ICEDB_DEBUG_RAISE_EXCEPTION();
				if (p->magic != icedb::fs::impl::magic)ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->h) ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->i) ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->d) ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->h_dest) ICEDB_DEBUG_RAISE_EXCEPTION();
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
				if (!i->Bind_attr_rewind(i)) return false;
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
				if (!i->Bind_rewind(i)) return false;
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
	p->h_dest = nullptr;
	p->i = nullptr;
	p->d = nullptr;
	delete p;
}

ICEDB_FS_HANDLE_p ICEDB_file_handle_create
(const char* path, const char* ftype, ICEDB_file_open_flags flags) {
	{
		ICEDB_FS_HANDLE_p res = new ICEDB_FS_HANDLE;
		res->magic = icedb::fs::impl::magic;
		res->i = nullptr;
		res->d = nullptr;
		res->h = nullptr;
		res->h_dest = nullptr;

		std::vector<std::string> dlls = icedb::dll::query_interface("fs");
		for (const auto &f : dlls) {
			std::shared_ptr<ICEDB_DLL_BASE_HANDLE> dhnd(
				ICEDB_DLL_BASE_HANDLE_create(f.c_str()), ICEDB_DLL_BASE_HANDLE_destroy);
			{ // Scope ensures orderly destruction
				std::shared_ptr<interface_ICEDB_fs_plugin> iface(
					create_ICEDB_fs_plugin(dhnd.get()), destroy_ICEDB_fs_plugin);
				if (!icedb::fs::impl::has_valid_fs_interface(iface.get())) continue;
				if (iface->can_open_path(iface.get(), path, ftype, flags)) {
					iface->get_capabilities(iface.get(), &(res->c));
					res->i = iface;
					res->d = dhnd;
					//res->h_d = 
					res->h_dest = std::bind(iface->destroy, iface.get(), std::placeholders::_1);
					res->h = icedb::fs::hnd_t(
						iface->open_path(iface.get(), path, ftype, flags), res->h_dest);
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
ICEDB_error_code ICEDB_file_handle_copy(ICEDB_FS_HANDLE_p p, const char* src, const char* dest) {
	verify_pointer_fs_p(p);
	if (!src || !dest) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->copy(p->i.get(), p, src, dest);
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
ICEDB_error_code ICEDB_file_handle_path_info(ICEDB_FS_HANDLE_p p, const char* path, ICEDB_FS_PATH_CONTENTS **res) {
	verify_pointer_fs_p(p);
	if (!path || !res) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->path_info(p->i.get(), p, path, res);
}
// Iterate / enumerate all one-level child objects
ICEDB_error_code ICEDB_file_handle_readobjs(ICEDB_FS_HANDLE_p p, ICEDB_FS_PATH_CONTENTS **res) {
	verify_pointer_fs_p(p);
	if (!res) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->readobjs(p->i.get(), p, res);
}
ICEDB_error_code ICEDB_file_handle_rewind(ICEDB_FS_HANDLE_p p) {
	verify_pointer_fs_p(p);
	return p->i->rewind(p->i.get(), p);
}

ICEDB_error_code ICEDB_file_handle_readobjattrs(ICEDB_FS_HANDLE_p p, ICEDB_FS_ATTR_CONTENTS **res) {
	verify_pointer_fs_p(p);
	if (!res) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->readobjattrs(p->i.get(), p, res);
}
ICEDB_error_code ICEDB_file_handle_attr_rewind(ICEDB_FS_HANDLE_p p) {
	verify_pointer_fs_p(p);
	return p->i->attr_rewind(p->i.get(), p);
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


ICEDB_END_DECL_C