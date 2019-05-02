#include <string>

#include "icedb/defs.h"
#include "icedb/Plugins/plugin.hpp"
#include "icedb/IO/Shapes.hpp"
#include "plugin-example.hpp"

namespace icedb
{
	namespace plugins
	{
		namespace example
		{
			example_handle::example_handle(const char* filename, IOtype t)
				: IOhandler(PLUGINID)
			{
				open(filename, t);
			}

			void example_handle::open(const char* filename, IOtype t)
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
					file = std::shared_ptr<FILE>(fopen(filename, "bo"), &fclose);
					break;
				}
			}

		}
	}
}

/** \brief This is the entry point of the plugin.
*
* It is called first when icedb loads the plugin. It should do two things:
* - it needs to tell icedb about itself (i.e. name and a description).
* - it needs to tell icedb about the features that it provides.
**/
D_icedb_start()
{
	using namespace icedb::registry;
	using namespace icedb::plugins::example;
	// Tell icedb about the plugin.
	static const icedb::registry::DLLpreamble id(
		"Plugin-Example",
		"An example plugin. Does nothing useful.",
		PLUGINID);
	dllInitResult res = icedb_registry_register_dll(id, (void*)dllStart);
	if (res != SUCCESS) return res;

	// Register different features with icedb.
	// Note: All of these templated functions are defined in <icedb/plugin.hpp>.
	// The plugin.hpp definitions instruct icedb to look for functions that match certain
	// signatures. These are the functions that perform the registered 
	// reading and writing operations. Examples of these functions are given in:
	// shape-import-example.cpp, shape-export-example.cpp, 
	// scatering-data-import-example.cpp, and scattering-data-output-example.cpp.

	// This plugin can read shapes of the "example-in-shape" format.
	// The skeleton code for this is in shape-import-example.cpp.
	genAndRegisterIOregistry_reader
		<::icedb::Shapes::NewShapeProperties,			// Reads into a NewShapeProperties structure. 
		icedb::Shapes::_impl::ShapeProps_IO_Input_Registry>	// Register with the ShapeProps_IO_Input_Registry.
														// This gets consulted when we are caling any of the "read" functions.
		("example-in-shape",							// File format that this plugin handles.
			PLUGINID);									// Plugin ID (helps when debugging).

	// This plugin can write shapes into the "example-out-shape" format.
	// The skeleton code for this is in shape-export-example.cpp.
	// TODO!
	//genAndRegisterIOregistry_writer<
	//	::icedb::Shapes::Shape,							// Works on a ::icedb::Shapes::Shape object.
	//	icedb::Shapes::_impl::Shape_IO_Output_Registry>	// Register with the Shape_IO_Output_Registry.
	//													// This gets consulted when we are calling any of the "write" functions.
	//	("example-out-shape", PLUGINID);

	// This plugin can read scattering data in the "example-in-scattering-data" format.
	// The skeleton code for this is in scatering-data-import-example.cpp.
	// TODO!
	//genAndRegisterIOregistry_reader
	//	<::icedb::exv::NewEXVrequiredProperties,
	//	icedb::exv::_impl::EXVProps_IO_Input_Registry>
	//	("example-in-scattering-data", PLUGINID);

	// This plugin can write scattering data in the "example-out-scattering-data" format.
	// The skeleton code for this is in scattering-data-output-example.cpp.
	// TODO!

	return SUCCESS;
}
