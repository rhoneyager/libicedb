#include "../../libicedb/icedb/fs/fs_dll_impl.hpp"
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/misc/util.h"
#include "../../libicedb/icedb/misc/utilInterfaceImpl.hpp"
#include "../../libicedb/icedb/misc/memInterfaceImpl.hpp"
#include "../../libicedb/icedb/dlls/plugins.h"
#include "../../libicedb/icedb/dlls/dllsInterface.hpp"
#include "../../libicedb/icedb/dlls/dllsInterfaceImpl.hpp"
#include "../../libicedb/icedb/error/error_contextInterfaceImpl.hpp"
#include "../../libicedb/icedb/error/errorInterfaceImpl.hpp"

#include "plugin_netcdf.hpp"
#include "plugin_netcdf_interface.h"
#include "plugin_netcdf_impl.hpp"

ICEDB_DLL_PLUGINS_COMMON(fs_netcdf);

namespace icedb {
	namespace plugins {
		namespace fs_netcdf {
			ICEDB_DLL_BASE_HANDLE *hnd, *hndSelf;
			const uint64_t pluginMagic = 378274376432;
			const char* pluginName = "fs_netcdf";
			ICEDB_fs_plugin_capabilities caps;
			
			std::map<std::string, std::string> libprops;
			std::shared_ptr<interface_ICEDB_core_util> i_util;
			std::shared_ptr<interface_ICEDB_core_mem> i_mem;
			std::shared_ptr<interface_ICEDB_core_error> i_error;
			std::shared_ptr<interface_ICEDB_core_error_context> i_error_context;
			std::shared_ptr<interface_ICEDB_fs_plugin> i_fs_self;
			std::shared_ptr<interface_ICEDB_core_fs> i_fs_core;
			std::string sSelfName;

			std::shared_ptr<ICEDB_DLL_BASE_HANDLE> hndNetCDF = nullptr;
			std::shared_ptr<interface_dlls> i_dlls;
			std::function<void(ICEDB_DLL_BASE_HANDLE*)> hndDestroyer;
			std::shared_ptr<interface_netcdf> i_nc;
		}
	}
}
using namespace icedb::plugins::fs_netcdf;
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
		p->h->ncid = -1;
		return p;
	}


	SHARED_EXPORT_ICEDB void fs_get_capabilities(ICEDB_fs_plugin_capabilities* p) {
		caps.can_copy = true;
		caps.can_delete = true;
		caps.can_hard_link = true;
		caps.can_move = true;
		caps.can_soft_link = true;
		caps.fs_has_cyclic_links = false;
		caps.has_external_links = false;
		caps.has_folders = true;
		caps.has_xattrs = true;
		if (!p) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		*p = caps;
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


	SHARED_EXPORT_ICEDB bool Register(ICEDB_register_interface_f fReg, ICEDB_get_module_f fMod, ICEDB_DLL_BASE_HANDLE* hDllMain, ICEDB_DLL_BASE_HANDLE* hSelf) {
		const size_t sz = 2048;
		char cSelf[ICEDB_FS_PATH_CONTENTS_PATH_MAX] = "";
		fMod((void*)Register, ICEDB_FS_PATH_CONTENTS_PATH_MAX, cSelf);
		sSelfName = std::string(cSelf);
		fReg("fs", 100, cSelf);

		hnd = hDllMain;
		hndSelf = hSelf;
		i_fs_self = std::shared_ptr<interface_ICEDB_fs_plugin>(create_ICEDB_fs_plugin(hSelf), destroy_ICEDB_fs_plugin);

		i_util = std::shared_ptr<interface_ICEDB_core_util>(create_ICEDB_core_util(hDllMain), destroy_ICEDB_core_util);
		i_mem = std::shared_ptr<interface_ICEDB_core_mem>(create_ICEDB_core_mem(hDllMain), destroy_ICEDB_core_mem);
		i_error = std::shared_ptr<interface_ICEDB_core_error>(create_ICEDB_core_error(hDllMain), destroy_ICEDB_core_error);
		i_error_context = std::shared_ptr<interface_ICEDB_core_error_context>(create_ICEDB_core_error_context(hDllMain), destroy_ICEDB_core_error_context);
		i_fs_core = std::shared_ptr<interface_ICEDB_core_fs>(create_ICEDB_core_fs(hDllMain), destroy_ICEDB_core_fs);

		i_dlls = std::shared_ptr<interface_dlls>(create_dlls(hDllMain), destroy_dlls);
		
		auto hNC = i_dlls->DLL_BASE_HANDLE_create(i_dlls.get(), "C:\\Program Files\\netCDF 4.4.1.1\\bin\\netcdf.dll");
		if (!hNC) {
			hnd->_vtable->_raiseExcept(hnd,
				__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
			return false;
		}
		hndDestroyer = std::bind(i_dlls->DLL_BASE_HANDLE_destroy, i_dlls.get(), std::placeholders::_1);
		auto hndNetCDF = std::shared_ptr<ICEDB_DLL_BASE_HANDLE>(hNC, hndDestroyer);
		ICEDB_error_code dlerr = hndNetCDF->_vtable->open(hndNetCDF.get());
		if (dlerr) hnd->_vtable->_raiseExcept(hnd,
			__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		bool isOpen = hndNetCDF->_vtable->isOpen(hndNetCDF.get());
		auto i_nc = std::shared_ptr<interface_netcdf>(create_netcdf(hndNetCDF.get()), destroy_netcdf);

		//int ncfp = 0;
		//auto b = i_nc->Bind_nc_create(i_nc.get());
		//int err = i_nc->nc_create(i_nc.get(), "C:\\Users\\ryan\\Documents\\Visual Studio 2017\\Projects\\germany_api_3\\build\\test.nc", NC_NETCDF4, &ncfp);
		//if (err != NC_NOERR) hnd->_vtable->_raiseExcept(hnd,
	//		__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
		//i_nc->nc_close(i_nc.get(), ncfp);
		return true;
	}

	SHARED_EXPORT_ICEDB void Unregister(ICEDB_register_interface_f fUnReg, ICEDB_get_module_f fMod) {
		const size_t sz = 2048;
		char buf[sz] = "";
		fUnReg("fs", 100, sSelfName.c_str());
		i_nc = nullptr;
		hndNetCDF = nullptr;
		hndDestroyer = nullptr;
		i_dlls = nullptr;
		
		i_util = nullptr;
		i_mem = nullptr;
		i_error = nullptr;
		i_error_context = nullptr;
		i_fs_self = nullptr;
		i_fs_core = nullptr;
		hndSelf = nullptr;
	}
}
