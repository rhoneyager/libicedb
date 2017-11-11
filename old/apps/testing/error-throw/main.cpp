#include <iostream>
#include <functional>
#include "../../../libicedb/icedb/error/error.hpp"
#include "../../../libicedb/icedb/error/error.h"
#include "../../../libicedb/icedb/error/error_context.h"

int main(int, char**) {
	ICEDB_error_code code = ICEDB_error_test();
	if (code) {
		ICEDB_error_context *cxt = ICEDB_get_error_context_thread_local();
		char buf[5000] = "\0";
		ICEDB_error_context_to_message(cxt, 5000, buf);
		printf("%s", buf);
		ICEDB_error_context_deallocate(cxt);
	}
	return 0;
}