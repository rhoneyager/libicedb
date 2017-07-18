#include "../icedb/dlls/dllsImpl.h"
#include "../icedb/dlls/dllsImpl.hpp"
#include "../icedb/misc/mem.h"
#include "../icedb/error/error_context.h"
#include "../icedb/error/error.h"
#include "../icedb/misc/util.h"
#include "../icedb/dlls/dlls.hpp"
#include "../icedb/misc/os_functions.h"
#include "../icedb/dlls/plugins.h"
#include "../icedb/versioning/versioning.hpp"
#include <stdarg.h>
#include <functional>
#include <mutex>
#include <typeinfo>
#include <map>
#include <set>
#include <string>
#if defined(_WIN32)
#include <Windows.h>
#endif

#ifdef UNICODE
typedef wchar_t* CharArrayStr_t;
CharArrayStr_t convertCharArrayStr(const char* charArray)
{
	wchar_t* wString = new wchar_t[16384];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 16384);
	return wString;
}
#else
typedef const char* CharArrayStr_t;
CharArrayStr_t convertCharArrayStr(const char* charArray)
{
	return ICEDB_COMPAT_strdup_s(charArray, strlen(charArray));
	charArray;
}
#endif


namespace icedb {
	namespace dll {
		namespace impl {
			std::map<std::string, ICEDB_DLL_BASE_HANDLE*> pluginHandles;
			std::map<std::string, std::multimap<int, std::string> > topicMaps;

			ICEDB_DLL_BASE_HANDLE* libInst;

			void _init() {
				static bool inited = false;
				if (inited) return;
				inited = true;
				libInst = ICEDB_DLL_BASE_HANDLE_create(ICEDB_getLibPathC());
			}
		}
	}
}

ICEDB_CALL_C DL_ICEDB ICEDB_DLL_BASE_HANDLE* ICEDB_DLL_BASE_HANDLE_create(const char* filename) {
	ICEDB_DLL_BASE_HANDLE *res = (ICEDB_DLL_BASE_HANDLE*)ICEDB_malloc(sizeof (ICEDB_DLL_BASE_HANDLE));
	res->refCount = 0;
	res->autoOpen = true;
	res->openCount = 0;
	res->_dlHandle = (_dlHandleType_impl*) ICEDB_malloc(sizeof(_dlHandleType_impl));
	if (!res->_dlHandle) ICEDB_DEBUG_RAISE_EXCEPTION();
	res->_dlHandle->h = NULL;
	res->path = NULL;
	res->_vtable = ICEDB_DLL_BASE_create_vtable();
	res->_vtable->setPath(res, filename);
	return res;
}

ICEDB_CALL_C DL_ICEDB ICEDB_DLL_BASE_HANDLE* ICEDB_DLL_BASE_HANDLE_create_from_lib() {
	ICEDB_DLL_BASE_HANDLE* res = ICEDB_DLL_BASE_HANDLE_create(ICEDB_getLibPathC());
	return res;
}

ICEDB_CALL_C DL_ICEDB void ICEDB_DLL_BASE_HANDLE_destroy(ICEDB_DLL_BASE_HANDLE* h) {
	if (h->refCount) {
		ICEDB_error_context* e = ICEDB_error_context_create(ICEDB_ERRORCODES_DLL_BASE_REFS_EXIST);
		ICEDB_error_context_add_string2(e, "DLL-Path", h->path);
		const int errStrSz = 250;
		char ErrString[errStrSz] = "";
		snprintf(ErrString, errStrSz, "%u", h->refCount);
		ICEDB_error_context_add_string2(e, "Num-Refs", ErrString);
		ICEDB_DEBUG_RAISE_EXCEPTION();
	}
	if (h->path) {
		ICEDB_free((void*)h->path);
		h->path = NULL;
	}
	if (h->_vtable->isOpen(h)) h->_vtable->close(h);
	if (h->_dlHandle) ICEDB_free(h->_dlHandle);
	ICEDB_DLL_BASE_destroy_vtable(h->_vtable);

	ICEDB_free((void*)h);
}

