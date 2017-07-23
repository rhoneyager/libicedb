#pragma once
#ifndef ICEDB_HPP_ERRORINTERFACE
#define ICEDB_HPP_ERRORINTERFACE
#include "../defs.h"
#include "../dlls/dlls.hpp"
#include "error.h"
ICEDB_BEGIN_DECL

ICEDB_DLL_INTERFACE_BEGIN(ICEDB_core_error)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_error, error_code_to_message_size, uint16_t, ICEDB_error_code)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_error, error_code_to_message, uint16_t, ICEDB_error_code, size_t, char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_error, error_code_to_stream, uint16_t, ICEDB_error_code, FILE*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_error, get_error_context_thread_local, ICEDB_error_context*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_error, error_context_deallocate, void, ICEDB_error_context*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_error, error_code_to_code, ICEDB_error_code, const ICEDB_error_context*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_error, error_context_to_message_size, uint16_t, const ICEDB_error_context*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_error, error_context_to_message, uint16_t, const struct ICEDB_error_context*, size_t, char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_error, error_context_to_stream, uint16_t, const ICEDB_error_context*, FILE*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_error, error_test, ICEDB_error_code)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_error, error_getOSname, const char*)
ICEDB_DLL_INTERFACE_END

ICEDB_END_DECL
#endif
