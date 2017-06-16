#include "../germany_api/error.h"
#include "../germany_api/error_context.h"
#include "../germany_api/mem.h"
#include "../germany_api/dllsImpl.h"
#include <stdio.h>

	int main(int, char**) {
		/*
		ICEDB_error_code code = ICEDB_error_test();
		if (code) {
			ICEDB_error_context *cxt = ICEDB_get_error_context_thread_local();
			char buf[5000] = "\0";
			ICEDB_error_context_to_message(cxt, 5000, buf);
			printf("%s", buf);
			ICEDB_error_context_deallocate(cxt);
		}
		*/
		auto td = create_testdll(NULL);
		int res = td->testNum(td, 2);
		printf("Res is %d.\n", res);
		destroy_testdll(td);
		return 0;
	}