ICEDB_error_code ICEDB_DLL_BASE_HANDLE_IMPL_open(ICEDB_DLL_BASE_HANDLE *p) {
#if defined(__unix__) || defined(__APPLE__) // Indicates that DLSYM is provided (unix, linux, mac, etc. (sometimes even windows))
	//Check that file exists here
	p->_dlHandle->h = dlopen(p->path, RTLD_LAZY);
	const char* cerror = dlerror(); // This is thread safe.
	if (cerror)
	{
		ICEDB_error_context* e = ICEDB_error_context_create(ICEDB_ERRORCODES_DLLOPEN);
		ICEDB_error_context_add_string2(e, "dlopen-Error-Code", cerror);
		ICEDB_error_context_add_string2(e, "DLL-Path", p->path);
		// Add some dir info.
		const int sz = 500;
		char bAppDir[sz] = "", bCWD[sz] = "", bLibDir[sz] = "";
		ICEDB_getAppDir(sz, bAppDir);
		ICEDB_getCWD(sz, bCWD);
		ICEDB_getLibDir(sz, bLibDir);
		ICEDB_error_context_add_string2(e, "App Dir", bAppDir);
		ICEDB_error_context_add_string2(e, "CWD", bCWD);
		ICEDB_error_context_add_string2(e, "Lib Dir", bLibDir);
		return ICEDB_ERRORCODES_DLLOPEN;
	}
	p->openCount++;
	return ICEDB_ERRORCODES_NONE;
#elif defined(_WIN32)
	CharArrayStr_t ppath = convertCharArrayStr(p->path);
	p->_dlHandle->h = LoadLibrary(ppath);
	delete ppath;
	// Could not open the dll for some reason
	if (p->_dlHandle->h == NULL)
	{
		DWORD err = GetLastError(); // TODO: Thread sync here?
		ICEDB_error_context* e = ICEDB_error_context_create(ICEDB_ERRORCODES_DLLOPEN);
		const int errStrSz = 250;
		char winErrString[errStrSz] = "";
		snprintf(winErrString, errStrSz, "%u", err);
		ICEDB_error_context_add_string2(e, "Win-Error-Code", winErrString);
		ICEDB_error_context_add_string2(e, "DLL-Path", p->path);
		return ICEDB_ERRORCODES_DLLOPEN;
	}
	p->openCount++;
	return ICEDB_ERRORCODES_NONE;
#else
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return ICEDB_ERRORCODES_NONE;
#endif
}
ICEDB_error_code ICEDB_DLL_BASE_HANDLE_IMPL_close(ICEDB_DLL_BASE_HANDLE *p) {
	if (!p->_dlHandle->h) return ICEDB_ERRORCODES_NO_DLHANDLE;
#if defined(__unix__) || defined(__APPLE__)
	dlclose(p->_dlHandle);
#elif defined(_WIN32)
	FreeLibrary(p->_dlHandle->h);
#endif
	p->_dlHandle->h = NULL;
	return ICEDB_ERRORCODES_NONE;
}
uint16_t ICEDB_DLL_BASE_HANDLE_IMPL_isOpen(ICEDB_DLL_BASE_HANDLE *p) { return (p->_dlHandle->h) ? true : false; }
uint16_t ICEDB_DLL_BASE_HANDLE_IMPL_getRefCount(ICEDB_DLL_BASE_HANDLE *p) { return p->refCount; }
void ICEDB_DLL_BASE_HANDLE_IMPL_incRefCount(ICEDB_DLL_BASE_HANDLE *p) { p->refCount++; }
ICEDB_error_code ICEDB_DLL_BASE_HANDLE_IMPL_decRefCount(ICEDB_DLL_BASE_HANDLE *p) {
	if (p->refCount > 0) {
		p->refCount--;
		return ICEDB_ERRORCODES_NONE;
	} else return ICEDB_ERRORCODES_DLL_DEC_REFS_LE_0;
}
void* ICEDB_DLL_BASE_HANDLE_IMPL_getSym(ICEDB_DLL_BASE_HANDLE* p, const char* symbol) {
	if (!p->_dlHandle->h && p->autoOpen) ICEDB_DLL_BASE_HANDLE_IMPL_open(p);
	if (!p->_dlHandle->h) {
		ICEDB_error_context* e = ICEDB_error_context_create(ICEDB_ERRORCODES_NO_DLHANDLE);
		return NULL;
	}
	void* sym = nullptr;
#if defined(__unix__) || defined(__APPLE__)
	sym = dlsym(p->_dlHandle->h, symbol);
#elif defined(_WIN32)
	sym = GetProcAddress(p->_dlHandle->h, symbol);
#endif
	if (!sym)
	{
		ICEDB_error_context* e = ICEDB_error_context_create(ICEDB_ERRORCODES_NO_DLSYMBOL);
		const int errStrSz = 250;
		char ErrString[errStrSz] = "";
#ifdef _WIN32
		DWORD errcode = GetLastError();
		snprintf(ErrString, errStrSz, "%ul", errcode);
		ICEDB_error_context_add_string2(e, "Win-Error-Code", ErrString);
#endif

		ICEDB_error_context_add_string2(e, "Symbol-Name", symbol);
		ICEDB_error_context_add_string2(e, "DLL-path", p->path);
	}
	return (void*)sym;
}
const char* ICEDB_DLL_BASE_HANDLE_IMPL_getPath(ICEDB_DLL_BASE_HANDLE* p) { return p->path; }
ICEDB_error_code ICEDB_DLL_BASE_HANDLE_IMPL_setPath(ICEDB_DLL_BASE_HANDLE* p, const char* filename) {
	size_t sz = strlen(filename);
	if (p->path) ICEDB_free((void*)p->path);
	p->path = ICEDB_COMPAT_strdup_s(filename, sz);
	return ICEDB_ERRORCODES_NONE;
}
void ICEDB_DLL_BASE_HANDLE_IMPL_set_autoopen(ICEDB_DLL_BASE_HANDLE* p, bool val) {p->autoOpen = val;}
bool ICEDB_DLL_BASE_HANDLE_IMPL_get_autoopen(ICEDB_DLL_BASE_HANDLE* p) {return p->autoOpen;}
void ICEDB_DLL_BASE_HANDLE_IMPL_RAISE_EXCEPTION(ICEDB_DLL_BASE_HANDLE*, const char* filename, int line, const char* sym) {
	ICEDB_DEBUG_RAISE_EXCEPTION_HANDLER(filename, line, sym);
}

