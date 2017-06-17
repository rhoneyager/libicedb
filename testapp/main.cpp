#include "../germany_api/error.h"
#include "../germany_api/error_context.h"
#include "../germany_api/mem.h"
#include "../germany_api/dllsImpl.h"
#include "../germany_api/error.hpp"
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
	auto dll = ICEDB_DLL_BASE_HANDLE_create("testdll.dll");
	auto td = create_testdll(dll);
	if (!td->_base->_vtable->open(td->_base)) {
		auto cxt = icedb::error::get_error_context_thread_local();
		printf("%s", icedb::error::stringify(cxt).c_str());
	}
	int res = td->testNum(td, 36);
	printf("Res is %d.\n", res);
	destroy_testdll(td);
	ICEDB_DLL_BASE_HANDLE_destroy(dll);
	

	return 0;
}
