#include "../germany_api/error.h"
#include "../germany_api/error_context.h"
#include "../germany_api/mem.h"
#include "../germany_api/dllsImpl.hpp"
#include "../germany_api/error.hpp"
#include <stdio.h>
#include <iostream>


/* struct _pimpl_interface_##INTERFACENAME {
		~_pinpl_interface_##INTERFACENAME() {}
		/// Repeat for each defined function
		struct tfname_##FUNCNAME { static const char* Symbol() { return "##FUNCNAME"; } };
		/// Then comes the constructor function. Sets values to zero (ignorable via memset),
		/// but then binds the individual functions.... refers to the struct tfname_##FUNCNAME
}
*/

/*namespace _pimpl_interface_nm_testdll {
	ICEDB_CALL_CPP PRIVATE_ICEDB struct _pimpl_interface_testdll {
	public:
		~_pimpl_interface_testdll() {}
		_pimpl_interface_testdll(interface_testdll* obj) {
			obj->status_m_testNum = ICEDB_DLL_FUNCTION_UNLOADED;
			obj->m_testNum = NULL;
			struct tfname_testfunc { static const char* Symbol() { return "testfunc"; } };
			obj->testNum = icedb::dll::binding::DoBind<interface_testdll, tfname_testfunc, int, int>;
		}
	};
}*/

// Odd wrapping scheme to accomodate differences between C and C++ structs.
/*ICEDB_CALL_C HIDDEN_ICEDB struct _impl_interface_testdll {
//	_pimpl_interface_testdll_nm::_pimpl_interface_testdll* p;
};*/

/*ICEDB_CALL_C DL_ICEDB interface_testdll* create_testdll(ICEDB_DLL_BASE_HANDLE *base) {
	if (!base) ICEDB_DEBUG_RAISE_EXCEPTION();
	interface_testdll* p = (interface_testdll*)ICEDB_malloc(sizeof interface_testdll);
	memset(p, NULL, sizeof(interface_testdll));
	p->_base = base;
	p->_base->_vtable->incRefCount(p->_base);
	p->_p = (_impl_interface_testdll*)ICEDB_malloc(sizeof _impl_interface_testdll);
	p->_p->p = new _pimpl_interface_testdll_nm::_pimpl_interface_testdll(p);
	return p;
}
*/


ICEDB_DLL_INTERFACE_BEGIN(testdll)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(testdll, int, testfunc, int)
ICEDB_DLL_INTERFACE_END


ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(testdll);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, int, testfunc, int);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(testdll);


int main(int, char**) {
	/*
	ICEDB_error_code code = ICEDB_error_test();
	if (code) {
		ICEDB_error_context *cxt = ICEDB_get_error_context_thread_local();
		char buf[5000] = "\0";
		ICEDB_error_context_to_message(cxt, 5000, buf);
		printf("%s", buf);
		ICEDB_error_context_deallocate(cxt);
	}
	*/
	auto dll = ICEDB_DLL_BASE_HANDLE_create("testdll.dll");
	auto td = create_testdll(dll);
	if (td->_base->_vtable->open(td->_base)) {
		auto cxt = icedb::error::get_error_context_thread_local();
		if (cxt)
			printf("%s", icedb::error::stringify(cxt).c_str());
		else ICEDB_DEBUG_RAISE_EXCEPTION();
	}
	int res = td->testfunc(td, 36);
	printf("Res is %d.\n", res);
	destroy_testdll(td);
	ICEDB_DLL_BASE_HANDLE_destroy(dll);
	return 0;
}
