#pragma once
#ifndef ICEDB_H_ERROR_CONTEXT
#define ICEDB_H_ERROR_CONTEXT
#include "../defs.h"
#include "error.h"
ICEDB_BEGIN_DECL_C

	/** This private header lists the symbols for manipulating error contexts. **/

	struct ICEDB_error_context_var_val {
		wchar_t* varname;
		wchar_t* val;
	};
	/** Error contexts are simple things. **/
	struct ICEDB_error_context {
		ICEDB_error_code code;
		size_t message_size;
		size_t message_size_alloced;
		wchar_t* message_text;
		size_t num_var_fields;
		size_t max_num_var_fields;
		ICEDB_error_context_var_val* var_vals;
	};

	extern ICEDB_THREAD_LOCAL ICEDB_error_context* __ICEDB_LOCAL_THREAD_error_context;

	/** Creates and stores an error context in the current thread. If another object is occupying the TLS,
	it is freed, so it is important to copy such objects using ICEDB_error_context_copy prior to read. **/
	DL_ICEDB struct ICEDB_error_context* ICEDB_error_context_create_impl(int,const wchar_t*, int, const wchar_t*);
#define ICEDB_error_context_create(x) ICEDB_error_context_create_impl(x, ICEDB_WIDEN(__FILE__), (int)__LINE__,ICEDB_WIDEN(ICEDB_DEBUG_FSIG));

	/** Copies an error context. Used in reporting to application. **/
	DL_ICEDB struct ICEDB_error_context* ICEDB_error_context_copy(const struct ICEDB_error_context*);

	/** Append extra information to the error context. **/
	DL_ICEDB void ICEDB_error_context_append(struct ICEDB_error_context*, size_t sz, const wchar_t* data);
	DL_ICEDB void ICEDB_error_context_append_str(struct ICEDB_error_context*, const wchar_t* data);

	/** Add a string to the error context. **/
	DL_ICEDB void ICEDB_error_context_add_string(struct ICEDB_error_context*,
		size_t var_sz, const wchar_t* var_name, size_t val_sz, const wchar_t* var_val);
	DL_ICEDB void ICEDB_error_context_add_string2(struct ICEDB_error_context*,
		const wchar_t* var_name, const wchar_t* var_val);

	/** Widen the error_context var_vals array (safely and non-destructively) **/
	DL_ICEDB void ICEDB_error_context_widen(struct ICEDB_error_context*, size_t numNewSpaces);

ICEDB_END_DECL_C
#endif
