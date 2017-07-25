#pragma once
#ifndef ICEDB_H_ERROR_CONTEXT
#define ICEDB_H_ERROR_CONTEXT
#include "../defs.h"
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
		size_t message_size;
		size_t message_size_alloced;
		char* message_text;
		size_t num_var_fields;
		size_t max_num_var_fields;
		ICEDB_error_context_var_val* var_vals;
	};

	extern ICEDB_THREAD_LOCAL ICEDB_error_context* __ICEDB_LOCAL_THREAD_error_context;

	/** Creates and stores an error context in the current thread. If another object is occupying the TLS,
	it is freed, so it is important to copy such objects using ICEDB_error_context_copy prior to read. **/
	DL_ICEDB struct ICEDB_error_context* ICEDB_error_context_create_impl(int,const char*, int, const char*);
#define ICEDB_error_context_create(x) ICEDB_error_context_create_impl(x, (__FILE__), (int)__LINE__,(ICEDB_DEBUG_FSIG));

	/** Copies an error context. Used in reporting to application. **/
	DL_ICEDB struct ICEDB_error_context* ICEDB_error_context_copy(const struct ICEDB_error_context*);

	/** Append extra information to the error context. **/
	DL_ICEDB void ICEDB_error_context_appendA(struct ICEDB_error_context*, size_t sz, const char* data);
	DL_ICEDB void ICEDB_error_context_append_strA(struct ICEDB_error_context*, const char* data);

	/** Add a string to the error context. **/
	DL_ICEDB void ICEDB_error_context_add_stringA(struct ICEDB_error_context*,
		size_t var_sz, const char* var_name, size_t val_sz, const char* var_val);
	DL_ICEDB void ICEDB_error_context_add_string2A(struct ICEDB_error_context*,
		const char* var_name, const char* var_val);

	/** Widen the error_context var_vals array (safely and non-destructively) **/
	DL_ICEDB void ICEDB_error_context_widen(struct ICEDB_error_context*, size_t numNewSpaces);


#define ICEDB_error_context_append ICEDB_error_context_appendA
#define ICEDB_error_context_append_str ICEDB_error_context_append_strA
#define ICEDB_error_context_add_string ICEDB_error_context_add_stringA
#define ICEDB_error_context_add_string2 ICEDB_error_context_add_string2A
/*#ifdef UNICODE
#define ICEDB_error_context_append ICEDB_error_context_appendW
#define ICEDB_error_context_append_str ICEDB_error_context_append_strW
#define ICEDB_error_context_add_string ICEDB_error_context_add_stringW
#define ICEDB_error_context_add_string2 ICEDB_error_context_add_string2W
#else
#define ICEDB_error_context_append ICEDB_error_context_appendA
#define ICEDB_error_context_append_str ICEDB_error_context_append_strA
#define ICEDB_error_context_add_string ICEDB_error_context_add_stringA
#define ICEDB_error_context_add_string2 ICEDB_error_context_add_string2A
#endif*/

ICEDB_END_DECL_C
#endif
