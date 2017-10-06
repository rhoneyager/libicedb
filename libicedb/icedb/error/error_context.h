#pragma once
#ifndef ICEDB_H_ERROR_CONTEXT
#define ICEDB_H_ERROR_CONTEXT
#include "../defs.h"
#include "error.h"
ICEDB_BEGIN_DECL_C

/** @defgroup errs Errors
*
* @{
**/
	/** This private header lists the symbols for manipulating error contexts. **/

	struct ICEDB_error_context_var_val {
		char* varname;
		char* val;
	};
	/** Error contexts are simple things. Use functions to manipulate, and do not allocate / free parts of the structure directly. **/
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
	typedef struct ICEDB_error_context* (*ICEDB_error_context_create_impl_f)(int,const char*, int, const char*);
	extern DL_ICEDB ICEDB_error_context_create_impl_f ICEDB_error_context_create_impl;
#define ICEDB_error_context_create(x) ICEDB_error_context_create_impl(x, (__FILE__), (int)__LINE__,(ICEDB_DEBUG_FSIG));

	/** Copies an error context. Used in reporting to application. **/
	typedef struct ICEDB_error_context* (*ICEDB_error_context_copy_f)(const struct ICEDB_error_context*);
	extern DL_ICEDB ICEDB_error_context_copy_f ICEDB_error_context_copy;

	/** Append extra information to the error context. **/
	typedef void (*ICEDB_error_context_appendA_f)(struct ICEDB_error_context*, size_t sz, const char* data);
	typedef void (*ICEDB_error_context_append_strA_f)(struct ICEDB_error_context*, const char* data);
	extern DL_ICEDB ICEDB_error_context_appendA_f ICEDB_error_context_appendA;
	extern DL_ICEDB ICEDB_error_context_append_strA_f ICEDB_error_context_append_strA;

	/** Add a string to the error context. **/
	typedef void (*ICEDB_error_context_add_stringA_f)(struct ICEDB_error_context*,
		size_t var_sz, const char* var_name, size_t val_sz, const char* var_val);
	extern DL_ICEDB ICEDB_error_context_add_stringA_f ICEDB_error_context_add_stringA;
	typedef void (*ICEDB_error_context_add_string2A_f)(struct ICEDB_error_context*,
		const char* var_name, const char* var_val);
	extern DL_ICEDB ICEDB_error_context_add_string2A_f ICEDB_error_context_add_string2A;

	/** Widen the error_context var_vals array (safely and non-destructively) **/
	typedef void (*ICEDB_error_context_widen_f)(struct ICEDB_error_context*, size_t numNewSpaces);
	extern DL_ICEDB ICEDB_error_context_widen_f ICEDB_error_context_widen;


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

/// A convenience wrapper for all error functions
	struct ICEDB_error_context_container_ftable {
		ICEDB_error_context_create_impl_f createImpl;
		ICEDB_error_context_copy_f copy;
		ICEDB_error_context_appendA_f appendA;
		ICEDB_error_context_append_strA_f appendStrA;
		ICEDB_error_context_add_stringA_f addStringA;
		ICEDB_error_context_add_string2A_f addString2A;
		ICEDB_error_context_widen_f widen;
	};
	DL_ICEDB extern const struct ICEDB_error_context_container_ftable ICEDB_ct_error_context;
	

/** @} */ // end of errs
ICEDB_END_DECL_C
#endif
