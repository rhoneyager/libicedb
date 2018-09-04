#include "../icedb/defs.h"
#include "../icedb/versioning/versioning.hpp"
#include "../icedb/dlls.hpp"

// This file exists just to provide this symbol to plugins.
//D_icedb_validator();

extern "C" SHARED_EXPORT_ICEDB void dlVer_impl(icedb::versioning::versionInfo& vf, void** rd)
{
	icedb::versioning::genVersionInfo(vf);
	*rd = (void*) &(icedb_registry_register_dll);
}

