#include "dllsImpl.h"
#include "mem.h"
#include "error_context.h"
#include "error.h"
#include "util.h"
#include <stdarg.h>
#include <functional>
#include <mutex>

ICEDB_CALL_C DL_ICEDB ICEDB_DLL_BASE_HANDLE* ICEDB_DLL_BASE_HANDLE_create(const char* filename) {
	ICEDB_DLL_BASE_HANDLE *res = (ICEDB_DLL_BASE_HANDLE*)ICEDB_malloc(sizeof ICEDB_DLL_BASE_HANDLE);
	res->refCount = 0;
	res->dlHandle = NULL;
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
	ICEDB_DLL_BASE_destroy_vtable(h->_vtable);
	ICEDB_free((void*)h);
}

dlHandleType ICEDB_DLL_BASE_HANDLE_IMPL_open(ICEDB_DLL_BASE_HANDLE *p) {
#if defined(__unix__) // Indicates that DLSYM is provided (unix, linux, mac, etc. (sometimes even windows))
	//Check that file exists here
	p->dlHandle = dlopen(p->path, RTLD_LAZY);
	const char* cerror = dlerror(); // This is thread safe.
	if (cerror)
	{
		ICEDB_error_context* e = ICEDB_error_context_create(ICEDB_ERRORCODES_DLLOPEN);
		ICEDB_error_context_add_string2(e, "dlopen-Error-Code", cerror);
		ICEDB_error_context_add_string2(e, "DLL-Path", p->path);
	}
	return p->dlHandle;
#elif defined(_WIN32)
	p->dlHandle = LoadLibrary(p->path);
	// Could not open the dll for some reason
	if (p->dlHandle == NULL)
	{
		DWORD err = GetLastError(); // TODO: Thread sync here?
		ICEDB_error_context* e = ICEDB_error_context_create(ICEDB_ERRORCODES_DLLOPEN);
		const int errStrSz = 250;
		char winErrString[errStrSz] = "";
		snprintf(winErrString, errStrSz, "%u", err);
		ICEDB_error_context_add_string2(e, "Win-Error-Code", winErrString);
		ICEDB_error_context_add_string2(e, "DLL-Path", p->path);
	}
	return p->dlHandle;
#endif
}
void ICEDB_DLL_BASE_HANDLE_IMPL_close(ICEDB_DLL_BASE_HANDLE *p) {
	if (!p->dlHandle) return;
#if defined(__unix__)
	dlclose(p->dlHandle);
#elif defined(_WIN32)
	FreeLibrary(p->dlHandle);
#endif
	p->dlHandle = NULL;
}
bool ICEDB_DLL_BASE_HANDLE_IMPL_isOpen(ICEDB_DLL_BASE_HANDLE *p) { return (p->dlHandle) ? true : false; }
uint16_t ICEDB_DLL_BASE_HANDLE_IMPL_getRefCount(ICEDB_DLL_BASE_HANDLE *p) { return p->refCount; }
void ICEDB_DLL_BASE_HANDLE_IMPL_incRefCount(ICEDB_DLL_BASE_HANDLE *p) { p->refCount++; }
void ICEDB_DLL_BASE_HANDLE_IMPL_decRefCount(ICEDB_DLL_BASE_HANDLE *p) { p->refCount--; }
void* ICEDB_DLL_BASE_HANDLE_IMPL_getSym(ICEDB_DLL_BASE_HANDLE* p, const char* symbol) {
	if (!p->dlHandle) {
		ICEDB_error_context* e = ICEDB_error_context_create(ICEDB_ERRORCODES_NO_DLHANDLE);
		return NULL;
	}
	void* sym = nullptr;
#if defined(__unix__)
	sym = dlsym(p->dlHandle, symbol);
#elif defined(_WIN32)
	sym = GetProcAddress(p->dlHandle, symbol);
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
void ICEDB_DLL_BASE_HANDLE_IMPL_setPath(ICEDB_DLL_BASE_HANDLE* p, const char* filename) {
	size_t sz = strlen(filename);
	if (p->path) ICEDB_free((void*)p->path);
	p->path = ICEDB_COMPAT_strdup_s(filename, sz);
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



namespace icedb {
	namespace dll {
		namespace binding {

			// Cannot be a member inside the struct. May decide to place in a namespace.
			template<class InterfaceType, class SymbolName, class ReturnType, class ...Args>
			ReturnType TestBind(InterfaceType *iface, Args... args) {
				if (iface->status_m_testNum != ICEDB_DLL_FUNCTION_LOADED) {
					iface->m_testNum = (interface_testdll::TYPE_testNum)
						iface->_base->_vtable->getSym(iface->_base, SymbolName::Symbol());
					iface->status_m_testNum = ICEDB_DLL_FUNCTION_LOADED;
				}
				return (ReturnType)iface->m_testNum(args...);
			}


		}
	}
}

namespace _pimpl_interface_testdll_nm {
	
	ICEDB_CALL_CPP PRIVATE_ICEDB struct _pimpl_interface_testdll {
	public:
		struct tfname_testfunc { static const char* Symbol() { return "testfunc"; } };

		_pimpl_interface_testdll(interface_testdll* obj) {
			obj->status_m_testNum = ICEDB_DLL_FUNCTION_UNLOADED;
			obj->m_testNum = NULL;
			interface_testdll::F_TYPE_testNum a = 
				icedb::dll::binding::TestBind<interface_testdll, tfname_testfunc, int, int>;
			obj->testNum = a;
		}
		~_pimpl_interface_testdll() {}
	};
}

// Odd wrapping scheme to accomodate differences between C and C++ structs.
ICEDB_CALL_C HIDDEN_ICEDB struct _impl_interface_testdll {
	_pimpl_interface_testdll_nm::_pimpl_interface_testdll* p;
};

ICEDB_CALL_C DL_ICEDB interface_testdll* create_testdll(ICEDB_DLL_BASE_HANDLE *base) {
	if (!base) ICEDB_DEBUG_RAISE_EXCEPTION();
	interface_testdll* p = (interface_testdll*) ICEDB_malloc(sizeof interface_testdll);
	memset(p, NULL, sizeof(interface_testdll));
	p->_base = base;
	p->_base->_vtable->incRefCount(p->_base);
	p->_p = (_impl_interface_testdll*)ICEDB_malloc(sizeof _impl_interface_testdll);
	p->_p->p = new _pimpl_interface_testdll_nm::_pimpl_interface_testdll(p);
	return p;
}

ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(testdll);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, int, testNum, int);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(testdll);
//ICEDB_BEGIN_DECL
//ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, int, testNum, int);
//ICEDB_END_DECL

/*
template <class InterfaceType, class ReturnType, class ... InputTypes>
void generateBindingFunction(const char* SymbolName, InterfaceType* InterfacePtr,
	std::function<ReturnType(InputTypes)> &f)
{
	if (InterfacePtr->_base) {
		if (ICEDB_DLL_FUNCTION_LOADED != InterfacePtr->status_m_testNum) { // replace the symbol name here
			InterfacePtr->m_testNum = 0;
			InterfacePtr->m_testNum = (TYPE_testNum)InterfacePtr->_base->_vtable->getSym(InterfacePtr->_base, SymbolName);
			InterfacePtr->status_m_testNum = ICEDB_DLL_FUNCTION_LOADED;
		}
	}
}


// Creating a template that will properly handle the return type.
template <class InterfaceType, class ReturnType, class ... InputTypes>
ReturnType process(InterfaceType* InterfacePtr, InputTypes... i) {
	if (InterfacePtr->_base) {
		if (ICEDB_DLL_FUNCTION_LOADED != InterfacePtr->status_m_testNum) {
			InterfacePtr->m_testNum = 0;
			InterfacePtr->m_testNum = (TYPE_testNum)InterfacePtr->_base->_vtable->getSym(InterfacePtr->_base, "testNum");
			InterfacePtr->status_m_testNum = ICEDB_DLL_FUNCTION_LOADED;
		}
		ReturnType res;
		//if (0 != InterfacePtr->m_testNum) 
		//	res = InterfacePtr->m_testNum(int);
		return res;
	return (ReturnType)(NULL);
}

template DL_ICEDB void process<interface_testdll, void, int, int, char>
	(interface_testdll*, int, int, char);

DL_ICEDB void a(interface_testdll *InterfacePtr) {
	process<interface_testdll, void, int, int, char>(InterfacePtr, 0, 0, 'a');
}
*/
