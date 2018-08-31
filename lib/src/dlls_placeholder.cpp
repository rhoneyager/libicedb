#include "../icedb/defs.h"
#include "../icedb/versioning/versioning.hpp"
#include "../icedb/plugin.hpp"

// This file exists just to provide this symbol to plugins.
D_icedb_validator();

extern "C"
{
	SHARED_EXPORT_ICEDB void _ICEDB_dllPluginBase() noexcept {}
}
