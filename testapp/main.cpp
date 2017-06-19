#include <iostream>
#include "../germany_api/error.hpp"
#include "../germany_api/error.h"
#include "../germany_api/error_context.h"
#include "../germany_api/mem.h"
#include "../germany_api/dlls.hpp"
#include "../germany_api/dllsImpl.hpp"

ICEDB_DLL_INTERFACE_BEGIN(testdll)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(testdll, int, setint, int)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(testdll, int, getint)
ICEDB_DLL_INTERFACE_END


ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(testdll);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(testdll, int, setint, "set", int);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(testdll, int, getint, "get");
ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(testdll);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, int, setint, int);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, int, getint);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(testdll);

ICEDB_DLL_CPP_INTERFACE_BEGIN(testdllcpp)
ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(testdllcpp, int, setint, int)
ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(testdllcpp, int, getint)
ICEDB_DLL_CPP_INTERFACE_END

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
	auto dllInst1 = ICEDB_DLL_BASE_HANDLE_create("testdll.dll");
	auto td1a = create_testdll(dllInst1);
	if (td1a->_base->_vtable->open(td1a->_base)) {
		auto cxt = icedb::error::get_error_context_thread_local();
		if (cxt)
			printf("%s", icedb::error::stringify(cxt).c_str());
		else ICEDB_DEBUG_RAISE_EXCEPTION();
	}
	auto td1b = create_testdll(dllInst1);
	if (td1b->_base->_vtable->open(td1b->_base)) {
		auto cxt = icedb::error::get_error_context_thread_local();
		if (cxt)
			printf("%s", icedb::error::stringify(cxt).c_str());
		else ICEDB_DEBUG_RAISE_EXCEPTION();
	}

	auto dllInst2 = ::icedb::dll::Dll_Base_Handle::generate("testdll.dll");
	auto td2a = testdllcpp::generate(dllInst2);
	if (td2a->getDll()->open()) {
		auto cxt = icedb::error::get_error_context_thread_local();
		if (cxt)
			printf("%s", icedb::error::stringify(cxt).c_str());
		else ICEDB_DEBUG_RAISE_EXCEPTION();
	}
	auto td2b = testdllcpp::generate(dllInst2);
	if (td2b->getDll()->open()) {
		auto cxt = icedb::error::get_error_context_thread_local();
		if (cxt)
			printf("%s", icedb::error::stringify(cxt).c_str());
		else ICEDB_DEBUG_RAISE_EXCEPTION();
	}

	using namespace std;
	td1a->setint(td1a, 3);
	cout << "td1a set to " << td1a->getint(td1a) << endl;
	cout << "td1b is " << td1b->getint(td1b) << endl;
	td2a->setint(5);
	cout << "td2a set to " << td2a->getint() << endl;
	cout << "td1a is " << td1a->getint(td1a) << endl;

	destroy_testdll(td1a);
	destroy_testdll(td1b);
	ICEDB_DLL_BASE_HANDLE_destroy(dllInst1);
	return 0;
}
