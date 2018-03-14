#define __STDC_WANT_LIB_EXT1__ 1
#include "../icedb/util.h"
#include <string.h>
#include <stdlib.h>
#include "../icedb/error_context.h"
#include "../icedb/util.h"

ICEDB_SYMBOL_PRIVATE ICEDB_THREAD_LOCAL ICEDB_error_context* __ICEDB_LOCAL_THREAD_error_context = NULL;

struct ICEDB_error_context* error_context_create_impl(int code, const char* file, int line, const char* fsig)
{
	ICEDB_error_context* res = (ICEDB_error_context*) ICEDB_malloc(sizeof (ICEDB_error_context));
	if (!res) ICEDB_DEBUG_RAISE_EXCEPTION();
	res->code = code;
	res->message_size_alloced = (size_t) (sizeof(char)*(1 + ICEDB_COMPAT_strnlen_s(ICEDB_ERRORCODES_MAP[code],UINT16_MAX-1)));
	res->message_text = (char*)ICEDB_malloc(sizeof(char)*res->message_size_alloced);
	ICEDB_COMPAT_strncpy_s(res->message_text, res->message_size_alloced, 
		ICEDB_ERRORCODES_MAP[code], (size_t)sizeof(char)*(1+ ICEDB_COMPAT_strnlen_s(ICEDB_ERRORCODES_MAP[code], UINT16_MAX - 1)));
	// Note that the null character is counted here, by my convention. message_size should NEVER equal zero.
	res->message_size = (size_t) (sizeof(char)*(1 + ICEDB_COMPAT_strnlen_s(res->message_text, UINT16_MAX - 1)));
	res->num_var_fields = 0;
	res->max_num_var_fields = 0;
	res->var_vals = NULL;
	ICEDB_error_context_widen(res, 30);
	ICEDB_error_context_add_string2(res, "file", file);
	ICEDB_error_context_add_string2(res, "fsig", fsig);

	const int slinesz = 50;
	char sline[slinesz];
	snprintf(sline, slinesz, "%d", line);

	ICEDB_error_context_add_string2(res, "line", sline);

	if (__ICEDB_LOCAL_THREAD_error_context) ICEDB_free(__ICEDB_LOCAL_THREAD_error_context);
	__ICEDB_LOCAL_THREAD_error_context = res;
	return res;
}
DL_ICEDB ICEDB_error_context_create_impl_f ICEDB_error_context_create_impl = error_context_create_impl;

struct ICEDB_error_context* error_context_copy(const struct ICEDB_error_context *c)
{
	if (!c) return NULL;
	ICEDB_error_context* res = ICEDB_error_context_create(c->code);
	res->code = c->code;
	res->message_size = c->message_size;
	res->message_size_alloced = c->message_size_alloced;
	res->message_text = (char*)ICEDB_malloc(c->message_size_alloced);
	ICEDB_COMPAT_strncpy_s(res->message_text, c->message_size_alloced, c->message_text, c->message_size_alloced);

	for (int i = 0; i < c->num_var_fields; ++i)
		ICEDB_error_context_add_string2(res, c->var_vals[i].varname, c->var_vals[i].val);

	return res;
}
DL_ICEDB ICEDB_error_context_copy_f ICEDB_error_context_copy = error_context_copy;

void error_context_append(struct ICEDB_error_context *c, size_t sz, const char * data)
{
	if (data[sz] != '\0' && data[sz + 1] == '\0') sz++; // Null character check
	const size_t min_alloc_size_inc = 256;
	if (c->message_size + sz >= c->message_size_alloced) {
		size_t newszinc = (sz> min_alloc_size_inc) ? sz : min_alloc_size_inc;
		size_t newsz = c->message_size_alloced + newszinc;
		char* newtext = (char*)ICEDB_malloc(newsz);
		if (c->message_text) {
			ICEDB_COMPAT_strncpy_s(newtext, newsz, c->message_text, c->message_size_alloced);
			ICEDB_free(c->message_text);
		}
		c->message_text = newtext;
		c->message_size_alloced = newsz;
	}
	ICEDB_COMPAT_strncpy_s(c->message_text+ c->message_size-1, //c->message_size always includes the null character
		c->message_size_alloced-c->message_size,
		data, sz);
	c->message_size = (size_t) ICEDB_COMPAT_strnlen_s(c->message_text, 
		(c->message_size_alloced < UINT16_MAX-1) ? c->message_size_alloced : UINT16_MAX-1)+1; // -1,+1 because message_size includes the null character.
}
DL_ICEDB ICEDB_error_context_appendA_f ICEDB_error_context_appendA = error_context_append;

