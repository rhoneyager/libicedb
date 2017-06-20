#pragma once
#ifndef ICEDB_HPP_DLLS
#define ICEDB_HPP_DLLS
#include <memory>
#include "defs.h"
#include "dlls.h"

ICEDB_BEGIN_DECL_CPP

struct ICEDB_DLL_BASE_HANDLE;

#if defined _MSC_FULL_VER
#pragma warning(push)
#pragma warning( disable : 4251 )
#endif
namespace icedb {
	namespace dll {
		class DL_ICEDB Dll_Base_Handle {
			typedef std::unique_ptr<ICEDB_DLL_BASE_HANDLE,
				decltype(&ICEDB_DLL_BASE_HANDLE_destroy)> base_pointer_type;
			base_pointer_type _base;
			Dll_Base_Handle(base_pointer_type&);
		public:
			virtual ~Dll_Base_Handle();
			typedef std::shared_ptr<Dll_Base_Handle> pointer_type;
			ICEDB_DLL_BASE_HANDLE* getBase();
			ICEDB_error_code open();
			ICEDB_error_code close();
			uint16_t isOpen() const;
			uint16_t getRefCount() const;
			void incRefCount();
			ICEDB_error_code decRefCount();
			void* getSym(const char* symbol_name);
			const char* getPath() const;
			ICEDB_error_code setPath(const char* filename);
			void setAutoOpen(bool);
			bool getAutoOpen() const;
			static pointer_type generate(const char* filename);
			static pointer_type generate(ICEDB_DLL_BASE_HANDLE*); // Transfers control to C++ class.
		};
		typedef Dll_Base_Handle::pointer_type Dll_Base_Handle_pt;
	}
}


#define ICEDB_DLL_CPP_INTERFACE_BEGIN(InterfaceName, CInterfaceName) \
	class InterfaceName { \
		::icedb::dll::Dll_Base_Handle::pointer_type _base; \
		InterfaceName(); \
		std::shared_ptr<interface_##CInterfaceName> _p; \
	public: \
		::icedb::dll::Dll_Base_Handle::pointer_type getDll(); \
		typedef std::shared_ptr<InterfaceName> pointer_type; \
		static pointer_type generate(::icedb::dll::Dll_Base_Handle::pointer_type); \
		virtual ~InterfaceName();
#define ICEDB_DLL_CPP_INTERFACE_DECLARE_FUNCTION(InterfaceName, FuncName, retVal, ...) \
	std::function<retVal(__VA_ARGS__)> FuncName;
	//typedef retVal (* type_##FuncName)(__VA_ARGS__); \
	//type_##FuncName FuncName;
//	retVal FuncName(__VA_ARGS__);
#define ICEDB_DLL_CPP_INTERFACE_END \
	};

#if defined _MSC_FULL_VER
#pragma warning(pop)
#endif
ICEDB_END_DECL_CPP
#endif
