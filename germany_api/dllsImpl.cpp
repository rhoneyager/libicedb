#include "dllsImpl.h"
#include "mem.h"
#include <stdarg.h>
#include <functional>

ICEDB_CALL_C DL_ICEDB ICEDB_DLL_BASE_HANDLE_vtable* ICEDB_DLL_BASE_create_vtable() {
	ICEDB_DLL_BASE_HANDLE_vtable* res = (ICEDB_DLL_BASE_HANDLE_vtable*) ICEDB_malloc(sizeof ICEDB_DLL_BASE_HANDLE_vtable);
	res->close = NULL;
	res->decRefCount = NULL;
	res->getPath = NULL;
	res->getRefCount = NULL;
	res->getSym = NULL;
	res->incRefCount = NULL;
	res->isOpen = NULL;
	res->open = NULL;
	res->setPath = NULL;
	res->_ctor = NULL;
	return res;
}

template<class InterfaceType, class ReturnType, class A>
ReturnType TestBind(InterfaceType *iface, A) {
	return (ReturnType) 42;
}

ICEDB_CALL_CPP PRIVATE_ICEDB struct _pimpl_interface_testdll {
public:
	_pimpl_interface_testdll(interface_testdll* obj) {
		obj->status_m_testNum = ICEDB_DLL_FUNCTION_LOADED;
		obj->m_testNum = NULL;
		//interface_testdll::F_TYPE_testNum
		interface_testdll::F_TYPE_testNum a = TestBind<interface_testdll,int,int>;
		obj->testNum = a;
	}
	~_pimpl_interface_testdll() {}
};

// Odd wrapping scheme to accomodate differences between C and C++ structs.
ICEDB_CALL_C HIDDEN_ICEDB struct _impl_interface_testdll {
	_pimpl_interface_testdll* p;
};

ICEDB_CALL_C DL_ICEDB interface_testdll* create_testdll(ICEDB_DLL_BASE_HANDLE *base) {
	//if (!base) ICEDB_DEBUG_RAISE_EXCEPTION();
	interface_testdll* p = (interface_testdll*) ICEDB_malloc(sizeof interface_testdll);
	// The easy way to make sure everything is NULL.
	// This is needed because the member variables / indicator flags must be set to null.
	memset(p, NULL, sizeof(interface_testdll));
	//p->_base = base;
	//p->_base->_vtable->incRefCount(p->_base);
	p->_p = (_impl_interface_testdll*)ICEDB_malloc(sizeof _impl_interface_testdll);
	p->_p->p = new _pimpl_interface_testdll(p);
	return p;
}

ICEDB_CALL_C DL_ICEDB void destroy_testdll(interface_testdll* p) {
	delete p->_p->p;
	ICEDB_free(p->_p);
	//p->_base->_vtable->decRefCount(p->_base);
	ICEDB_free(p);
}

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
