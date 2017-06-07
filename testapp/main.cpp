#include "../germany_api/error.h"
#include "../germany_api/error_context.h"
#include "../germany_api/mem.h"
#include <stdio.h>

//#ifdef __cplusplus
//extern "C" {
//#endif
	int main(int, char**) {
		ICEDB_error_context *cxt = ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		ICEDB_error_context_append_str(cxt, "This is a test.\n");
		ICEDB_error_context_append_str(cxt, "This is a second test.\n");

		//ICEDB_error_context_to_stream(cxt, stdout);
		char buf[5000] = "\0";
		ICEDB_error_context_to_message(cxt, 5000, buf);
		printf("%s", buf);
		ICEDB_error_context_deallocate(cxt);
		return 0;
	}

//#ifdef __cplusplus
//}
//#endif