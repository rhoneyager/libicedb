#include "../germany_api/defs.h"

extern "C" {
	SHARED_EXPORT_ICEDB int testfunc(int a) {
		return (a < 16) ? 2 * a : 42;
	}
}