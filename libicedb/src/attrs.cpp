#include "../icedb/data/attrs.h"
#include "../icedb/data/attrs.hpp"
#include "../icedb/error/error_context.h"

bool validate_attr_ptr(ICEDB_attr *attr) {
	if (!attr) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return false;
	}
	if (!attr->parent) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return false;
	}
	return true;
}
bool attr_close(ICEDB_attr *attr)
{
	if (!validate_attr_ptr(attr)) return false;
	delete[] attr->dims;
	delete[] attr->name;
	delete attr;
	return true;
}
DL_ICEDB ICEDB_attr_close_f ICEDB_attr_close = attr_close;

bool attr_write(ICEDB_attr *attr) {
	if (!validate_attr_ptr(attr)) return false;

	return true;
}