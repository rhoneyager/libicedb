#pragma once
#pragma once
#ifndef ICEDB_HPP_ERRORCONTEXTINTERFACE
#define ICEDB_HPP_ERRORCONTEXTINTERFACE
#include "../defs.h"
#include "../dlls/dlls.hpp"
#include "error.h"
ICEDB_BEGIN_DECL

ICEDB_DLL_INTERFACE_BEGIN(error_context)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(error_context, error_context_create_impl, ICEDB_error_context*, int, const char*, int, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(error_context, error_context_copy, ICEDB_error_context*, const struct ICEDB_error_context*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(error_context, error_context_append, void, struct ICEDB_error_context*, uint16_t, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(error_context, error_context_append_str, void, struct ICEDB_error_context*, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(error_context, error_context_add_string, void, struct ICEDB_error_context*, uint16_t, const char*, uint16_t, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(error_context, error_context_add_string2, void, struct ICEDB_error_context*, const char*, const char*)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(error_context, error_context_widen, void, struct ICEDB_error_context*, uint16_t)
ICEDB_DLL_INTERFACE_END

ICEDB_END_DECL
#endif
