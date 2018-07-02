#pragma once
#include "defs.h"
#include <exception>
#include <memory>
#include <string>
#include "optionsForwards.hpp"
#include "errorTypes.hpp"
#include "error.h"

namespace icedb {
	namespace error {
		class error_options_inner;
		class xError;
		typedef std::shared_ptr<xError> xError_ptr;

		// The C++ interface to the C-style (non-exceptions) errors:
		// These C++ functions may throw exceptions. Their C counterparts will not.
		// The possible exceptions involve use of shared pointers and C++ strings.

		/** Defines an error condition. This is an integer. Zero (0) indicates that there is no error. **/
		typedef int error_code_t;

		DL_ICEDB void stringify(error_code_t err, std::string &);
		DL_ICEDB void stringify(error_code_t err, char* const*);
		template <class StringType> StringType stringify(error_code_t err) { return StringType(); }
		template<> DL_ICEDB std::string stringify(error_code_t err);
		//extern template std::string stringify<std::string>(error_code_t err);
		template<> DL_ICEDB const char* stringify(error_code_t err);
		//extern template const char* stringify<const char*>(error_code_t err);

		/** Defines an error context. This structure contains both an error code (for fast lookups) and
		any ancillary information to determine why / how the error occurred. The library keeps an internal buffer
		of these objects, and copies them when the program requests them. It is then the programmer's job
		to free all instances that are thus marshalled. **/
		typedef std::shared_ptr<ICEDB_error_context> error_context_pt;

		/** Copy the last error context raised within the active thread. The resulting object should be freed once no longer needed.
		Returns NULL if no context exists. **/
		DL_ICEDB error_context_pt get_error_context_thread_local();

		/** Get the error code from the context. Shouldn't be necessary. **/
		DL_ICEDB error_code_t error_context_to_code(const error_context_pt&);

		DL_ICEDB void stringify(const error_context_pt &err, std::string &);
		DL_ICEDB std::string stringify(const error_context_pt &err);



		/// xError is the base class for icedb C++ exceptions.
		/// C code should use the C interface.
		class xError : public std::exception
		{
		protected:
			std::shared_ptr<error_options_inner> ep;
		public:
			xError(error_types = error_types::xOtherError);
			xError(error_context_pt p);
			virtual ~xError();
			virtual const char* what() const noexcept;

			/// Insert a context of Errors.
			//void push(::icedb::registry::const_options_ptr);
			xError& push(::icedb::registry::options_ptr);
			xError& push();

			template <class T> xError& add(const std::string &key, const T value);

		};
	}
}

#ifdef _MSC_FULL_VER
#define ICEDB_FUNCSIG __FUNCSIG__
#endif
#ifdef __GNUC__
#define ICEDB_FUNCSIG __PRETTY_FUNCTION__
#endif

#define ICEDB_RSpushErrorvars \
	.add<std::string>("source_filename", std::string(__FILE__)) \
	.add<int>("source_line", (int)__LINE__) \
	.add<std::string>("source_function", std::string(ICEDB_FUNCSIG))
#define ICEDB_RSmkError(x) ::icedb::error::xError(x).push() \
	ICEDB_RSpushErrorvars
/// \todo Detect if inherits from std::exception or not.
/// If inheritable, check if it is an xError. If yes, push a new context.
/// If not inheritable, push a new context with what() as the expression.
/// If not an exception, then create a new xError and push the appropriate type in a context.
#define ICEDB_throw(x) throw ICEDB_RSmkError(x)
