#pragma once
#ifndef ICEDB_H_ERROR_CONTEXT
#define ICEDB_H_ERROR_CONTEXT
#include "defs.h"
#include "error.h"
ICEDB_BEGIN_DECL_C

	/** This private header lists the symbols for manipulating error contexts. **/

	struct ICEDB_error_context_var_val {
		char* varname;
		char* val;
	};
	/** Error contexts are simple things. **/
	struct ICEDB_error_context {
		ICEDB_error_code code;
		uint16_t message_size;
		uint16_t message_size_alloced;
		char* message_text;
		uint16_t num_var_fields;
		uint16_t max_num_var_fields;
		ICEDB_error_context_var_val* var_vals;
	};

	extern ICEDB_SYMBOL_PRIVATE ICEDB_THREAD_LOCAL ICEDB_error_context* __ICEDB_LOCAL_THREAD_error_context;

	/** Creates and stores an error context in the current thread. If another object is occupying the TLS,
	it is freed, so it is important to copy such objects using ICEDB_error_context_copy prior to read. **/
	ICEDB_SYMBOL_PRIVATE struct ICEDB_error_context* ICEDB_error_context_create_impl(int,const char*, int, const char*);
#define ICEDB_error_context_create(x) ICEDB_error_context_create_impl(x, __FILE__, (int)__LINE__,ICEDB_DEBUG_FSIG);

	/** Copies an error context. Used in reporting to application. **/
	ICEDB_SYMBOL_PRIVATE struct ICEDB_error_context* ICEDB_error_context_copy(const struct ICEDB_error_context*);

	/** Append extra information to the error context. **/
	ICEDB_SYMBOL_PRIVATE void ICEDB_error_context_append(struct ICEDB_error_context*, uint16_t sz, const char* data);
	ICEDB_SYMBOL_PRIVATE void ICEDB_error_context_append_str(struct ICEDB_error_context*, const char* data);

	/** Add a string to the error context. **/
	ICEDB_SYMBOL_PRIVATE void ICEDB_error_context_add_string(struct ICEDB_error_context*,
		uint16_t var_sz, const char* var_name, uint16_t val_sz, const char* var_val);
	ICEDB_SYMBOL_PRIVATE void ICEDB_error_context_add_string2(struct ICEDB_error_context*,
		const char* var_name, const char* var_val);

	/** Widen the error_context var_vals array (safely and non-destructively) **/
	ICEDB_SYMBOL_PRIVATE void ICEDB_error_context_widen(struct ICEDB_error_context*, uint16_t numNewSpaces);

ICEDB_END_DECL_C
#endif