void error_context_append_str(struct ICEDB_error_context *c, const char * data)
{
	size_t sz = (size_t) strnlen(data, UINT16_MAX);
	ICEDB_error_context_append(c, sz+1, data);
}
DL_ICEDB ICEDB_error_context_append_strA_f ICEDB_error_context_append_strA = error_context_append_str;

void error_context_add_string(struct ICEDB_error_context *c, size_t var_sz, const char * var_name, size_t val_sz, const char * var_val)
{
	if (c->num_var_fields == c->max_num_var_fields)
		ICEDB_error_context_widen(c, 30);
	c->var_vals[c->num_var_fields].varname = ICEDB_COMPAT_strdup_s(var_name, var_sz);
	c->var_vals[c->num_var_fields].val = ICEDB_COMPAT_strdup_s(var_val, val_sz);
	c->num_var_fields++;
}
DL_ICEDB ICEDB_error_context_add_stringA_f ICEDB_error_context_add_stringA = error_context_add_string;

void error_context_add_string2(struct ICEDB_error_context *c, const char * var_name, const char * var_val)
{
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
	ICEDB_error_context_add_string(c, (size_t)strnlen_s(var_name, UINT16_MAX), var_name, (size_t)strnlen_s(var_val, UINT16_MAX), var_val);
#else
	ICEDB_error_context_add_string(c, strnlen(var_name, UINT16_MAX), var_name, strnlen(var_val, UINT16_MAX), var_val);
#endif
}
DL_ICEDB ICEDB_error_context_add_string2A_f ICEDB_error_context_add_string2A = error_context_add_string2;

void error_context_widen(struct ICEDB_error_context *c, size_t numNewSpaces)
{
	ICEDB_error_context_var_val* newvals = (ICEDB_error_context_var_val*)ICEDB_malloc(
		(numNewSpaces + c->max_num_var_fields) * (sizeof (ICEDB_error_context)));

	for (size_t i = 0; i < numNewSpaces; ++i) {
		newvals[i + c->max_num_var_fields].val = NULL;
		newvals[i + c->max_num_var_fields].varname = NULL;
	}
	if (c->var_vals) {
		for (size_t i = 0; i < c->max_num_var_fields; ++i) {
			size_t szval = sizeof(char) * strlen(c->var_vals[i].val);
			newvals[i].val = (char*)ICEDB_malloc(szval);
			ICEDB_COMPAT_strncpy_s(newvals[i].val, szval, c->var_vals[i].val, szval);

			size_t szname = sizeof(char) * strlen(c->var_vals[i].varname);
			newvals[i].varname = (char*)ICEDB_malloc(szname);
			ICEDB_COMPAT_strncpy_s(newvals[i].varname, szname, c->var_vals[i].varname, szname);
		}
		ICEDB_free(c->var_vals);
	}
	c->var_vals = newvals;
	c->max_num_var_fields += numNewSpaces;
}
DL_ICEDB ICEDB_error_context_widen_f ICEDB_error_context_widen = error_context_widen;



DL_ICEDB const struct ICEDB_error_context_container_ftable ICEDB_ct_error_context = {
	error_context_create_impl,
	error_context_copy,
	error_context_append,
	error_context_append_str,
	error_context_add_string,
	error_context_add_string2,
	error_context_widen
};
