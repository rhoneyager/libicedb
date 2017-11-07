#include "cdffs.h"
#include "shape.hpp"
#include "fs_backend.hpp"

int main(void) {
	if (sfs::exists("testdataset")) {
		auto db = icedb::fs::Database::openDatabase("testdataset", icedb::fs::IOopenFlags::READ_WRITE);
		icedb::fs::Database::indexDatabase("testdataset");
	}
	else {
		icedb::fs::Database::createDatabase("testdataset");
	}
	return 0;
}