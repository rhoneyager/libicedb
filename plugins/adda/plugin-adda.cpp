#include <string>
#include <icedb/shape.hpp>
#include <icedb/error.hpp>
#include <BinaryIO/Files.hpp>
#include "plugin-adda.hpp"

/** This is a macro that defines a few symbols and functions that indicate that this is a valid icedb plugin.
 * It must always be present in a single file.
 **/
D_icedb_validator();

namespace icedb {
	namespace plugins {
		namespace adda {
			//int nullFileCloser(FILE*) { return 0; }

			adda_handle::adda_handle(const char* filename, IOtype t)
				: IOhandler(PLUGINID) //, file(nullptr, &nullFileCloser)
			{
				open(filename, t);
			}

			void adda_handle::open(const char* filename, IOtype t)
			{
				switch (t)
				{
				case IOtype::READONLY:
					this->filename = filename;
					//file.swap(bIO::_impl::openFile(filename, "rb"));
					//file = HH::File::openFile(filename, H5F_ACC_RDONLY);
					break;
				default:
					ICEDB_throw(icedb::error::error_types::xUnsupportedIOaction)
						.add<std::string>("Reason", "This plugin only supports reading files")
						.add<std::string>("filename", std::string(filename));
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
* In this example, the plugin gives icedb the ability to read ADDA shape files and scattering data.
**/
D_icedb_start()
{
	using namespace icedb::registry;
	using namespace icedb::plugins::adda;
	// Provide a brief description of the plugin.
	static const icedb::registry::DLLpreamble id(
		"Plugin-ADDA",
		"For reading and writing ADDA files.",
		PLUGINID);
	dllInitResult res = icedb_registry_register_dll(id, (void*)dllStart);
	if (res != SUCCESS) return res;

	// Tell icedb that this code can read shape files and phase function files.
	const size_t nexts = 2;
	const char *exts[nexts] = { "adda", "adda-shape" };
	genAndRegisterIOregistryPlural_reader
		<::icedb::Shapes::NewShapeProperties, icedb::Shapes::_impl::ShapeProps_IO_Input_Registry>
		(1, exts, PLUGINID);

	return SUCCESS;
}
