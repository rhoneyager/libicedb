#pragma once
#ifndef ICEDB_HPP_DLLS
#define ICEDB_HPP_DLLS
#include <memory>
#include "defs.h"
#include "dlls.h"

ICEDB_BEGIN_DECL_CPP

struct ICEDB_DLL_BASE_HANDLE;
namespace icedb {
	namespace dll {
		class DL_ICEDB Dll_Base_Handle {
			const char* path;
			uint16_t refCount;
			std::unique_ptr<_dlHandleType_impl> _dlHandle;
		public:
			typedef std::shared_ptr<Dll_Base_Handle> pointer_type;
			ICEDB_error_code open();
			ICEDB_error_code close();
			bool isOpen() const;
			uint16_t getRefCount() const;
			void incRefCount();
			ICEDB_error_code decRefCount();
			void* getSym(const char* symbol_name);
			const char* getPath() const;
			ICEDB_error_code setPath(const char* filename);
			static pointer_type generate(const char* filename);
		};
		typedef Dll_Base_Handle::pointer_type Dll_Base_Handle_pt;
	}
}


#define ICEDB_DLL_CPP_INTERFACE_BEGIN(InterfaceName) \
    HIDDEN_ICEDB struct _impl_interface_##InterfaceName; \
	class InterfaceName { \
		::icedb::dll::Dll_Base_Handle::pointer_type _base; \
		InterfaceName(); \
		std::unique_ptr<_impl_interface_##InterfaceName> _p; \
	public: \
		::icedb::dll::Dll_Base_Handle::pointer_type getDll(); \
		typedef std::shared_ptr<InterfaceName> pointer_type; \
		static pointer_type generate(::icedb::dll::Dll_Base_Handle::pointer_type); \
		virtual ~InterfaceName();
#define ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(InterfaceName, retVal, FuncName, ...) \
	typedef retVal (* type_##FuncName)(__VA_ARGS__); \
	type_##FuncName FuncName;
#define ICEDB_DLL_CPP_INTERFACE_END \
	};

ICEDB_END_DECL_CPP
#endif
