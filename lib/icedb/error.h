#pragma once
//#error "Replaced for now"
#ifndef ICEDB_H_ERROR
#define ICEDB_H_ERROR
#include "defs.h"
#include "errorCodes.h"
#include <stdio.h>
#include <stdint.h>
//#include <intsafe.h>

ICEDB_BEGIN_DECL_C

/** @defgroup errs Errors
*
* @{
**/
	/** Defines an error condition. This is an integer. Zero (0) indicates that there is no error. **/
	typedef int ICEDB_error_code;

	/** Support function to turn the general error code into a human-readable message.
	This function allows you to determine the size of the output buffer necessary to write the entire
	error message.
	\returns The minimum size of the character array needed to hold the message.
	\param err is the error code in question. **/
	typedef size_t (*ICEDB_error_code_to_message_size_f)(ICEDB_error_code err);
	extern DL_ICEDB ICEDB_error_code_to_message_size_f ICEDB_error_code_to_message_size;

	/** Support function to turn the general error code into a human-readable message.
	This function safely writes the error string. The buffer will always be null-terminated, either at the
	end of the written string, or at the end of the buffer. To query the necessary buffer size before writing,
	see the ICEDB_error_code_to_message_size function.

	\see ICEDB_error_code_to_message_size

	\returns The number of bytes actually written (including the NULL). 
	\param err is the error code in question. 
	\param buf_size is the size of the output buffer.
	\param buf is the buffer. **/
	typedef size_t (*ICEDB_error_code_to_message_f)(ICEDB_error_code err, size_t buf_size, char* buf);
	extern DL_ICEDB ICEDB_error_code_to_message_f ICEDB_error_code_to_message;

	/** Support function to write the error code to an output stream (C-style).
	This is a convenience function that allows for printing the error without an intermediate string copy,
	but streams are not thread-safe.
	\returns The number of bytes actually written (including the NULL).
	\param fp is the FILE pointer.
	\param err is the error code in question. **/
	typedef size_t (*ICEDB_error_code_to_stream_f)(ICEDB_error_code err, FILE* fp);
	extern DL_ICEDB ICEDB_error_code_to_stream_f ICEDB_error_code_to_stream;

	/** Defines an error context. This structure contains both an error code (for fast lookups) and
	any ancillary information to determine why / how the error occurred. The library keeps an internal buffer
	of these objects, and copies them when the program requests them. It is then the programmer's job
	to free all instances that are thus marshalled. **/
	struct ICEDB_error_context;

	/** Copy the last error context raised within the active thread. The resulting object should be freed once no longer needed. 
	Returns NULL if no context exists. **/
	typedef struct ICEDB_error_context* (*ICEDB_get_error_context_thread_local_f)();
	extern DL_ICEDB ICEDB_get_error_context_thread_local_f ICEDB_get_error_context_thread_local;

	/** Release the error context (this is the destructor, and frees the memory) **/
	typedef void (*ICEDB_error_context_deallocate_f)(struct ICEDB_error_context*);
	extern DL_ICEDB ICEDB_error_context_deallocate_f ICEDB_error_context_deallocate;

	/** Get the error code from the context. **/
	typedef ICEDB_error_code (*ICEDB_error_context_to_code_f)(const struct ICEDB_error_context*);
	extern DL_ICEDB ICEDB_error_context_to_code_f ICEDB_error_context_to_code;

	/** Determines the minimum buffer size for a human-readable representation of the ICEDB_error_context. **/
	typedef size_t (*ICEDB_error_context_to_message_size_f)(const struct ICEDB_error_context*);
	extern DL_ICEDB ICEDB_error_context_to_message_size_f ICEDB_error_context_to_message_size;
	/** Support function to turn the general error code into a human-readable message.
	This function safely writes the error string. The buffer will always be null-terminated, either at the
	end of the written string, or at the end of the buffer. To query the necessary buffer size before writing,
	see the ICEDB_error_context_to_message_size function.

	\see ICEDB_error_context_to_message_size

	\returns The number of bytes actually written (including the NULL).
	\param err is the error context in question.
	\param buf_size is the size of the output buffer.
	\param buf is the buffer. **/
	typedef size_t (*ICEDB_error_context_to_message_f)(const struct ICEDB_error_context* err, size_t buf_size, char* buf);
	extern DL_ICEDB ICEDB_error_context_to_message_f ICEDB_error_context_to_message;

	/** Support function to write the error code to an output stream (C-style).
	This is a convenience function that allows for printing the error without an intermediate string copy,
	but streams are not thread-safe.
	\returns The number of bytes actually written (including the NULL).
	\param fp is the FILE pointer.
	\param err is the error context in question. **/
	typedef size_t (*ICEDB_error_context_to_stream_f)(const struct ICEDB_error_context* err, FILE* fp);
	extern DL_ICEDB ICEDB_error_context_to_stream_f ICEDB_error_context_to_stream;

	/** Testing function that raises an error. **/
	typedef ICEDB_error_code (*ICEDB_error_test_f)();
	extern DL_ICEDB ICEDB_error_test_f ICEDB_error_test;

	/// Convenience function that returns an immutable string describing the OS type. Staticly allocated.
	typedef const char* (*ICEDB_error_getOSname_f)();
	extern DL_ICEDB ICEDB_error_getOSname_f ICEDB_error_getOSname;

	/// A convenience wrapper for all error functions
	struct ICEDB_error_container_ftable {
		ICEDB_error_code_to_message_size_f codeToCstrSize;
		ICEDB_error_code_to_message_f codeToCstr;
		ICEDB_error_code_to_stream_f codeToStream;
		ICEDB_get_error_context_thread_local_f getContextThreadLocal;
		ICEDB_error_context_deallocate_f contextFree;
		ICEDB_error_context_to_code_f contextToCode;
		ICEDB_error_context_to_message_size_f contextToCstrSize;
		ICEDB_error_context_to_message_f contextToCstr;
		ICEDB_error_context_to_stream_f contextToStream;
		ICEDB_error_test_f test;
		ICEDB_error_getOSname_f getOSname;
	};
	DL_ICEDB extern const struct ICEDB_error_container_ftable ICEDB_ct_error;

/** @} */ // end of errs
	ICEDB_END_DECL_C
#endif
