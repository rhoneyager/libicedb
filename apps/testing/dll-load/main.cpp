#include <iostream>
#include <functional>
#include "../../../germany_api/germany/error.hpp"
#include "../../../germany_api/germany/error.h"
#include "../../../germany_api/germany/error_context.h"
#include "../../../germany_api/germany/mem.h"
#include "../../../germany_api/germany/dlls.hpp"
#include "../../../germany_api/germany/dllsImpl.hpp"

ICEDB_DLL_INTERFACE_BEGIN(testdll)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(testdll, setint, int, int)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(testdll, getint, int)
ICEDB_DLL_INTERFACE_END

ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(testdll);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(testdll, setint, "set", int, int);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(testdll, getint, "get", int);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(testdll);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, setint, int, int);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, getint, int);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(testdll);


ICEDB_DLL_CPP_INTERFACE_BEGIN(testdllcpp, testdll)
ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(testdllcpp, setint, int, int)
ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(testdllcpp, getint, int)
ICEDB_DLL_CPP_INTERFACE_END

ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_BEGIN(testdllcpp, testdll)
ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, getint, int)
ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, setint, int, int)
ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_END


int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: %s (testdll path)\n",argv[0]);
		return 1;
	}
	const char* dlpath = argv[1];
	auto dllInst1 = ICEDB_DLL_BASE_HANDLE_create(dlpath);
	auto td1a = create_testdll(dllInst1);
#define MUST_RETURN(x) if(x) { \
		auto cxt = icedb::error::get_error_context_thread_local(); \
		if (cxt) printf("%s", icedb::error::stringify(cxt).c_str()); \
		ICEDB_DEBUG_RAISE_EXCEPTION(); \
	}
	MUST_RETURN(td1a->_base->_vtable->open(td1a->_base));
	auto td1b = create_testdll(dllInst1);
	//MUST_RETURN(td1b->_base->_vtable->open(td1b->_base));

	auto dllInst2 = ::icedb::dll::Dll_Base_Handle::generate(dlpath);
	auto td2a = testdllcpp::generate(dllInst2);
	//MUST_RETURN(td2a->getDll()->open());
	auto td2b = testdllcpp::generate(dllInst2);
	//MUST_RETURN(td2b->getDll()->open());
	
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
