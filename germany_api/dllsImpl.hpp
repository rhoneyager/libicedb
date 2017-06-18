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
	ICEDB_CALL_C DL_ICEDB void destroy_##InterfaceName(interface_##InterfaceName* p) { ::icedb::dll::binding::destroy_interface<interface_##InterfaceName>(p); }
#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(InterfaceName)
#define ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(InterfaceName, retVal, FuncName, ...)

ICEDB_END_DECL
#endif
