#include "../germany_api/defs.h"

extern "C" {
	SHARED_EXPORT_ICEDB int testfunc(int a) {
		return (a < 16) ? 2 * a : 43;
	}

	static int val = 0;
	SHARED_EXPORT_ICEDB int set(int a) {
		val = a;
		return a;
	}
	SHARED_EXPORT_ICEDB int get() {
		return val;
	}
}