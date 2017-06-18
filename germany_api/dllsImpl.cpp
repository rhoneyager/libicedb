#include "dllsImpl.h"
#include "dllsImpl.hpp"
#include "mem.h"
#include "error_context.h"
#include "error.h"
#include "util.h"
#include <stdarg.h>
#include <functional>
#include <mutex>
#include <typeinfo>

ICEDB_CALL_C DL_ICEDB ICEDB_DLL_BASE_HANDLE* ICEDB_DLL_BASE_HANDLE_create(const char* filename) {
	ICEDB_DLL_BASE_HANDLE *res = (ICEDB_DLL_BASE_HANDLE*)ICEDB_malloc(sizeof ICEDB_DLL_BASE_HANDLE);
	res->refCount = 0;
	res->_dlHandle = (_dlHandleType_impl*) ICEDB_malloc(sizeof(_dlHandleType_impl));
	if (!res->_dlHandle) ICEDB_DEBUG_RAISE_EXCEPTION();
	res->_dlHandle->h = NULL;
	res->path = NULL;
	res->_vtable = ICEDB_DLL_BASE_create_vtable();
	res->_vtable->setPath(res, filename);
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
#if defined(__unix__) // Indicates that DLSYM is provided (unix, linux, mac, etc. (sometimes even windows))
	//Check that file exists here
	p->_dlHandle->h = dlopen(p->path, RTLD_LAZY);
	const char* cerror = dlerror(); // This is thread safe.
	if (cerror)
	{
		ICEDB_error_context* e = ICEDB_error_context_create(ICEDB_ERRORCODES_DLLOPEN);
		ICEDB_error_context_add_string2(e, "dlopen-Error-Code", cerror);
		ICEDB_error_context_add_string2(e, "DLL-Path", p->path);
		return ICEDB_ERRORCODES_DLLOPEN;
	}
	return ICEDB_ERRORCODES_NONE;
#elif defined(_WIN32)
	p->_dlHandle->h = LoadLibrary(p->path);
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
	return ICEDB_ERRORCODES_NONE;
#endif
}
ICEDB_error_code ICEDB_DLL_BASE_HANDLE_IMPL_close(ICEDB_DLL_BASE_HANDLE *p) {
	if (!p->_dlHandle->h) return ICEDB_ERRORCODES_NO_DLHANDLE;
#if defined(__unix__)
	dlclose(p->dlHandle);
#elif defined(_WIN32)
	FreeLibrary(p->_dlHandle->h);
#endif
	p->_dlHandle->h = NULL;
	return ICEDB_ERRORCODES_NONE;
}
bool ICEDB_DLL_BASE_HANDLE_IMPL_isOpen(ICEDB_DLL_BASE_HANDLE *p) { return (p->_dlHandle->h) ? true : false; }
uint16_t ICEDB_DLL_BASE_HANDLE_IMPL_getRefCount(ICEDB_DLL_BASE_HANDLE *p) { return p->refCount; }
void ICEDB_DLL_BASE_HANDLE_IMPL_incRefCount(ICEDB_DLL_BASE_HANDLE *p) { p->refCount++; }
ICEDB_error_code ICEDB_DLL_BASE_HANDLE_IMPL_decRefCount(ICEDB_DLL_BASE_HANDLE *p) {
	if (p->refCount > 0) {
		p->refCount--;
		return ICEDB_ERRORCODES_NONE;
	} else return ICEDB_ERRORCODES_DLL_DEC_REFS_LE_0;
}
void* ICEDB_DLL_BASE_HANDLE_IMPL_getSym(ICEDB_DLL_BASE_HANDLE* p, const char* symbol) {
	if (!p->_dlHandle->h) {
		ICEDB_error_context* e = ICEDB_error_context_create(ICEDB_ERRORCODES_NO_DLHANDLE);
		return NULL;
	}
	void* sym = nullptr;
#if defined(__unix__)
	sym = dlsym(p->_dlHandle->h, symbol);
#elif defined(_WIN32)
	sym = GetProcAddress(p->_dlHandle->h, symbol);
#endif
	if (!sym)
	{
#ifdef _WIN32
		DWORD errcode = GetLastError();
#endif
		ICEDB_error_context* e = ICEDB_error_context_create(ICEDB_ERRORCODES_NO_DLSYMBOL);
		const int errStrSz = 250;
		char ErrString[errStrSz] = "";
		snprintf(ErrString, errStrSz, "%ul", errcode);

		ICEDB_error_context_add_string2(e, "Symbol-Name", symbol);
		ICEDB_error_context_add_string2(e, "DLL-path", p->path);
#ifdef _WIN32
			ICEDB_error_context_add_string2(e, "Win-Error-Code", ErrString);
#endif
			;
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

ICEDB_CALL_C DL_ICEDB ICEDB_DLL_BASE_HANDLE_vtable* ICEDB_DLL_BASE_create_vtable() {
	ICEDB_DLL_BASE_HANDLE_vtable* res = (ICEDB_DLL_BASE_HANDLE_vtable*) ICEDB_malloc(sizeof ICEDB_DLL_BASE_HANDLE_vtable);
	res->close = ICEDB_DLL_BASE_HANDLE_IMPL_close;
	res->decRefCount = ICEDB_DLL_BASE_HANDLE_IMPL_decRefCount;
	res->getPath = ICEDB_DLL_BASE_HANDLE_IMPL_getPath;
	res->getRefCount = ICEDB_DLL_BASE_HANDLE_IMPL_getRefCount;
	res->getSym = ICEDB_DLL_BASE_HANDLE_IMPL_getSym;
	res->incRefCount = ICEDB_DLL_BASE_HANDLE_IMPL_incRefCount;
	res->isOpen = ICEDB_DLL_BASE_HANDLE_IMPL_isOpen;
	res->open = ICEDB_DLL_BASE_HANDLE_IMPL_open;
	res->setPath = ICEDB_DLL_BASE_HANDLE_IMPL_setPath;
	return res;
}

ICEDB_CALL_C DL_ICEDB void ICEDB_DLL_BASE_destroy_vtable(ICEDB_DLL_BASE_HANDLE_vtable* h) {
	ICEDB_free((void*)h);
}

