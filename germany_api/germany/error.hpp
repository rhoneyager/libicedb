#pragma once
#ifndef ICEDB_HPP_ERROR
#define ICEDB_HPP_ERROR
#include "defs.h"
#include "errorCodes.h"
#include "error.h"
#include <string>
#include <iostream>
#include <memory>
#include <cstdint>

ICEDB_BEGIN_DECL_CPP

namespace icedb {
	namespace error {


		/** Defines an error condition. This is an integer. Zero (0) indicates that there is no error. **/
		typedef ICEDB_error_code error_code_t;

		ICEDB_SYMBOL_SHARED void stringify(error_code_t err, std::string &);
		ICEDB_SYMBOL_SHARED void stringify(error_code_t err, char* const* );
		template <class StringType> StringType stringify(error_code_t err) { return StringType(); }
		template<> ICEDB_SYMBOL_SHARED std::string stringify(error_code_t err);
		//extern template std::string stringify<std::string>(error_code_t err);
		template<> ICEDB_SYMBOL_SHARED const char* stringify(error_code_t err);
		//extern template const char* stringify<const char*>(error_code_t err);

		/** Defines an error context. This structure contains both an error code (for fast lookups) and
		any ancillary information to determine why / how the error occurred. The library keeps an internal buffer
		of these objects, and copies them when the program requests them. It is then the programmer's job
		to free all instances that are thus marshalled. **/
		typedef std::unique_ptr<ICEDB_error_context, decltype(&ICEDB_error_context_deallocate)> error_context_pt;

		/** Copy the last error context raised within the active thread. The resulting object should be freed once no longer needed.
		Returns NULL if no context exists. **/
		ICEDB_SYMBOL_SHARED error_context_pt get_error_context_thread_local();

		/** Get the error code from the context. Shouldn't be necessary. **/
		ICEDB_SYMBOL_SHARED error_code_t error_context_to_code(const error_context_pt&);

		ICEDB_SYMBOL_SHARED void stringify(const error_context_pt &err, std::string &);
		ICEDB_SYMBOL_SHARED std::string stringify(const error_context_pt &err);

	}
}

ICEDB_END_DECL_CPP
#endif
