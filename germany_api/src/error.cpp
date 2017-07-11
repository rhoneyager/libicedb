#define __STDC_WANT_LIB_EXT1__ 1
#include "../germany/error/error.h"
#include "../germany/error/error_context.h"
#include "../germany/error/error.hpp"
#include "../germany/misc/util.h"
#include "../germany/misc/mem.h"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
ICEDB_BEGIN_DECL_C

ICEDB_SYMBOL_SHARED ICEDB_error_code ICEDB_error_context_to_code(const struct ICEDB_error_context* err) {
	if (!err) ICEDB_DEBUG_RAISE_EXCEPTION();
	return err->code;
}

ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_context_to_message_size(const struct ICEDB_error_context* err) {
	if (!err) ICEDB_DEBUG_RAISE_EXCEPTION();
	size_t total = 1;
	uint16_t buf_size = UINT16_MAX - 1;
	char buf[UINT16_MAX];
	if (err->message_text) {
		total += ICEDB_COMPAT_strncpy_s(buf, buf_size, err->message_text, err->message_size); // Reports the number of characters copied. Note lack of count of null.
	}
	// Write the error variables
	const char* sep = "\t%s\t=\t%s\n";
	const size_t seplen = strlen(sep);

#ifdef ICEDB_USING_SECURE_STRINGS
	if (err->num_var_fields)
		for (int i = 0; i < err->num_var_fields; ++i)
			total += sprintf_s(buf + total, buf_size - total, sep, err->var_vals[i].varname, err->var_vals[i].val);
#else
	size_t expectedSize = 0;
	if (err->num_var_fields) {
		for (int i = 0; i < err->num_var_fields; ++i) {
			// Manually size a temporary buffer, and then copy the data.
			expectedSize += seplen + strlen(err->var_vals[i].val) + strlen(err->var_vals[i].varname);
		}
	}
	char *tempbuf = (char*)ICEDB_malloc(sizeof(char)*expectedSize);
	size_t j = 0;
	if (err->num_var_fields) {
		for (int i = 0; i < err->num_var_fields; ++i) {
			j += sprintf(tempbuf + j, sep, err->var_vals[i].varname, err->var_vals[i].val);
		}
	}
	ICEDB_COMPAT_strncpy_s(buf + total, buf_size - total, tempbuf, j);
	total += j;
	ICEDB_free(tempbuf);
#endif

	if (total >= UINT16_MAX) ICEDB_DEBUG_RAISE_EXCEPTION();
	return uint16_t(total);
}

ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_context_to_message(const struct ICEDB_error_context * err, size_t buf_size, char * buf)
{
	if (!err) ICEDB_DEBUG_RAISE_EXCEPTION();
	if (!buf) ICEDB_DEBUG_RAISE_EXCEPTION();
	size_t total = 0;
	if (err->message_text) {
		total = ICEDB_COMPAT_strncpy_s(buf, buf_size, err->message_text, err->message_size); // Reports the number of characters copied. Note lack of count of null.
	}
	// Write the error variables
	const char* sep = "\t%s\t=\t%s\n";
	const size_t seplen = strlen(sep);
	
#ifdef ICEDB_USING_SECURE_STRINGS
	if (err->num_var_fields)
		for (int i = 0; i < err->num_var_fields; ++i)
			total += sprintf_s(buf+total, buf_size - total, sep, err->var_vals[i].varname, err->var_vals[i].val);
#else
	size_t expectedSize = 0;
	if (err->num_var_fields) {
		for (int i = 0; i < err->num_var_fields; ++i) {
			// Manually size a temporary buffer, and then copy the data.
			expectedSize += seplen + strlen(err->var_vals[i].val) + strlen(err->var_vals[i].varname);
		}
	}
	char *tempbuf = (char*)ICEDB_malloc(sizeof(char)*expectedSize);
	size_t j = 0;
	if (err->num_var_fields) {
		for (int i = 0; i < err->num_var_fields; ++i) {
			j += sprintf(tempbuf + j, sep, err->var_vals[i].varname, err->var_vals[i].val);
		}
	}
	ICEDB_COMPAT_strncpy_s(buf + total, buf_size - total, tempbuf, j);
	total += j;
	ICEDB_free(tempbuf);
#endif

	if (total >= UINT16_MAX) ICEDB_DEBUG_RAISE_EXCEPTION();
	return uint16_t(total);
}

ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_context_to_stream(const struct ICEDB_error_context * err, FILE * fp)
{
	if (!err) ICEDB_DEBUG_RAISE_EXCEPTION();
	if (!fp) ICEDB_DEBUG_RAISE_EXCEPTION();
	int res = 0;
	if (err->message_text) {
		res = ICEDB_COMPAT_fputs_s(err->message_text, fp);
	}
	// Error validation
	if (res == EOF) {
		printf("Error in writing error context to stream. Error code is %d. Context text is: %s.\n",
			ferror(fp), err->message_text);
	}

	// Write the error variables
	if (err->num_var_fields) {
		for (int i = 0; i < err->num_var_fields; ++i)
			ICEDB_COMPAT_fprintf_s(fp, "\t%s\t=\t%s\n", err->var_vals[i].varname, err->var_vals[i].val);
	}
	return uint16_t(res);
}

