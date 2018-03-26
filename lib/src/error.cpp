#include "../icedb/error.hpp"
#include "../private/options.hpp"
#include "../icedb/error_context.h"
#include "../icedb/error.h"
#include "../icedb/util.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <cwchar>
#include <complex>
#include <list>
#include <sstream>

ICEDB_BEGIN_DECL_C

ICEDB_error_code error_context_to_code(const struct ICEDB_error_context* err) {
	if (!err) ICEDB_DEBUG_RAISE_EXCEPTION();
	return err->code;
}
DL_ICEDB ICEDB_error_context_to_code_f ICEDB_error_context_to_code = error_context_to_code;

size_t error_context_to_message_size(const struct ICEDB_error_context* err) {
	if (!err) ICEDB_DEBUG_RAISE_EXCEPTION();
	size_t total = 1;
	const size_t buf_size = 1000;
	char buf[buf_size];
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

	return size_t(total);
}
DL_ICEDB ICEDB_error_context_to_message_size_f ICEDB_error_context_to_message_size = error_context_to_message_size;

size_t error_context_to_message(const struct ICEDB_error_context * err, size_t buf_size, char * buf)
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

	return size_t(total);
}
DL_ICEDB ICEDB_error_context_to_message_f ICEDB_error_context_to_message = error_context_to_message;

size_t error_context_to_stream(const struct ICEDB_error_context * err, FILE * fp)
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
DL_ICEDB ICEDB_error_context_to_stream_f ICEDB_error_context_to_stream = error_context_to_stream;

ICEDB_error_code error_test()
{
	ICEDB_error_context *cxt = ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
	ICEDB_error_context_append_str(cxt, "This is a test.\n");
	return ICEDB_ERRORCODES_TODO;
}
DL_ICEDB ICEDB_error_test_f ICEDB_error_test = error_test;

size_t error_code_to_message_size(ICEDB_error_code err)
{
	if (err >= ICEDB_ERRORCODES_TOTAL) ICEDB_DEBUG_RAISE_EXCEPTION();
	size_t res = (size_t)ICEDB_COMPAT_strnlen_s(ICEDB_ERRORCODES_MAP[err],
		1000) + 1; // -1,+1 because message_size includes the null character.
	return res;
}
DL_ICEDB ICEDB_error_code_to_message_size_f ICEDB_error_code_to_message_size = error_code_to_message_size;

size_t error_code_to_message(ICEDB_error_code err, size_t buf_size, char * buf)
{
	if (!buf) ICEDB_DEBUG_RAISE_EXCEPTION();
	if (err >= ICEDB_ERRORCODES_TOTAL) ICEDB_DEBUG_RAISE_EXCEPTION();
	uint16_t cnt = (uint16_t)ICEDB_COMPAT_strnlen_s(ICEDB_ERRORCODES_MAP[err],
		SIZE_MAX - 1) + 1; // -1,+1 because message_size includes the null character.
	size_t res = ICEDB_COMPAT_strncpy_s(buf, buf_size, ICEDB_ERRORCODES_MAP[err], cnt);
	return size_t((res>SIZE_MAX) ? SIZE_MAX : res);
}
DL_ICEDB ICEDB_error_code_to_message_f ICEDB_error_code_to_message = error_code_to_message;

size_t error_code_to_stream(ICEDB_error_code err, FILE * fp)
{
	if (!fp) ICEDB_DEBUG_RAISE_EXCEPTION();
	if (err >= ICEDB_ERRORCODES_TOTAL) ICEDB_DEBUG_RAISE_EXCEPTION();
	size_t res = ICEDB_COMPAT_fputs_s(ICEDB_ERRORCODES_MAP[err], fp);

	return size_t(res);
}
DL_ICEDB ICEDB_error_code_to_stream_f ICEDB_error_code_to_stream = error_code_to_stream;

struct ICEDB_error_context * get_error_context_thread_local_c()
{
	return __ICEDB_LOCAL_THREAD_error_context;
}
DL_ICEDB ICEDB_get_error_context_thread_local_f ICEDB_get_error_context_thread_local = get_error_context_thread_local_c;

