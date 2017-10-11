#include "../icedb/fs/fs.h"
#include "../icedb/fs/fs_backend.hpp"
//#include "../icedb/fs/fs_dll_impl.hpp"
#include "../icedb/dlls/dllsImpl.hpp"
#include "../icedb/dlls/dlls.hpp"
#include "../icedb/dlls/dlls.h"
#include "../icedb/dlls/linking.h"
#include "../icedb/fs/fs_attr_base.h"
#include "../icedb/fs/fs_tbl_base.h"
#include <string>
#include <vector>

namespace icedb {
	namespace fs {
		namespace impl {
			const int64_t magic = 234987234973820484;
			void verify_pointer_fs_p(ICEDB_fs_hnd* p) {
				if (!p) ICEDB_DEBUG_RAISE_EXCEPTION();
				//if (p->magic != icedb::fs::impl::magic)ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->_h) ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->_h->i) ICEDB_DEBUG_RAISE_EXCEPTION();
				if (!p->_h->d) ICEDB_DEBUG_RAISE_EXCEPTION();
				//if (!p->h_dest) ICEDB_DEBUG_RAISE_EXCEPTION();
			}
			bool has_valid_fs_interface(interface_ICEDB_fs_plugin *i) {
				/*
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
				*/
				ICEDB_DEBUG_RAISE_EXCEPTION();
				return true;
			}
		}
	}
}
ICEDB_BEGIN_DECL_C

using namespace icedb::fs::impl;

void fs_getHandlers(
	const char* registry,
	ICEDB_OUT size_t* const numPlugins,
	ICEDB_OUT char *** const pluginids,
	ICEDB_OUT ICEDB_free_charIPPP_f * const deallocator)
{
	ICEDB_DEBUG_RAISE_EXCEPTION();
}
DL_ICEDB ICEDB_fs_getHandlers_f ICEDB_fs_getHandlers = fs_getHandlers;

bool fs_canOpenPath(
	const char* path,
	ICEDB_OPTIONAL const char* pathtype,
	ICEDB_OPTIONAL const char* pluginid,
	ICEDB_OPTIONAL const ICEDB_fs_hnd* base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags,
	ICEDB_OUT size_t* const numHandlersThatCanOpen,
	ICEDB_OPTIONAL ICEDB_OUT char *** const pluginids,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPPP_f * const deallocator,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err)
{
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return false;
}
DL_ICEDB ICEDB_fs_canOpenPath_f ICEDB_fs_canOpenPath = fs_canOpenPath;

ICEDB_fs_hnd* fs_openPath(
	const char* path,
	ICEDB_OPTIONAL const char* ftype,
	ICEDB_OPTIONAL const char* pluginid,
	ICEDB_OPTIONAL ICEDB_fs_hnd* base_handle,
	ICEDB_OPTIONAL ICEDB_file_open_flags flags)
{
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return nullptr;
}
DL_ICEDB ICEDB_fs_openPath_f ICEDB_fs_openPath = fs_openPath;

ICEDB_fs_hnd* fs_cloneHandle(
	ICEDB_fs_hnd* handle)
{
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return nullptr;
}
DL_ICEDB ICEDB_fs_cloneHandle_f ICEDB_fs_cloneHandle = fs_cloneHandle;

const char* fs_getPathFromHandle(
	const ICEDB_fs_hnd* handle,
	ICEDB_OPTIONAL size_t inPathSize,
	ICEDB_OUT size_t* outPathSize,
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator)
{
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return nullptr;
}
DL_ICEDB ICEDB_fs_getPathFromHandle_f ICEDB_fs_getPathFromHandle = fs_getPathFromHandle;

ICEDB_file_open_flags fs_getHandleIOflags(const ICEDB_fs_hnd* p) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return ICEDB_file_open_flags::ICEDB_flags_invalid;
}
DL_ICEDB ICEDB_fs_getHandleIOflags_f ICEDB_fs_getHandleIOflags = fs_getHandleIOflags;

ICEDB_error_code fs_closeHandle(ICEDB_fs_hnd* p) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return 0;
}
DL_ICEDB ICEDB_fs_closeHandle_f ICEDB_fs_closeHandle = fs_closeHandle;

