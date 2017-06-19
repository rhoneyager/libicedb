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
			// Cannot be a member inside the struct. May decide to place in a namespace.
			template<class InterfaceType, class SymbolName, class ReturnType, class ...Args>
			ReturnType DoBind(InterfaceType *iface, Args... args) {
				if (iface->status_m_testNum != ICEDB_DLL_FUNCTION_LOADED) {
					iface->m_testNum = (interface_testdll::TYPE_testNum)
						iface->_base->_vtable->getSym(iface->_base, SymbolName::Symbol());
					iface->status_m_testNum = ICEDB_DLL_FUNCTION_LOADED;
				}
				bool iv = (typeid(ReturnType) == typeid(void));

				if (iv) {
					iface->m_testNum(args...);
					return static_cast<ReturnType>(NULL);
				}
				return (ReturnType)iface->m_testNum(args...);
			}

			// This should only be in the implementation file, which is in C++.
			template <class InterfaceClass>
			void destroy_interface(InterfaceClass* p) {
				delete p->_p->p;
				ICEDB_free(p->_p);
				p->_base->_vtable->decRefCount(p->_base);
				ICEDB_free(p);
			}

		}
	}
}

#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(InterfaceName) \
	namespace _pimpl_interface_nm_##InterfaceName{ \
		ICEDB_CALL_CPP PRIVATE_ICEDB struct _pimpl_interface_##InterfaceName { \
		public: \
			~_pimpl_interface_##InterfaceName() {} \
			_pimpl_interface_##InterfaceName(interface_##InterfaceName* obj) {


#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(InterfaceName, retVal, FuncName, ...) \
				obj->status_m_##FuncName = ICEDB_DLL_FUNCTION_UNLOADED; \
				obj->m_##FuncName = NULL; \
				struct tfname_##FuncName { static const char* Symbol() { return #FuncName ; } }; \
				obj->##FuncName = ::icedb::dll::binding::DoBind<interface_##InterfaceName, tfname_##FuncName, retVal, __VA_ARGS__>;


#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(InterfaceName) \
			} \
		}; \
	} \
	ICEDB_CALL_C DL_ICEDB void destroy_##InterfaceName(interface_##InterfaceName* p) \
		{ ::icedb::dll::binding::destroy_interface<interface_##InterfaceName>(p); } \
	ICEDB_CALL_C HIDDEN_ICEDB struct _impl_interface_##InterfaceName { \
			_pimpl_interface_nm_##InterfaceName::_pimpl_interface_##InterfaceName* p; \
		}; \
	ICEDB_CALL_C DL_ICEDB interface_testdll* create_testdll(ICEDB_DLL_BASE_HANDLE *base) { \
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
