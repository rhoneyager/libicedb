/// \brief Provides silo file IO
#define _SCL_SECURE_NO_WARNINGS

#include <string>
#include <icedb/defs.h>
#include "plugin-qhull.hpp"

namespace icedb
{
	namespace plugins
	{
		namespace qhull
		{
			
		}
	}
}

D_icedb_start()
{
	using namespace icedb::registry;
	using namespace icedb::plugins::qhull;
	static const icedb::registry::DLLpreamble id(
		"Plugin-qhull",
		"Bindings for the qhull algorithms",
		PLUGINID);
	dllInitResult res = icedb_registry_register_dll(id, (void*)dllStart);
	if (res != SUCCESS) return res;

	//genAndRegisterIOregistry_writer<::icedb::Shapes::Shape,
	//	icedb::Shapes::_impl::Shape_IO_Output_Registry>("silo", PLUGINID);

	return SUCCESS;
}