ICEDB_CALL_C DL_ICEDB ICEDB_DLL_BASE_HANDLE_vtable* ICEDB_DLL_BASE_create_vtable() {
	ICEDB_DLL_BASE_HANDLE_vtable* res = (ICEDB_DLL_BASE_HANDLE_vtable*) ICEDB_malloc(sizeof (ICEDB_DLL_BASE_HANDLE_vtable));
	res->close = ICEDB_DLL_BASE_HANDLE_IMPL_close;
	res->decRefCount = ICEDB_DLL_BASE_HANDLE_IMPL_decRefCount;
	res->getPath = ICEDB_DLL_BASE_HANDLE_IMPL_getPath;
	res->getRefCount = ICEDB_DLL_BASE_HANDLE_IMPL_getRefCount;
	res->getSym = ICEDB_DLL_BASE_HANDLE_IMPL_getSym;
	res->incRefCount = ICEDB_DLL_BASE_HANDLE_IMPL_incRefCount;
	res->isOpen = ICEDB_DLL_BASE_HANDLE_IMPL_isOpen;
	res->open = ICEDB_DLL_BASE_HANDLE_IMPL_open;
	res->setPath = ICEDB_DLL_BASE_HANDLE_IMPL_setPath;
	res->getAutoOpen = ICEDB_DLL_BASE_HANDLE_IMPL_get_autoopen;
	res->setAutoOpen = ICEDB_DLL_BASE_HANDLE_IMPL_set_autoopen;
	res->_raiseExcept = ICEDB_DLL_BASE_HANDLE_IMPL_RAISE_EXCEPTION;
	return res;
}

ICEDB_CALL_C DL_ICEDB void ICEDB_DLL_BASE_destroy_vtable(ICEDB_DLL_BASE_HANDLE_vtable* h) {
	ICEDB_free((void*)h);
}

