#pragma once
#ifndef ICEDB_HPP_ERRORINTERFACEIMPL
#define ICEDB_HPP_ERRORINTERFACEIMPL
#include "../defs.h"
#include "../dlls/dlls.hpp"
#include "error.h"
#include "../dlls/dllsImpl.hpp"
#include "errorInterface.hpp"
ICEDB_BEGIN_DECL

ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(error)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(error, error_code_to_message_size, uint16_t, ICEDB_error_code)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(error, error_code_to_message, uint16_t, ICEDB_error_code, size_t, char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(error, error_code_to_stream, uint16_t, ICEDB_error_code, FILE*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(error, get_error_context_thread_local, ICEDB_error_context*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(error, error_context_deallocate, void, ICEDB_error_context*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(error, error_code_to_code, ICEDB_error_code, const ICEDB_error_context*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(error, error_context_to_message_size, uint16_t, const ICEDB_error_context*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(error, error_context_to_message, uint16_t, const struct ICEDB_error_context*, size_t, char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(error, error_context_to_stream, uint16_t, const ICEDB_error_context*, FILE*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(error, error_test, ICEDB_error_code)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION_B(error, error_getOSname, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(error);
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(error, error_code_to_message_size, uint16_t, ICEDB_error_code)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(error, error_code_to_message, uint16_t, ICEDB_error_code, size_t, char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(error, error_code_to_stream, uint16_t, ICEDB_error_code, FILE*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(error, get_error_context_thread_local, ICEDB_error_context*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(error, error_context_deallocate, void, ICEDB_error_context*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(error, error_code_to_code, ICEDB_error_code, const ICEDB_error_context*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(error, error_context_to_message_size, uint16_t, const ICEDB_error_context*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(error, error_context_to_message, uint16_t, const struct ICEDB_error_context*, size_t, char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(error, error_context_to_stream, uint16_t, const ICEDB_error_context*, FILE*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(error, error_test, ICEDB_error_code)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(error, error_getOSname, const char*)
ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(error)

ICEDB_END_DECL
#endif
