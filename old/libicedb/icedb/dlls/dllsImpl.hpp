#pragma once
#ifndef ICEDB_HPP_DLLS_IMPL
#define ICEDB_HPP_DLLS_IMPL
#include "../defs.h"
#include "../error/error.h"
#include "dllsImpl.h"
#include <stdarg.h>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>

ICEDB_BEGIN_DECL

namespace icedb {
	namespace dll {
		namespace binding {
			// This should only be in the implementation file, which is in C++.
			template <class InterfaceType>
			void destroy_interface(InterfaceType* p) {
				if (!p) return; // Shared_ptr with deleters, on accident
				delete p->_p->p;
				free(p->_p);
				p->_base->_vtable->decRefCount(p->_base);
				free(p);
			}

			template<class InterfaceType, class SymbolClass, class SymbolAccessor>
			bool MakeBind(InterfaceType *p) {
				SymbolClass *s = SymbolAccessor::Access(p);
				if ((!s->status) || (s->status != p->_base->_vtable->isOpen(p->_base))) {
					s->inner = (typename SymbolClass::inner_type) p->_base->_vtable->getSym(p->_base, SymbolClass::Symbol());
					if (!s->inner) return false;
					s->status = p->_base->openCount;
				}
				return true;
			}

			template<class InterfaceType, class SymbolClass, class SymbolAccessor, class ReturnType, class ...Args>
			ReturnType DoBind(InterfaceType *p, Args... args) {
				SymbolClass *s = SymbolAccessor::Access(p);
				if ((!s->status) || (s->status != p->_base->_vtable->isOpen(p->_base))) {
					const char* symName = SymbolClass::Symbol();
					s->inner = (typename SymbolClass::inner_type) p->_base->_vtable->getSym(p->_base, symName);
					if (!s->inner) p->_base->_vtable->_raiseExcept(p->_base,
						__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
					s->status = p->_base->openCount;
				}
				return static_cast<ReturnType>(s->inner(args...));
			}

			template <class CInterfaceType, class SymbolClass, class SymbolAccessor>
				std::function<bool()> CanBindCPP(std::weak_ptr<CInterfaceType> wp) {
				auto res = [wp]() {
					std::shared_ptr<CInterfaceType> p = wp.lock();
					SymbolClass *s = SymbolAccessor::Access(p.get());
					if ((!s->status) || (s->status != p->_base->_vtable->isOpen(p->_base))) {
						std::string sym = SymbolClass::Symbol();
						s->inner = (typename SymbolClass::inner_type) p->_base->_vtable->getSym(p->_base, sym.c_str());
						if (!s->inner) return false;
						s->status = p->_base->openCount;
					}
					return true;
				};
				return res;
			}

			template <class CInterfaceType, class SymbolClass, class SymbolAccessor,
				class ReturnType, class ...Args>
				std::function<ReturnType(Args...)> BindCPP(std::weak_ptr<CInterfaceType> wp) {
				auto res = [wp](Args... args) {
					std::shared_ptr<CInterfaceType> p = wp.lock();
					SymbolClass *s = SymbolAccessor::Access(p.get());
					if ((!s->status) || (s->status != p->_base->_vtable->isOpen(p->_base))) {
						std::string sym = SymbolClass::Symbol();
						s->inner = (typename SymbolClass::inner_type) p->_base->_vtable->getSym(p->_base, sym.c_str());
						if (!s->inner) p->_base->_vtable->_raiseExcept(p->_base,
							__FILE__, (int)__LINE__, ICEDB_DEBUG_FSIG);
						s->status = p->_base->openCount;
					}
					return (ReturnType)s->inner(args...);
				};
				return res;
			}
		}
	}
}

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(InterfaceName) \
	namespace _pimpl_interface_nm_##InterfaceName{ \
		struct _pimpl_interface_##InterfaceName; } \
	ICEDB_CALL_C struct HIDDEN_ICEDB _impl_interface_##InterfaceName { \
			_pimpl_interface_nm_##InterfaceName::_pimpl_interface_##InterfaceName* p; \
		}; \
	namespace _pimpl_interface_nm_##InterfaceName{ \
		ICEDB_CALL_CPP struct PRIVATE_ICEDB _pimpl_interface_##InterfaceName { \
		public: \
			~_pimpl_interface_##InterfaceName() {}