ICEDB_CALL_C DL_ICEDB void ICEDB_register_interface(const char* topic, int priority, const char* path) {
	std::string sTopic(topic);
	std::string sPath(path);
	using namespace icedb::dll::impl;
	if (!topicMaps.count(sTopic)) {
		topicMaps[sTopic] = std::multimap<int,std::string>();
	}
	std::multimap<int, std::string> &tMap = topicMaps[sTopic];
	tMap.emplace(std::pair<int,std::string>(priority,sPath));
}
ICEDB_CALL_C DL_ICEDB void ICEDB_unregister_interface(const char* topic, int priority, const char* path) {
	std::string sTopic(topic);
	std::string sPath(path);
	using namespace icedb::dll::impl;
	if (!topicMaps.count(sTopic)) {
		topicMaps[sTopic] = std::multimap<int, std::string>();
	}
	std::multimap<int, std::string> &tMap = topicMaps[sTopic];
	std::pair<int, std::string> p(priority, sPath);
	auto it = tMap.find(priority);
	if (it != tMap.end()) tMap.erase(it);
}
ICEDB_CALL_C DL_ICEDB ICEDB_query_interface_res_t ICEDB_query_interface(const char* topic) {
	std::string sTopic(topic);
	using namespace icedb::dll::impl;
	if (!topicMaps.count(sTopic)) {
		topicMaps[sTopic] = std::multimap<int, std::string>();
	}
	std::multimap<int, std::string> &tMap = topicMaps[sTopic];
	// Returning a null-terminated array of pointers to const char*s.
	char** res = (char**) ICEDB_malloc(sizeof(char*) * (tMap.size() + 1));
	int i = 0;
	for (auto it = tMap.begin(); it != tMap.end(); ++it) {
		char* s = ICEDB_COMPAT_strdup_s(it->second.c_str(), it->second.size());
		res[i] = s;
		++i;
	}
	res[i] = nullptr;
	return res;
}
ICEDB_CALL_C DL_ICEDB void ICEDB_query_interface_free(ICEDB_query_interface_res_t p) {
	// Free a null-terminated list
	int i = 0;
	while (p[i]) {
		ICEDB_free(p[i]);
		++i;
	}
	ICEDB_free(p);
}

ICEDB_CALL_C DL_ICEDB size_t ICEDB_dll_name_mangle_simple(const char* libname, char* out, size_t maxsz) {
	std::string res;
	if (!libname || !out) ICEDB_DEBUG_RAISE_EXCEPTION();
#if defined(_WIN32)
	// X.dll
	res = std::string(libname);
	res.append(".dll");
#elif defined(__APPLE__)
	// libX.dylib
	res = "lib" + std::string(libname) + ".dylib";
#else
	// libX.so
	res = "lib" + std::string(libname) + ".so";
#endif
	ICEDB_COMPAT_strncpy_s(out, maxsz, res.c_str(), res.size() + 1);
	return (maxsz > res.size()+1) ? maxsz : res.size() + 1;
}

ICEDB_CALL_C DL_ICEDB bool ICEDB_load_plugin(const char* dlpath) {
	if (icedb::dll::impl::pluginHandles.count(std::string(dlpath))) return true;

	ICEDB_DLL_BASE_HANDLE* dllInst = ICEDB_DLL_BASE_HANDLE_create(dlpath);
	auto td = create_icedb_plugin_base(dllInst);
	if((td->_base->_vtable->open(td->_base))) return false;

	icedb::versioning::versionInfo_p libver;
	// Try to bind the functions. If failure, unload the dll.
	if (!td->Bind_GetVerInfo(td)) goto failed;
	if (!td->Bind_Register(td)) goto failed;
	if (!td->Bind_Unregister(td)) goto failed;

	auto pver = td->GetVerInfo(td);
	libver = icedb::versioning::getLibVersionInfo();
	ICEDB_ver_match compat_level = icedb::versioning::compareVersions(pver->p, libver);
	if (compat_level == ICEDB_VER_INCOMPATIBLE) goto failed;

	icedb::dll::impl::_init();
	bool res = td->Register(td, ICEDB_register_interface, ICEDB_findModuleByFunc, icedb::dll::impl::libInst);
	if (!res) goto failed;
	destroy_icedb_plugin_base(td);
	icedb::dll::impl::pluginHandles[std::string(dlpath)] = dllInst;
	return true;
failed:
	td->_base->_vtable->close(td->_base);
	destroy_icedb_plugin_base(td);
	ICEDB_DLL_BASE_HANDLE_destroy(dllInst);
	// Return an error code indicating that the load was unsuccessful
	ICEDB_error_context *cxt = ICEDB_error_context_create(ICEDB_ERRORCODES_BAD_PLUGIN);
	ICEDB_error_context_add_string2(cxt, "dlpath", dlpath);
	return false;
}
ICEDB_CALL_C DL_ICEDB bool ICEDB_unload_plugin(const char* dlpath) {
	if (!icedb::dll::impl::pluginHandles.count(std::string(dlpath))) return true;
	ICEDB_DLL_BASE_HANDLE* dllInst = icedb::dll::impl::pluginHandles.at(std::string(dlpath));
	auto td = create_icedb_plugin_base(dllInst);
	td->Unregister(td, ICEDB_unregister_interface, ICEDB_findModuleByFunc);
	destroy_icedb_plugin_base(td);
	icedb::dll::impl::pluginHandles.erase(std::string(dlpath));
	ICEDB_DLL_BASE_HANDLE_destroy(dllInst);
	return true;
}






