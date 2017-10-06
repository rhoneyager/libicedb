#include "../icedb/data/attrs.h"
#include "../icedb/data/attrs.hpp"

bool attr_close(ICEDB_ATTR *attr)
{
	return false;
}

DL_ICEDB ICEDB_ATTR_close_f ICEDB_ATTR_close = attr_close;



void a()
{
	
}