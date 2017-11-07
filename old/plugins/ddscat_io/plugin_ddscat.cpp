#include <map>
#include <string>
#include "plugin_ddscat.hpp"
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/fs/fs_dll_impl.hpp"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/misc/mem.h"
#include "../../libicedb/icedb/misc/memInterfaceImpl.hpp"
#include "../../libicedb/icedb/misc/util.h"
#include "../../libicedb/icedb/misc/utilInterfaceImpl.hpp"
#include "../../libicedb/icedb/error/error_contextInterfaceImpl.hpp"
#include "../../libicedb/icedb/error/errorInterfaceImpl.hpp"

ICEDB_DLL_PLUGINS_COMMON(level_00_ddscat_io);

namespace icedb {
	namespace plugins {
		namespace ddscat_io {
			ICEDB_DLL_BASE_HANDLE* hnd, *hndSelf;
			const uint64_t pluginMagic = 12149382433;
			const char* pluginName = "ddscat_io";
			ICEDB_fs_plugin_capabilities caps;
			std::map<std::string, std::string> libprops;
			std::shared_ptr<interface_ICEDB_core_util> i_util;
			std::shared_ptr<interface_ICEDB_core_mem> i_mem;
			std::shared_ptr<interface_ICEDB_core_error> i_error;
			std::shared_ptr<interface_ICEDB_core_error_context> i_error_context;
			std::shared_ptr<interface_ICEDB_fs_plugin> i_fs_self;
			std::shared_ptr<interface_ICEDB_core_fs> i_fs_core;
			std::string sSelfName;
		}
	}
}

using namespace icedb::plugins::ddscat_io;
ICEDB_handle_inner::ICEDB_handle_inner() : magic(pluginMagic) {}



