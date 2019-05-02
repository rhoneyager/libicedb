/// \brief Provides silo file IO

#include <string>

#include "defs.hpp"
#include "icedb/Plugins/plugin.hpp"
#include "plugin-io-ddscat.hpp"

namespace icedb {
	namespace plugins {
		namespace ddscat {
			ddscat_text_handle::ddscat_text_handle(const char* filename, IOtype t)
				: IOhandler(PLUGINID) //, file(nullptr, &nullFileCloser)
			{
				open(filename, t);
			}

			void ddscat_text_handle::open(const char* filename, IOtype t)
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

D_icedb_start()
{
	using namespace icedb::registry;
	using namespace icedb::plugins::ddscat;
	static const icedb::registry::DLLpreamble id(
		"Plugin-ddscat",
		"Example plugin to provide icedb with the ability to "
		"read and write ddscat files.",
		PLUGINID);
	dllInitResult res = icedb_registry_register_dll(id, (void*)dllStart);
	if (res != SUCCESS) return res;

	const size_t nexts = 5;
	const char *exts[nexts] = { "ddscat", "ddscat-shape", "ddscat7", "ddscat7-shape", "ddscat6-shape" };
	genAndRegisterIOregistryPlural_reader
		<::icedb::Shapes::NewShapeProperties, icedb::Shapes::_impl::ShapeProps_IO_Input_Registry>
		(1, exts, PLUGINID);

	return SUCCESS;
}
