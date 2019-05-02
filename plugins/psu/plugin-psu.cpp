#include <string>
#include "icedb/IO/Shapes.hpp"
#include "icedb/IO/exv.hpp"
#include "plugin-psu.hpp"

namespace icedb {
	namespace plugins {
		namespace psu {
			psu_handle::psu_handle(const char* filename, IOtype t)
				: IOhandler(PLUGINID), file(HH::Handles::HH_hid_t::dummy())
			{
				open(filename, t);
			}

			void psu_handle::open(const char* filename, IOtype t)
			{
				switch (t)
				{
				case IOtype::READWRITE:
					file = HH::File::openFile(filename, H5F_ACC_RDWR);
					break;
				case IOtype::EXCLUSIVE:
					file = HH::File::openFile(filename, H5F_ACC_EXCL);
					break;
				case IOtype::DEBUG:
					file = HH::File::openFile(filename, H5F_ACC_DEBUG);
					break;
				case IOtype::CREATE:
					file = HH::File::openFile(filename, H5F_ACC_TRUNC); // H5F_ACC_CREAT is deprecated.
					break;
				case IOtype::READONLY:
					file = HH::File::openFile(filename, H5F_ACC_RDONLY);
					break;
				case IOtype::TRUNCATE:
					file = HH::File::openFile(filename, H5F_ACC_TRUNC);
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

	genAndRegisterIOregistryPlural_reader
		<::icedb::exv::NewEXVrequiredProperties, icedb::exv::_impl::EXVProps_IO_Input_Registry>
		(1, exts, PLUGINID);

	//genAndRegisterIOregistry_writer<::icedb::Shapes::Shape,
	//	icedb::Shapes::_impl::Shape_IO_Output_Registry>("psu", PLUGINID);

	return SUCCESS;
}