ICEDB_SYMBOL_SHARED ICEDB_error_code ICEDB_error_test()
{
	ICEDB_error_context *cxt = ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
	ICEDB_error_context_append_str(cxt, "This is a test.\n");
	return ICEDB_ERRORCODES_TODO;
}

ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_code_to_message_size(ICEDB_error_code err)
{
	if (err >= ICEDB_ERRORCODES_TOTAL) ICEDB_DEBUG_RAISE_EXCEPTION();
	uint16_t res = (uint16_t)ICEDB_COMPAT_strnlen_s(ICEDB_ERRORCODES_MAP[err],
		UINT16_MAX - 1) + 1; // -1,+1 because message_size includes the null character.
	return res;
}

ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_code_to_message(ICEDB_error_code err, size_t buf_size, char * buf)
{
	if (!buf) ICEDB_DEBUG_RAISE_EXCEPTION();
	if (err >= ICEDB_ERRORCODES_TOTAL) ICEDB_DEBUG_RAISE_EXCEPTION();
	uint16_t cnt = (uint16_t)ICEDB_COMPAT_strnlen_s(ICEDB_ERRORCODES_MAP[err],
		UINT16_MAX - 1) + 1; // -1,+1 because message_size includes the null character.
	size_t res = ICEDB_COMPAT_strncpy_s(buf, buf_size, ICEDB_ERRORCODES_MAP[err], cnt);
	return uint16_t((res>UINT16_MAX) ? UINT16_MAX : res);
}

ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_code_to_stream(ICEDB_error_code err, FILE * fp)
{
	if (!fp) ICEDB_DEBUG_RAISE_EXCEPTION();
	if (err >= ICEDB_ERRORCODES_TOTAL) ICEDB_DEBUG_RAISE_EXCEPTION();
	size_t res = ICEDB_COMPAT_fputs_s(ICEDB_ERRORCODES_MAP[err], fp);
	
	return uint16_t(res);
}

ICEDB_SYMBOL_SHARED struct ICEDB_error_context * ICEDB_get_error_context_thread_local()
{
	return __ICEDB_LOCAL_THREAD_error_context;
}

ICEDB_SYMBOL_SHARED void ICEDB_error_context_deallocate(struct ICEDB_error_context *c)
{
	if (!c) ICEDB_DEBUG_RAISE_EXCEPTION();
	if (c->message_text) ICEDB_free(c->message_text);
	if (c->var_vals) { // Should always exist, as the construction function automatically widens / allocates.
		for (int i = 0; i < c->num_var_fields; ++i) {
			ICEDB_free(c->var_vals[i].val);
			ICEDB_free(c->var_vals[i].varname);
		}
		ICEDB_free(c->var_vals);
	}
	ICEDB_free(c);
}

DL_ICEDB const char* ICEDB_error_getOSname() {
	const char* name =
#if defined(__FreeBSD__)
		"FreeBSD";
#elif defined(__NetBSD__)
		"NetBSD";
#elif defined(__OpenBSD__)
		"OpenBSD";
#elif defined(__bsdi__)
		"bsdi";
#elif defined(__DragonFly__)
		"DragonFly BSD";
#elif defined (__APPLE__)
		"Apple";
#elif defined(__linux__)
		"Linux";
#elif defined(_WIN32)
		"Windows";
#elif defined(__unix__)
		"Generic Unix";
#else
		"UNKNOWN";
#endif
	return name;
}

ICEDB_END_DECL_C

ICEDB_BEGIN_DECL_CPP
namespace icedb {
	namespace error {
		template<> ICEDB_SYMBOL_SHARED std::string stringify<std::string>(error_code_t err) {
			std::string res;
			stringify(err, res);
			return res;
		}
		template<> ICEDB_SYMBOL_SHARED const char* stringify<const char*>(error_code_t err) {
			return ICEDB_ERRORCODES_MAP[err];
		}


		ICEDB_SYMBOL_SHARED error_context_pt get_error_context_thread_local() {
			return error_context_pt(ICEDB_get_error_context_thread_local(),&ICEDB_error_context_deallocate);
		}

		ICEDB_SYMBOL_SHARED void stringify(error_code_t err, std::string &res) {
			res = std::string(ICEDB_ERRORCODES_MAP[err]);
		}
		ICEDB_SYMBOL_SHARED void stringify(error_code_t err, const char** res) {
			*res = ICEDB_ERRORCODES_MAP[err];
		}
		ICEDB_SYMBOL_SHARED void stringify(const error_context_pt &err, std::string &res) {
			// A few memory copies occur here. Inefficient, but errors should not occur much in properly running code.
			uint16_t sz = ICEDB_error_context_to_message_size(err.get());
			//std::unique_ptr<char[]> buf(new char[sz]);
			std::vector<char> buf(sz);
			ICEDB_error_context_to_message(err.get(), sz, buf.data());
			res = std::string(buf.data());
		}
		ICEDB_SYMBOL_SHARED std::string stringify(const error_context_pt &err) {
			std::string res;
			stringify(err, res);
			return res;
		}

		ICEDB_SYMBOL_SHARED error_code_t error_context_to_code(const error_context_pt& err) { return ICEDB_error_context_to_code(err.get()); }
	}
}
ICEDB_END_DECL_CPP
