#pragma once
#ifndef ICEDB_HPP_DLLS_IMPL
#define ICEDB_HPP_DLLS_IMPL
#include "defs.h"
#include "error.h"
#include "dllsImpl.h"
#include <stdarg.h>

ICEDB_BEGIN_DECL

namespace icedb {
	namespace dll {
		namespace binding {
			// This should only be in the implementation file, which is in C++.
			template <class InterfaceType>
			void destroy_interface(InterfaceType* p) {
				delete p->_p->p;
				ICEDB_free(p->_p);
				p->_base->_vtable->decRefCount(p->_base);
				ICEDB_free(p);
			}

			template<class InterfaceType, class SymbolClass, class SymbolAccessor, class ReturnType, class ...Args>
			ReturnType DoBind(InterfaceType *p, Args... args) {
				SymbolClass *s = SymbolAccessor::Access(p);
				if ((!s->status) || (s->status != p->_base->isOpen(p->_base))) {
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
			}
		}
	}
}

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(InterfaceName) \
	namespace _pimpl_interface_nm_##InterfaceName{ \
		struct _pimpl_interface_##InterfaceName; }; \
	ICEDB_CALL_C HIDDEN_ICEDB struct _impl_interface_##InterfaceName { \
			_pimpl_interface_nm_##InterfaceName::_pimpl_interface_##InterfaceName* p; \
		}; \
	namespace _pimpl_interface_nm_##InterfaceName{ \
		ICEDB_CALL_CPP PRIVATE_ICEDB struct _pimpl_interface_##InterfaceName { \
		public: \
			~_pimpl_interface_##InterfaceName() {}


#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(InterfaceName, retVal, FuncName, FuncSymbolName, ...) \
			struct Sym_##FuncName { \
				typedef retVal (* inner_type)(__VA_ARGS__); \
				typedef retVal (* outer_type)(interface_##InterfaceName *, __VA_ARGS__); \
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
			}

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(InterfaceName) \
			_pimpl_interface_##InterfaceName(interface_##InterfaceName* obj) {

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(InterfaceName, retVal, FuncName, ...) \
				sym_##FuncName.status = 0; \
				sym_##FuncName.inner = NULL; \
				obj->##FuncName = ::icedb::dll::binding::DoBind \
					<interface_##InterfaceName, Sym_##FuncName, \
					Access_Sym_##FuncName, retVal, __VA_ARGS__>;

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(InterfaceName) \
			} \
		}; \
	} \
	ICEDB_CALL_C void destroy_##InterfaceName(interface_##InterfaceName* p) \
		{ ::icedb::dll::binding::destroy_interface<interface_##InterfaceName>(p); } \
	ICEDB_CALL_C interface_##InterfaceName* create_##InterfaceName(ICEDB_DLL_BASE_HANDLE *base) { \
		if (!base) ICEDB_DEBUG_RAISE_EXCEPTION(); \
		interface_##InterfaceName* p = (interface_##InterfaceName*)ICEDB_malloc(sizeof interface_##InterfaceName ); \
		memset(p, NULL, sizeof(interface_##InterfaceName)); \
		p->_base = base; \
		p->_base->_vtable->incRefCount(p->_base); \
		p->_p = (_impl_interface_##InterfaceName*)ICEDB_malloc(sizeof _impl_interface_##InterfaceName); \
		p->_p->p = new _pimpl_interface_nm_##InterfaceName::_pimpl_interface_##InterfaceName(p); \
		return p; \
	}



ICEDB_END_DECL

#endif
