/// \brief Provides silo file IO
#define _SCL_SECURE_NO_WARNINGS

#include <string>
#include <icedb/versioning/versioning.hpp>
#include <icedb/defs.h>
#include "plugin-silo.hpp"
#include "WritePoints.h"

D_icedb_validator();

namespace icedb
{
	namespace plugins
	{
		namespace silo
		{
			silo_handle::silo_handle(const char* filename, IOtype t)
				: IOhandler(PLUGINID)
			{
				open(filename, t);
			}

			void silo_handle::open(const char* filename, IOtype t)
			{
				switch (t)
				{
				case IOtype::READWRITE:
					//file = std::shared_ptr<siloFile>(new siloFile(filename, H5F_ACC_RDWR ));
					//break;
				case IOtype::EXCLUSIVE:
					//file = std::shared_ptr<siloFile>(new siloFile(filename, H5F_ACC_EXCL ));
					//break;
				case IOtype::DEBUG:
					//file = std::shared_ptr<siloFile>(new siloFile(filename, H5F_ACC_DEBUG ));
					//break;
				case IOtype::CREATE:
					//file = std::shared_ptr<siloFile>(new siloFile(filename, H5F_ACC_CREAT ));
					//break;
				case IOtype::READONLY:
					//file = std::shared_ptr<siloFile>(new siloFile(filename, H5F_ACC_RDONLY ));
					ICEDB_throw(icedb::error::error_types::xUnimplementedFunction);
					break;
				case IOtype::TRUNCATE:
					file = std::shared_ptr<siloFile>(new siloFile(filename));
					break;
				}
			}

		}
	}
}

D_icedb_start()
{
	using namespace icedb::registry;
	using namespace icedb::plugins::silo;
	static const icedb::registry::DLLpreamble id(
		"Plugin-SILO",
		"Example plugin to provide icedb with the ability to "
		"read and write SILO files.",
		PLUGINID);
	dllInitResult res = icedb_registry_register_dll(id, (void*)dllStart);
	if (res != SUCCESS) return res;

	genAndRegisterIOregistry_writer<::icedb::Shapes::Shape,
		icedb::Shapes::_impl::Shape_IO_Output_Registry>("silo", PLUGINID);

	return SUCCESS;
}
