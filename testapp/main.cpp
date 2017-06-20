#include <iostream>
#include <functional>
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

ICEDB_DLL_CPP_INTERFACE_BEGIN(testdllcpp, testdll)
ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(testdllcpp, int, setint, int)
ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(testdllcpp, int, getint)
ICEDB_DLL_CPP_INTERFACE_END
/*

class testdllcpp {
	::icedb::dll::Dll_Base_Handle::pointer_type _base; 
	testdllcpp();
	std::unique_ptr<interface_testdll, decltype(&destroy_testdll)> _p;
public: 
	::icedb::dll::Dll_Base_Handle::pointer_type getDll();
	typedef std::shared_ptr<testdllcpp> pointer_type;
	static pointer_type generate(::icedb::dll::Dll_Base_Handle::pointer_type);
	virtual ~testdllcpp();
	int getint();
	void setint(int);
};
*/

template <class CInterfaceType, class SymbolClass, class SymbolAccessor,
	class ReturnType, class ...Args>
std::function<ReturnType(Args...)> BindCPP(std::weak_ptr<CInterfaceType> wp) {
	auto res = [wp](Args... args) {
		std::shared_ptr<CInterfaceType> p = wp.lock();
		SymbolClass *s = SymbolAccessor::Access(p.get());
		if ((!s->status) || (s->status != p->_base->_vtable->isOpen(p->_base))) {
			s->inner = (SymbolClass::inner_type) p->_base->_vtable->getSym(p->_base, SymbolClass::Symbol());
			if (!s->inner) ICEDB_DEBUG_RAISE_EXCEPTION();
			s->status = p->_base->openCount;
		}
		bool iv = (typeid(ReturnType) == typeid(void));

		if (iv) {
			s->inner(args...);
			return static_cast<ReturnType>(NULL);
		}
		return (ReturnType)s->inner(args...);
	};
	return res;
}

testdllcpp::testdllcpp() : _p(nullptr, destroy_testdll) {} 
testdllcpp::~testdllcpp() {} 
::icedb::dll::Dll_Base_Handle::pointer_type testdllcpp::getDll() { return _base; } 
testdllcpp::pointer_type testdllcpp::generate(::icedb::dll::Dll_Base_Handle::pointer_type bp) { 
	testdllcpp::pointer_type p(new testdllcpp); 
	p->_base = bp; 
	std::shared_ptr<interface_testdll> np(
		create_testdll(bp->getBase()), destroy_testdll); 
	p->_p.swap(np);
	
	p->getint = BindCPP<interface_testdll,
		_pimpl_interface_nm_testdll::_pimpl_interface_testdll::Sym_getint,
		_pimpl_interface_nm_testdll::_pimpl_interface_testdll::Access_Sym_getint,
		int>(p->_p);
	p->setint = BindCPP<interface_testdll,
		_pimpl_interface_nm_testdll::_pimpl_interface_testdll::Sym_setint,
		_pimpl_interface_nm_testdll::_pimpl_interface_testdll::Access_Sym_setint,
		int, int>(p->_p);
	
	return p;
}
//*/
//ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_BEGIN(testdllcpp, testdll)


ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_END(testdllcpp, testdll)


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
#define ERR_VALIDATE(x) if(x) { \
		auto cxt = icedb::error::get_error_context_thread_local(); \
		if (cxt) printf("%s", icedb::error::stringify(cxt).c_str()); \
		else ICEDB_DEBUG_RAISE_EXCEPTION(); \
	}
	ERR_VALIDATE(td1a->_base->_vtable->open(td1a->_base));
	auto td1b = create_testdll(dllInst1);
	ERR_VALIDATE(td1b->_base->_vtable->open(td1b->_base));

	auto dllInst2 = ::icedb::dll::Dll_Base_Handle::generate("testdll.dll");
	auto td2a = testdllcpp::generate(dllInst2);
	ERR_VALIDATE(td2a->getDll()->open());
	auto td2b = testdllcpp::generate(dllInst2);
	ERR_VALIDATE(td2b->getDll()->open());
	
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
