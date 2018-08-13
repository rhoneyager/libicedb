#include <string>
#include <icedb/shape.hpp>
#include "plugin-psu.hpp"

/** This is a macro that defines a few symbols and functions that indicate that this is a valid icedb plugin.
 * It must always be present in a single file.
 **/
D_icedb_validator();

namespace icedb
{
	namespace plugins
	{
		namespace psu
		{
			
			psu_handle::psu_handle(const char* filename, IOtype t)
				: IOhandler(PLUGINID), grp(HH::Handles::HH_hid_t::dummy())
			{
				open(filename, t);
			}

			void psu_handle::open(const char* filename, IOtype t)
			{
				switch (t)
				{
				case IOtype::READWRITE:
					ICEDB_throw(icedb::error::error_types::xUnimplementedFunction);
					//file = std::shared_ptr<siloFile>(new siloFile(filename, H5F_ACC_RDWR ));
					break;
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
					break;
				case IOtype::TRUNCATE:
					//file = std::shared_ptr<siloFile>(new siloFile(filename));
					break;
				}
			}

		}
	}
}

/** 
* Plugins add additional features to icedb, but they have to tell icedb what they offer. The D_icedb_start function 
* does this; it is the "main" function for each plugin. 
* 
* When icedb is first used, it checks its "icedb-plugins" directory for shared objects (.so and .dll files)
* that can be dynamically loaded at runtime. It will open each object that it finds in this directory. It checks that
* the plugin is compatable with the core library, and then it passes control to the D_icedb_start function, which
* tells icedb the features that the plugin provides.
*
* In this example, the plugin gives icedb the ability to read Penn State shape files and scattering data.
**/
D_icedb_start()
{
	using namespace icedb::registry;
	using namespace icedb::plugins::psu;
	// Provide a brief description of the plugin.
	static const icedb::registry::DLLpreamble id(
		"Plugin-PSU",
		"For reading and writing Aydin et al. files.",
		PLUGINID);
	dllInitResult res = icedb_registry_register_dll(id, (void*)dllStart);
	if (res != SUCCESS) return res;

	// Tell icedb that this code can read shape files and phase function files.
	const size_t nexts = 2;
	const char *exts[nexts] = { "psu", "psu-shape" };
	genAndRegisterIOregistryPlural_reader
		<::icedb::Shapes::NewShapeProperties, icedb::Shapes::_impl::ShapeProps_IO_Input_Registry>
		(1, exts, PLUGINID);

	//genAndRegisterIOregistry_writer<::icedb::Shapes::Shape,
	//	icedb::Shapes::_impl::Shape_IO_Output_Registry>("psu", PLUGINID);

	return SUCCESS;
}
