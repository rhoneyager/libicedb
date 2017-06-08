#pragma once
#ifndef ICEDB_H_ERROR
#define ICEDB_H_ERROR
#include "defs.h"
#include "errorCodes.h"
#include <stdio.h>
#include <stdint.h>
#include <intsafe.h>
#ifdef __cplusplus
extern "C" {
#endif

	/** Defines an error condition. This is an integer. Zero (0) indicates that there is no error. **/
	typedef uint16_t ICEDB_error_code;

	/** Support function to turn the general error code into a human-readable message.
	This function allows you to determine the size of the output buffer necessary to write the entire
	error message.
	\returns The minimum size of the character array needed to hold the message.
	\param err is the error code in question. **/
	ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_code_to_message_size(ICEDB_error_code err);

	/** Support function to turn the general error code into a human-readable message.
	This function safely writes the error string. The buffer will always be null-terminated, either at the
	end of the written string, or at the end of the buffer. To query the necessary buffer size before writing,
	see the ICEDB_error_code_to_message_size function.

	\see ICEDB_error_code_to_message_size

	\returns The number of bytes actually written (including the NULL). 
	\param err is the error code in question. 
	\param buf_size is the size of the output buffer.
	\param buf is the buffer. **/
	ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_code_to_message(ICEDB_error_code err, size_t buf_size, char* buf);

	/** Support function to write the error code to an output stream (C-style).
	This is a convenience function that allows for printing the error without an intermediate string copy,
	but streams are not thread-safe.
	\returns The number of bytes actually written (including the NULL).
	\param fp is the FILE pointer.
	\param err is the error code in question. **/
	ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_code_to_stream(ICEDB_error_code err, FILE* fp);

	/** Defines an error context. This structure contains both an error code (for fast lookups) and
	any ancillary information to determine why / how the error occurred. The library keeps an internal buffer
	of these objects, and copies them when the program requests them. It is then the programmer's job
	to free all instances that are thus marshalled. **/
	struct ICEDB_error_context;

	/** Copy the last error context raised within the active thread. The resulting object should be freed once no longer needed. 
	Returns NULL if no context exists. **/
	ICEDB_SYMBOL_SHARED struct ICEDB_error_context* ICEDB_get_error_context_thread_local();

	/** Release the error context (this is the destructor, and frees the memory) **/
	ICEDB_SYMBOL_SHARED void ICEDB_error_context_deallocate(struct ICEDB_error_context*);

	/** Get the error code from the context. Shouldn't be necessary. **/
	ICEDB_SYMBOL_SHARED ICEDB_error_code ICEDB_error_context_to_code(const struct ICEDB_error_context*);

	/** Determines the minimum buffer size for a human-readable representation of the ICEDB_error_context. **/
	ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_context_to_message_size(const struct ICEDB_error_context*);
	/** Support function to turn the general error code into a human-readable message.
	This function safely writes the error string. The buffer will always be null-terminated, either at the
	end of the written string, or at the end of the buffer. To query the necessary buffer size before writing,
	see the ICEDB_error_context_to_message_size function.

	\see ICEDB_error_context_to_message_size

	\returns The number of bytes actually written (including the NULL).
	\param err is the error context in question.
	\param buf_size is the size of the output buffer.
	\param buf is the buffer. **/
	ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_context_to_message(const struct ICEDB_error_context* err, size_t buf_size, char* buf);

	/** Support function to write the error code to an output stream (C-style).
	This is a convenience function that allows for printing the error without an intermediate string copy,
	but streams are not thread-safe.
	\returns The number of bytes actually written (including the NULL).
	\param fp is the FILE pointer.
	\param err is the error context in question. **/
	ICEDB_SYMBOL_SHARED uint16_t ICEDB_error_context_to_stream(const struct ICEDB_error_context* err, FILE* fp);

	/** Testing function that raises an error. **/
	ICEDB_SYMBOL_SHARED ICEDB_error_code ICEDB_error_test();
#ifdef __cplusplus
}
#endif
#endif