extern "C" {
	bool isValidHandleInner(ICEDB_handle_inner* p) {
		if (!p) return false;
		if (p->magic != pluginMagic) return false;
		return true;
	}
	bool isValidHandle(ICEDB_FS_HANDLE_p p) {
		if (!p) return false;
		if (p->magic != pluginMagic) return false;
		if (!p->d) return false;
		if (!p->h) return false;
		//if (!p->h_dest) return false;
		if (!p->i) return false;
		if (!isValidHandleInner(p->h.get())) return false;
		return true;
	}

	ICEDB_FS_HANDLE_p makeHandle() {
		ICEDB_FS_HANDLE_p p(new ICEDB_FS_HANDLE);
		p->magic = pluginMagic;
		p->d = hndSelf;
		//p->h_dest;
		p->h = std::shared_ptr<ICEDB_handle_inner>(new ICEDB_handle_inner);
		p->i = i_fs_self;
		p->pluginName = pluginName;
		return p;
	}

	SHARED_EXPORT_ICEDB void fs_get_capabilities(ICEDB_fs_plugin_capabilities* p) {
		caps.can_copy = false;
		caps.can_delete = false;
		caps.can_hard_link = false;
		caps.can_move = false;
		caps.can_soft_link = false;
		caps.fs_has_cyclic_links = false;
		caps.has_external_links = false;
		caps.has_folders = false;
		caps.has_xattrs = false;
		if (!p) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		*p = caps;
	}

	SHARED_EXPORT_ICEDB	ICEDB_file_open_flags fs_get_open_flags(ICEDB_FS_HANDLE_p p) {
		if (!isValidHandle(p))
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		return p->open_flags;
	}

	SHARED_EXPORT_ICEDB void fs_destroy(ICEDB_FS_HANDLE_p p) {
		if (!isValidHandle(p))
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		p->h = nullptr;
		//p->h_dest = nullptr;
		p->i = nullptr;
		p->d = nullptr;
		p->magic = 0;
		delete p;
	}

	SHARED_EXPORT_ICEDB void fs_set_property(ICEDB_FS_HANDLE_p p, const char* key, const char* val) {
		if (!isValidHandle(p) || !key || !val)
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		p->h->props[std::string(key)] = std::string(val);
	}

	SHARED_EXPORT_ICEDB void fs_set_global_property(const char* key, const char* val) {
		if (!key || !val)
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		libprops[std::string(key)] = std::string(val);
	}

	SHARED_EXPORT_ICEDB size_t fs_get_property(ICEDB_FS_HANDLE_p p, const char* key, size_t mxsz, char** val, size_t *sz) {
		if (!isValidHandle(p) || !key || !mxsz)
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		if (p->h->props.count(std::string(key))) {
			std::string sval = p->h->props[std::string(key)];
			size_t res = i_util->strncpy_s(i_util.get(), *val, mxsz, sval.c_str(), sval.length());
			*sz = sval.length() + 1;
			return res;
		}
		else {
			*val[0] = '\0';
			*sz = 0;
			return 0;
		}
	}

	SHARED_EXPORT_ICEDB size_t fs_get_global_property(const char* key, size_t mxsz, char** val, size_t *sz) {
		if (!key)
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		if (libprops.count(std::string(key))) {
			std::string sval = libprops[std::string(key)];
			size_t res = i_util->strncpy_s(i_util.get(), *val, mxsz, sval.c_str(), sval.length());
			*sz = sval.length() + 1;
			return res;
		}
		else {
			*val[0] = '\0';
			*sz = 0;
			return 0;
		}
	}




	SHARED_EXPORT_ICEDB size_t fs_can_open_path(ICEDB_FS_HANDLE_p base_p, const char* p, const char* t, ICEDB_file_open_flags flags) {
		const size_t valid_pri = 100;
		size_t res = 0;
		
		if (!base_p) { // Use the base filesystem plugin
			base_p = i_fs_core->ICEDB_file_handle_create_with_registry(i_fs_core.get(), p, t, flags, "fs-base");
			if (!base_p) return 0;
		}
		if (!base_p->i) return 0;
		if (!base_p->i->path_exists(base_p->i.get(), base_p, p)) return 0;


		ICEDB_fs_plugin_capabilities caps;
		base_p->i->get_capabilities(base_p->i.get(), &caps);

		ICEDB_FS_PATH_CONTENTS finfo;
		i_fs_core->ICEDB_FS_PATH_CONTENTS_alloc(i_fs_core.get(), &finfo);
		base_p->i->path_info(base_p->i.get(), base_p, p, &finfo);
		ICEDB_path_types pt = finfo.p_type;
		//if (pt == ICEDB_path_types::ICEDB_type_symlink)
		//	base_p->i->follow_sym_link(base_p->i.get()), base_p, 
		std::map<std::string, ICEDB_path_types> children;
		if (pt == ICEDB_path_types::ICEDB_type_folder) {
			ICEDB_FS_PATH_CONTENTS **fc = nullptr;
			size_t numObjs = 0;
			ICEDB_error_code err = base_p->i->readobjs(base_p->i.get(), base_p, p, &numObjs, &fc);
			for (size_t i = 0; i < numObjs; ++i) {
				children[std::string((*fc)[i].p_name)] = (*fc)[i].p_type;
			}
			err = base_p->i->free_objs(base_p->i.get(), base_p, fc);
		} else if (pt == ICEDB_path_types::ICEDB_type_normal_file) {
			children[std::string(p)] = pt;
		}
		i_fs_core->ICEDB_FS_PATH_CONTENTS_free(i_fs_core.get(), &finfo);

		// Recognized signatures:
		// A ddscat directory (containing ddscat.par, target.out, and one or more .avg, .sca or .fml files)
		// ddscat.par
		// target.out / shape.dat
		// .avg file
		// .sca file
		// .fml file
		// Any types of : ddshape, ddpar, ddavg, ddsca, ddfml, ddfolder

		std::string sType;
		if (t) sType = std::string(t);
		if (t == "ddshape" && pt == ICEDB_path_types::ICEDB_type_normal_file) return valid_pri;
		else if (t == "ddpar" && pt == ICEDB_path_types::ICEDB_type_normal_file) return valid_pri;
		else if (t == "ddavg" && pt == ICEDB_path_types::ICEDB_type_normal_file) return valid_pri;
		else if (t == "ddsca" && pt == ICEDB_path_types::ICEDB_type_normal_file) return valid_pri;
		else if (t == "ddfml" && pt == ICEDB_path_types::ICEDB_type_normal_file) return valid_pri;
		else if (t == "ddfolder" && pt == ICEDB_path_types::ICEDB_type_folder) return valid_pri;
		else if (t == "level00ddscat") return valid_pri;

		// Consult attributes, too.
		if (caps.has_xattrs) {
			if (base_p->i->Bind_readobjattrs(base_p->i.get())) {
				base_p->i->readobjattrs(base_p->i.get(), base_p, 0);
			}
		}

		std::string sp, spfile, spext;
		if (p) sp = std::string(p);
		//else pfname = base_p->
		size_t pos = sp.find_last_of("/\\");
		if (pos == std::string::npos) pos = 0;
		if (sp.length() > pos + 1)
			spfile = sp.substr(pos+1);
		pos = spfile.find_last_of('.');
		if (pos == std::string::npos) pos = 0;
		spext = sp.substr(pos);

		if (spfile == "ddscat.par"&& pt != ICEDB_path_types::ICEDB_type_folder) return valid_pri;
		if (spext == ".avg"&& pt != ICEDB_path_types::ICEDB_type_folder) return valid_pri;
		if (spfile == "shape.dat"&& pt != ICEDB_path_types::ICEDB_type_folder) return valid_pri;
		if (spfile == "target.out"&& pt != ICEDB_path_types::ICEDB_type_folder) return valid_pri;
		if (spext == ".fml"&& pt != ICEDB_path_types::ICEDB_type_folder) return valid_pri;
		if (spext == ".sca"&& pt != ICEDB_path_types::ICEDB_type_folder) return valid_pri;

		if (pt == ICEDB_path_types::ICEDB_type_folder) {
			if (children.count("ddscat.par") && children.count("shape.dat")) {
				for (const auto &i : children) {
					const std::string &sp = i.first;
					std::string spfile, spext;
					size_t pos = sp.find_last_of("/\\");
					if (pos == std::string::npos) pos = 0;
					if (sp.length() > pos + 1)
						spfile = sp.substr(pos + 1);
					pos = spfile.find_last_of('.');
					if (pos == std::string::npos) pos = 0;
					spext = sp.substr(pos);

					if (spext == ".avg" && i.second != ICEDB_path_types::ICEDB_type_folder)
						return valid_pri;
				}
			}
		}
		return res;
	}

	SHARED_EXPORT_ICEDB ICEDB_FS_HANDLE_p fs_open_path(
		ICEDB_FS_HANDLE_p p, const char* path, const char* typ, ICEDB_file_open_flags flags) {
		// Does the path exist? Consult flags.
		// Implicitly checks that p is a valid handle of the same plugin, if it is defined.
		bool exists = false; // fs_path_exists(p, path);
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
			}
			else {
				std::string effpath; // = makeEffPath(p, path);
				//bool res = CreateDirectoryA(effpath.c_str(), NULL);
				//if (!res) GenerateWinOSerror();
				// Create the handle to the new folder.
				ICEDB_FS_HANDLE_p h = makeHandle();
				h->open_flags = flags;
				h->h->cwd = effpath;
				return h;
			}
		}
		else if (exists && (flags & ICEDB_file_open_flags::ICEDB_flags_truncate)) {
			// Not supported. No desire yet to truncate a folder.
			ICEDB_error_context* err = i_error_context->error_context_create_impl(
				i_error_context.get(), ICEDB_ERRORCODES_UNIMPLEMENTED, __FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
			return nullptr;
		}
		else if (exists && (
			(flags & ICEDB_file_open_flags::ICEDB_flags_rw) || (flags & ICEDB_file_open_flags::ICEDB_flags_none)
			|| (flags & ICEDB_file_open_flags::ICEDB_flags_readonly))) {
			std::string effpath; // = makeEffPath(p, path);
			// Create the handle to the new folder.
			ICEDB_FS_HANDLE_p h = makeHandle();
			h->open_flags = flags;
			h->h->cwd = effpath;
			return h;
		}

		return nullptr;
	}



	SHARED_EXPORT_ICEDB bool Register(ICEDB_register_interface_f fReg, ICEDB_get_module_f fMod, 
		ICEDB_DLL_BASE_HANDLE* hDll, ICEDB_DLL_BASE_HANDLE* hSelf) {
		char cSelf[ICEDB_FS_PATH_CONTENTS_PATH_MAX] = "";
		fMod((void*)Register, ICEDB_FS_PATH_CONTENTS_PATH_MAX, cSelf);
		sSelfName = std::string(cSelf);
		fReg("fs", 100, cSelf);
		fReg("level_00_ddscat_io_shape", 100, cSelf);
		fReg("level_00_ddscat_io_dieltab", 100, cSelf);
		fReg("level_00_ddscat_io_fml", 100, cSelf);
		fReg("level_00_ddscat_io_sca", 100, cSelf);
		fReg("level_00_ddscat_io_avg", 100, cSelf);

		hnd = hDll;
		hndSelf = hSelf;
		i_fs_self = std::shared_ptr<interface_ICEDB_fs_plugin>(create_ICEDB_fs_plugin(hSelf), destroy_ICEDB_fs_plugin);

		i_util = std::shared_ptr<interface_ICEDB_core_util>(create_ICEDB_core_util(hDll), destroy_ICEDB_core_util);
		i_mem = std::shared_ptr<interface_ICEDB_core_mem>(create_ICEDB_core_mem(hDll), destroy_ICEDB_core_mem);
		i_error = std::shared_ptr<interface_ICEDB_core_error>(create_ICEDB_core_error(hDll), destroy_ICEDB_core_error);
		i_error_context = std::shared_ptr<interface_ICEDB_core_error_context>(create_ICEDB_core_error_context(hDll), destroy_ICEDB_core_error_context);
		i_fs_core = std::shared_ptr<interface_ICEDB_core_fs>(create_ICEDB_core_fs(hDll), destroy_ICEDB_core_fs);
		return true;
	}

	SHARED_EXPORT_ICEDB void Unregister(ICEDB_register_interface_f fUnReg, ICEDB_get_module_f fMod) {
		fUnReg("fs", 100, sSelfName.c_str());
		fUnReg("level_00_ddscat_io_shape", 100, sSelfName.c_str());
		fUnReg("level_00_ddscat_io_dieltab", 100, sSelfName.c_str());
		fUnReg("level_00_ddscat_io_fml", 100, sSelfName.c_str());
		fUnReg("level_00_ddscat_io_sca", 100, sSelfName.c_str());
		fUnReg("level_00_ddscat_io_avg", 100, sSelfName.c_str());
		i_util = nullptr;
		i_mem = nullptr;
		i_error = nullptr;
		i_error_context = nullptr;
		i_fs_self = nullptr;
		i_fs_core = nullptr;
		hndSelf = nullptr;
	}
}