void error_context_deallocate(struct ICEDB_error_context *c)
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
DL_ICEDB ICEDB_error_context_deallocate_f ICEDB_error_context_deallocate = error_context_deallocate;

const char* error_getOSname() {
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
DL_ICEDB ICEDB_error_getOSname_f ICEDB_error_getOSname = error_getOSname;

DL_ICEDB const struct ICEDB_error_container_ftable ICEDB_ct_error = {
	error_code_to_message_size,
	error_code_to_message,
	error_code_to_stream,
	get_error_context_thread_local_c,
	error_context_deallocate,
	error_context_to_code,
	error_context_to_message_size,
	error_context_to_message,
	error_context_to_stream,
	error_test,
	error_getOSname
};

ICEDB_END_DECL_C

namespace icedb {
	namespace error {
		class error_options_inner {
		public:
			std::list<::icedb::registry::const_options_ptr> stk;
			::icedb::registry::options_ptr cur;
			error::error_types et;
			std::string emessage;
		};
		xError::xError(error::error_types xe) {
			ep = std::shared_ptr<error_options_inner>(new error_options_inner);
			ep->et = xe;
		}
		xError::~xError() {}
		const char* xError::what() const noexcept {
			//static const char* msg = "An unknown error has occurred.";
			//return msg;
			std::ostringstream o;
			o << "error: " << stringify(ep->et) << std::endl;
			// Pull from stack
			int i = 1;
			for (const auto &e : ep->stk) {
				o << "Throw frame " << i << std::endl;
				e->enumVals(o);
				++i;
			}
			ep->emessage = o.str();
			return ep->emessage.c_str();
		}
		xError& xError::push(::icedb::registry::options_ptr op)
		{
			ep->stk.push_back(op);
			ep->cur = op;
			return *this;
		}
		xError& xError::push()
		{
			::icedb::registry::options_ptr op = ::icedb::registry::options::generate();
			ep->stk.push_back(op);
			ep->cur = op;
			return *this;
		}

		template <class T> xError& xError::add(const std::string &key, const T &value)
		{
			if (!ep->cur) push();
			this->ep->cur->add<T>(key, value);
			return *this;
		}

#define DOTYPES(f) f(int); f(float); f(double); f(long); f(long long); \
	f(unsigned int); f(unsigned long); f(unsigned long long); f(std::string); f(bool); f(std::complex<double>);

#define IMPL_xError_ADD(T) template xError& xError::add<T>(const std::string&, const T&);
		DOTYPES(IMPL_xError_ADD);


		template<> DL_ICEDB std::string stringify<std::string>(error_code_t err) {
			std::string res;
			stringify(err, res);
			return res;
		}
		template<> DL_ICEDB const char* stringify<const char*>(error_code_t err) {
			return ICEDB_ERRORCODES_MAP[err];
		}


		DL_ICEDB error_context_pt get_error_context_thread_local() {
			return error_context_pt(get_error_context_thread_local_c(), error_context_deallocate);
		}

		DL_ICEDB void stringify(error_code_t err, std::string &res) {
			res = std::string(ICEDB_ERRORCODES_MAP[err]);
		}
		DL_ICEDB void stringify(error_code_t err, const char** res) {
			*res = ICEDB_ERRORCODES_MAP[err];
		}
		DL_ICEDB void stringify(const error_context_pt &err, std::string &res) {
			// A few memory copies occur here. Inefficient, but errors should not occur much in properly running code.
			size_t sz = ICEDB_error_context_to_message_size(err.get());
			//std::unique_ptr<char[]> buf(new char[sz]);
			std::vector<char> buf(sz);
			ICEDB_error_context_to_message(err.get(), sz, buf.data());
			res = std::string(buf.data());
		}
		DL_ICEDB std::string stringify(const error_context_pt &err) {
			std::string res;
			stringify(err, res);
			return res;
		}

		DL_ICEDB error_code_t error_context_to_code(const error_context_pt& err) { return ICEDB_error_context_to_code(err.get()); }

	}
}

