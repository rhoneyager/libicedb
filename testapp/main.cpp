#include <iostream>
#include "../germany_api/error.hpp"
#include "../germany_api/error.h"
#include "../germany_api/error_context.h"
#include "../germany_api/mem.h"
#include "../germany_api/dllsImpl.hpp"

/*
ICEDB_DLL_INTERFACE_BEGIN(testdll)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(testdll, int, testfunc, int)
ICEDB_DLL_INTERFACE_END


ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(testdll);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(testdll, int, testfunc, int);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(testdll);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(testdll, int, testfunc, int);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(testdll);
*/

struct interface_testdll; extern "C" __declspec(dllexport) interface_testdll* create_testdll(ICEDB_DLL_BASE_HANDLE *); extern "C" __declspec(dllexport) void destroy_testdll(interface_testdll*);  struct _impl_interface_testdll; struct interface_testdll {
	ICEDB_DLL_BASE_HANDLE *_base; _impl_interface_testdll *_p;
	typedef int(*F_TYPE_testfunc)(interface_testdll *, int); F_TYPE_testfunc testfunc;
};


namespace _pimpl_interface_nm_testdll { struct _pimpl_interface_testdll; }; extern "C"  struct _impl_interface_testdll { _pimpl_interface_nm_testdll::_pimpl_interface_testdll* p; }; namespace _pimpl_interface_nm_testdll {
	struct _pimpl_interface_testdll {
	public: ~_pimpl_interface_testdll() {};
			struct Sym_testfunc {
				typedef int(*inner_type)(int);
				typedef int(*outer_type)(interface_testdll *, int);
				ICEDB_DLL_FUNCTION_STATUSES status;
				inner_type inner;
				static const char* Symbol() { return "testfunc"; }
			} sym_testfunc; 
			Sym_testfunc* Access_Sym_testfunc(interface_testdll *p) {
				auto a = p->_p->p;
				return &(a->sym_testfunc);
				//Sym_testfunc* r = &(p->_p->p.sym_testfunc);
				//return r; 
			};
			_pimpl_interface_testdll(interface_testdll* obj) {
				;
				sym_testfunc.status = ICEDB_DLL_FUNCTION_UNLOADED;
				sym_testfunc.inner = 0;
				obj->testfunc = ::icedb::dll::binding::DoBind<
					interface_testdll, Sym_testfunc, Access_Sym_testfunc, int, int>;
			}
	};
} extern "C" void destroy_testdll(interface_testdll* p) { ::icedb::dll::binding::destroy_interface<interface_testdll>(p); } extern "C" interface_testdll* create_testdll(ICEDB_DLL_BASE_HANDLE *base) { if (!base) ICEDB_DEBUG_RAISE_EXCEPTION_HANDLER("c:\\users\\ryan\\documents\\visual studio 2017\\projects\\germany_api\\testapp\\main.cpp", (int)17, __FUNCSIG__);; interface_testdll* p = (interface_testdll*)ICEDB_malloc(sizeof interface_testdll); memset(p, 0, sizeof(interface_testdll)); p->_base = base; p->_base->_vtable->incRefCount(p->_base); p->_p = (_impl_interface_testdll*)ICEDB_malloc(sizeof _impl_interface_testdll); p->_p->p = new _pimpl_interface_nm_testdll::_pimpl_interface_testdll(p); return p; };

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
	auto a = &(td->_p->p->sym_testfunc);
	auto b = td->_p->p->Access_Sym_testfunc(td);
	//std::cout << " a " << a << " b " << b << std::endl;

	int res = td->testfunc(td, 36);
	std::cout << "Res is " << res << std::endl;
	destroy_testdll(td);
	ICEDB_DLL_BASE_HANDLE_destroy(dll);
	return 0;
}