ICEDB_CALL_CPP DL_ICEDB icedb::dll::Dll_Base_Handle::Dll_Base_Handle(base_pointer_type& r)
	:_base(nullptr, ICEDB_DLL_BASE_HANDLE_destroy){
	_base.swap(r);
}
ICEDB_CALL_CPP DL_ICEDB icedb::dll::Dll_Base_Handle::~Dll_Base_Handle() {}

ICEDB_CALL_CPP DL_ICEDB ICEDB_error_code icedb::dll::Dll_Base_Handle::open() { return _base->_vtable->open(_base.get()); }
ICEDB_CALL_CPP DL_ICEDB ICEDB_error_code icedb::dll::Dll_Base_Handle::close() { return _base->_vtable->close(_base.get()); }
ICEDB_CALL_CPP DL_ICEDB uint16_t icedb::dll::Dll_Base_Handle::isOpen() const { return _base->_vtable->isOpen(_base.get()); }
ICEDB_CALL_CPP DL_ICEDB uint16_t icedb::dll::Dll_Base_Handle::getRefCount() const { return _base->_vtable->getRefCount(_base.get()); }
ICEDB_CALL_CPP DL_ICEDB void icedb::dll::Dll_Base_Handle::incRefCount() { _base->_vtable->incRefCount(_base.get()); }
ICEDB_CALL_CPP DL_ICEDB ICEDB_error_code icedb::dll::Dll_Base_Handle::decRefCount() { return _base->_vtable->decRefCount(_base.get()); }
ICEDB_CALL_CPP DL_ICEDB void icedb::dll::Dll_Base_Handle::setAutoOpen(bool val) { _base->_vtable->setAutoOpen(_base.get(), val); }
ICEDB_CALL_CPP DL_ICEDB bool icedb::dll::Dll_Base_Handle::getAutoOpen() const { return _base->_vtable->getAutoOpen(_base.get()); }
ICEDB_CALL_CPP DL_ICEDB void* icedb::dll::Dll_Base_Handle::getSym(const char* val) { return _base->_vtable->getSym(_base.get(), val); }
ICEDB_CALL_CPP DL_ICEDB const char* icedb::dll::Dll_Base_Handle::getPath() const { return _base->_vtable->getPath(_base.get()); }
ICEDB_CALL_CPP DL_ICEDB ICEDB_error_code icedb::dll::Dll_Base_Handle::setPath(const char* val) { return _base->_vtable->setPath(_base.get(), val); }
ICEDB_CALL_CPP DL_ICEDB icedb::dll::Dll_Base_Handle::pointer_type icedb::dll::Dll_Base_Handle::generate(const char* filename) {
	base_pointer_type bp(ICEDB_DLL_BASE_HANDLE_create(filename), ICEDB_DLL_BASE_HANDLE_destroy);
	pointer_type p( new Dll_Base_Handle(bp));
	return p;
}
ICEDB_CALL_CPP DL_ICEDB icedb::dll::Dll_Base_Handle::pointer_type
icedb::dll::Dll_Base_Handle::generate(ICEDB_DLL_BASE_HANDLE* h) {
	base_pointer_type bp(h, ICEDB_DLL_BASE_HANDLE_destroy);
	pointer_type p(new Dll_Base_Handle(bp));
	return p;
}
ICEDB_DLL_BASE_HANDLE* icedb::dll::Dll_Base_Handle::getBase() { return _base.get(); }
