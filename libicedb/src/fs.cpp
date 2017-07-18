#include "../icedb/fs/fs.h"
#include "../icedb/fs/fs_backend.hpp"
#include "../icedb/fs/fs_dll_impl.hpp"

namespace icedb {
	namespace fs {
		namespace impl {
			const int64_t magic = 234987234973820484;
			void verify_pointer_fs_p(ICEDB_FS_HANDLE_p p) {
				if (!p) ICEDB_DEBUG_RAISE_EXCEPTION();
				if (p->magic != icedb::fs::impl::magic)ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->h) ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->i) ICEDB_DEBUG_RAISE_EXCEPTION();
			}
		}
	}
}
ICEDB_BEGIN_DECL_C

using namespace icedb::fs::impl;

void ICEDB_file_handle_destroy(ICEDB_FS_HANDLE_p p) {
	verify_pointer_fs_p(p);
	p->i = nullptr;
	p->h = nullptr;
	delete p;
}
/*
ICEDB_FS_HANDLE_p ICEDB_file_handle_create(const char* path, const char* ftype, ICEDB_file_open_flags) {

}
ICEDB_FS_HANDLE_p ICEDB_file_handle_open_sub(ICEDB_FS_HANDLE_p base, const char* path, const char* ftype, ICEDB_file_open_flags) {

}
*/
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