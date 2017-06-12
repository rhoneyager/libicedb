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
