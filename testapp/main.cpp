#include <iostream>
#include "../germany_api/error.hpp"
#include "../germany_api/error.h"
#include "../germany_api/error_context.h"
#include "../germany_api/mem.h"
#include "../germany_api/dllsImpl.hpp"


ICEDB_DLL_INTERFACE_BEGIN(testdll)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(testdll, int, testfunc, int)
ICEDB_DLL_INTERFACE_END


ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(testdll);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(testdll, int, testfunc, int);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(testdll);
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
	//auto a = &(td->_p->p->sym_testfunc);
	//auto b = td->_p->p->Access_Sym_testfunc(td);
	//std::cout << " a " << a << " b " << b << std::endl;

	int res = td->testfunc(td, 36);
	std::cout << "Res is " << res << std::endl;
	destroy_testdll(td);
	ICEDB_DLL_BASE_HANDLE_destroy(dll);
	return 0;
}