ICEDB_error_code fs_move(ICEDB_fs_hnd* p, const char* src, const char* dest) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return 0;
}
DL_ICEDB ICEDB_fs_move_f ICEDB_fs_move = fs_move;

ICEDB_error_code fs_copy(ICEDB_fs_hnd* p, const char* src, const char* dest, bool overwrite) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return 0;
}
DL_ICEDB ICEDB_fs_copy_f ICEDB_fs_copy = fs_copy;

ICEDB_error_code fs_unlink(ICEDB_fs_hnd* p, const char* path) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return 0;
}
DL_ICEDB ICEDB_fs_unlink_f ICEDB_fs_unlink = fs_unlink;

ICEDB_error_code fs_createHardLink(ICEDB_fs_hnd* p, const char* src, const char* dest) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return 0;
}
DL_ICEDB ICEDB_fs_createHardLink_f ICEDB_fs_createHardLink = fs_createHardLink;

ICEDB_error_code fs_createSymLink(ICEDB_fs_hnd* p, const char* src, const char* dest) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return 0;
}
DL_ICEDB ICEDB_fs_createSymLink_f ICEDB_fs_createSymLink = fs_createSymLink;

const char* fs_followSymLink(ICEDB_fs_hnd* p,
	const char* path,
	ICEDB_OPTIONAL size_t inPathSize,
	ICEDB_OUT size_t* outPathSize,
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator)
{
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return 0;
}
DL_ICEDB ICEDB_fs_followSymLink_f ICEDB_fs_followSymLink = fs_followSymLink;

bool fs_doesPathExist(const ICEDB_fs_hnd* p, const char* path, ICEDB_OPTIONAL ICEDB_OUT ICEDB_error_code* err) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return false;
}
DL_ICEDB ICEDB_fs_doesPathExist_f ICEDB_fs_doesPathExist = fs_doesPathExist;

ICEDB_fs_path_contents* fs_getPathInfo(
	const ICEDB_fs_hnd* p,
	const char* path,
	ICEDB_OUT ICEDB_fs_path_contents *res)
{
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return 0;
}
DL_ICEDB ICEDB_fs_getPathInfo_f ICEDB_fs_getPathInfo = fs_getPathInfo;

void fs_pathInfoFree(ICEDB_fs_path_contents *pc) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
}
DL_ICEDB ICEDB_fs_pathInfoFree_f ICEDB_fs_pathInfoFree = fs_pathInfoFree;

ICEDB_fs_objectList_t fs_getAllObjects(
	const ICEDB_fs_hnd* p,
	const char* path,
	size_t *numObjs,
	ICEDB_OUT ICEDB_fs_objectList_t res) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return 0;
}
DL_ICEDB ICEDB_fs_getAllObjects_f ICEDB_fs_getAllObjects = fs_getAllObjects;

void fs_freeObjs(
	ICEDB_fs_objectList_t p) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
}
DL_ICEDB ICEDB_fs_freeObjs_f ICEDB_fs_freeObjs = fs_freeObjs;

DL_ICEDB const struct ICEDB_fs_container_ftable ICEDB_funcs_fs = {
	fs_getHandlers,
	fs_canOpenPath,
	fs_openPath,
	fs_cloneHandle,
	fs_getPathFromHandle,
	fs_getHandleIOflags,
	fs_closeHandle,
	fs_move,
	fs_copy,
	fs_unlink,
	fs_createHardLink,
	fs_createSymLink,
	fs_followSymLink,
	fs_doesPathExist,
	fs_getPathInfo,
	fs_pathInfoFree,
	fs_getAllObjects,
	fs_freeObjs,
	ICEDB_funcs_attr_container,
	ICEDB_funcs_tbl_container
};