#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(InterfaceName, FuncName, FuncSymbolName, retVal, ...) \
			struct Sym_##FuncName { \
				typedef retVal (* inner_type)(__VA_ARGS__); \
				typedef retVal (* outer_type)(interface_##InterfaceName *, ##__VA_ARGS__); \
				ICEDB_DLL_FUNCTION_STATUSES status; \
				inner_type inner; \
				static const char* Symbol() { return FuncSymbolName ; } \
			} sym_##FuncName; \
			struct Access_Sym_##FuncName { \
				static Sym_##FuncName* Access(interface_##InterfaceName *p) \
				{ \
					_pimpl_interface_##InterfaceName*a = p->_p->p; \
					Sym_##FuncName* r = &(a->sym_##FuncName); \
					return r; \
				} \
			};

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(InterfaceName, FuncName, retVal, ...) \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(InterfaceName, FuncName, #FuncName, retVal, ##__VA_ARGS__)

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(InterfaceName) \
			_pimpl_interface_##InterfaceName(interface_##InterfaceName* obj) {

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONCAT(a,b) obj->b

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(InterfaceName, FuncName, retVal, ...) \
				sym_##FuncName.status = 0; \
				sym_##FuncName.inner = NULL; \
				ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONCAT((*obj),FuncName) = ::icedb::dll::binding::DoBind \
					<interface_##InterfaceName, Sym_##FuncName, \
					Access_Sym_##FuncName, retVal, ##__VA_ARGS__>; \
				obj->Bind_##FuncName = ::icedb::dll::binding::MakeBind \
					<interface_##InterfaceName, Sym_##FuncName, \
					Access_Sym_##FuncName>;

//				obj->##FuncName = ::icedb::dll::binding::DoBind 

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(InterfaceName) \
			} \
		}; \
	} \
	ICEDB_CALL_C void destroy_##InterfaceName(interface_##InterfaceName* p) \
		{ ::icedb::dll::binding::destroy_interface<interface_##InterfaceName>(p); } \
	ICEDB_CALL_C interface_##InterfaceName* create_##InterfaceName(ICEDB_DLL_BASE_HANDLE *base) { \
		if (!base) return nullptr; \
		interface_##InterfaceName* p = (interface_##InterfaceName*)malloc(sizeof(interface_##InterfaceName ) ); \
		memset(p, 0, sizeof(interface_##InterfaceName)); \
		p->_base = base; \
		p->_base->_vtable->incRefCount(p->_base); \
		p->_p = (_impl_interface_##InterfaceName*)malloc(sizeof(_impl_interface_##InterfaceName)); \
		p->_p->p = new _pimpl_interface_nm_##InterfaceName::_pimpl_interface_##InterfaceName(p); \
		return p; \
	}






#define ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_BEGIN(InterfaceName, CInterfaceName) \
	InterfaceName::InterfaceName() : _p(nullptr){} \
	InterfaceName::~InterfaceName() {} \
	::icedb::dll::Dll_Base_Handle::pointer_type InterfaceName::getDll() { return _base; } \
	InterfaceName::pointer_type InterfaceName::generate(::icedb::dll::Dll_Base_Handle::pointer_type bp) { \
		InterfaceName::pointer_type p(new InterfaceName); \
		p->_base = bp; \
		std::shared_ptr<interface_##CInterfaceName> \
			np(create_##CInterfaceName(bp->getBase()), destroy_##CInterfaceName); \
		p->_p.swap(np);

#define ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_FUNCTION(CInterfaceName, FuncName, ...) \
	p->FuncName = ::icedb::dll::binding::BindCPP<interface_##CInterfaceName, \
	_pimpl_interface_nm_##CInterfaceName::_pimpl_interface_##CInterfaceName::Sym_##FuncName, \
	_pimpl_interface_nm_##CInterfaceName::_pimpl_interface_##CInterfaceName::Access_Sym_##FuncName, \
		__VA_ARGS__>(p->_p); \
	p->Bind_##FuncName = ::icedb::dll::binding::CanBindCPP \
		<interface_##CInterfaceName, \
		_pimpl_interface_nm_##CInterfaceName::_pimpl_interface_##CInterfaceName::Sym_##FuncName, \
		_pimpl_interface_nm_##CInterfaceName::_pimpl_interface_##CInterfaceName::Access_Sym_##FuncName \
		>(p->_p);

#define ICEDB_DLL_CPP_INTERFACE_IMPLEMENTATION_END \
		return p; \
	}





ICEDB_END_DECL

#endif