/*
void ICEDB_file_handle_destroy(ICEDB_fs_hnd* p) {
	verify_pointer_fs_p(p);
	p->_h->h = nullptr; // h's destructor depends on h_dest's existence
	//p->h_dest = nullptr;
	p->_h->i = nullptr;
	p->_h->d = nullptr;
	delete p;
}

ICEDB_fs_hnd* ICEDB_file_handle_create
(const char* path, const char* ftype, ICEDB_file_open_flags flags) {
	return ICEDB_file_handle_create_with_registry(path, ftype, flags, "fs");
}

ICEDB_fs_hnd* ICEDB_file_handle_create_with_registry
(const char* path, const char* ftype, ICEDB_file_open_flags flags, const char* reg) {
	{
		ICEDB_fs_hnd* res = nullptr;
		//res->h_dest = nullptr;

		std::vector<std::string> dlls = icedb::dll::query_interface(reg);
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
ICEDB_error_code ICEDB_file_handle_move(ICEDB_fs_hnd* p, const char* src, const char* dest) {
	verify_pointer_fs_p(p);
	if (!src || !dest) ICEDB_DEBUG_RAISE_EXCEPTION();
	if (p->c.can_move == false) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->move(p->i.get(), p, src, dest);
}
ICEDB_error_code ICEDB_file_handle_copy(ICEDB_fs_hnd* p, const char* src, const char* dest, bool overwrite) {
	verify_pointer_fs_p(p);
	if (!src || !dest) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->copy(p->i.get(), p, src, dest, overwrite);
}
ICEDB_error_code ICEDB_file_handle_unlink(ICEDB_fs_hnd* p, const char* path) {
	verify_pointer_fs_p(p);
	if (!path) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->unlink(p->i.get(), p, path);
}
ICEDB_error_code ICEDB_file_handle_create_hard_link(ICEDB_fs_hnd* p, const char* src, const char* dest) {
	verify_pointer_fs_p(p);
	if (!src || !dest) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->create_hard_link(p->i.get(), p, src, dest);
}
ICEDB_error_code ICEDB_file_handle_create_sym_link(ICEDB_fs_hnd* p, const char* src, const char* dest) {
	verify_pointer_fs_p(p);
	if (!src || !dest) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->create_sym_link(p->i.get(), p, src, dest);
}
ICEDB_error_code ICEDB_file_handle_follow_sym_link(ICEDB_fs_hnd* p,
	const char* path, size_t out_mx_sz, size_t *szout, char** out) {
	verify_pointer_fs_p(p);
	if (!path || !out) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->follow_sym_link(p->i.get(), p, path, out_mx_sz, szout, out);
}

bool ICEDB_file_handle_path_exists(ICEDB_fs_hnd* p, const char* path) {
	verify_pointer_fs_p(p);
	if (!path) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->path_exists(p->i.get(), p, path);
}
ICEDB_error_code ICEDB_file_handle_path_info(ICEDB_fs_hnd* p, const char* path, ICEDB_FS_PATH_CONTENTS *res) {
	verify_pointer_fs_p(p);
	if (!path || !res) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->path_info(p->i.get(), p, path, res);
}
// Iterate / enumerate all one-level child objects
ICEDB_error_code ICEDB_file_handle_readobjs(ICEDB_fs_hnd* p, const char* path, size_t *numObjs, ICEDB_FS_PATH_CONTENTS ***res) {
	verify_pointer_fs_p(p);
	if (!res || !numObjs) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->readobjs(p->i.get(), p, path, numObjs, res);
}
ICEDB_error_code ICEDB_file_handle_free_objs(ICEDB_fs_hnd* p, ICEDB_FS_PATH_CONTENTS **res) {
	verify_pointer_fs_p(p);
	return p->i->free_objs(p->i.get(), p, res);
}

ICEDB_error_code ICEDB_file_handle_readobjattrs(ICEDB_fs_hnd* p, ICEDB_FS_attr_CONTENTS **res) {
	verify_pointer_fs_p(p);
	if (!res) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->readobjattrs(p->i.get(), p, res);
}
ICEDB_error_code ICEDB_file_handle_attr_free_objattrs(ICEDB_fs_hnd* p, ICEDB_FS_attr_CONTENTS **res) {
	verify_pointer_fs_p(p);
	return p->i->free_objattrs(p->i.get(), p, res);
}
ICEDB_error_code ICEDB_file_handle_attr_remove(ICEDB_fs_hnd* p, const char* name) {
	verify_pointer_fs_p(p);
	if (!name) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->i->attr_remove(p->i.get(), p, name);
}
ICEDB_error_code ICEDB_file_handle_attr_insert(ICEDB_fs_hnd* p, const char* name,
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

DL_ICEDB const char* ICEDB_file_handle_get_name(ICEDB_fs_hnd* p) {
	if (!p) ICEDB_DEBUG_RAISE_EXCEPTION();
	return p->pluginName;
}

*/

ICEDB_END_DECL_C
