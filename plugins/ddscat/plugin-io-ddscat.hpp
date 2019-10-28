#pragma once
#pragma warning( disable : 4251 ) // warning C4251: dll-interface

#include "defs.hpp"
#include <memory>
#include <string>
#include "icedb/IO/Shapes.hpp"
#include "icedb/Plugins/plugin.hpp"


#define PLUGINID "io-ddscat"

namespace icedb {
	namespace plugins {
		namespace ddscat {
			/** This is a handle - an opaque object - that can open the ADDA text.
			**/
			struct ddscat_text_handle : public registry::IOhandler
			{
				ddscat_text_handle(const char* filename, IOtype t);
				virtual ~ddscat_text_handle() {}
				void open(const char* filename, IOtype t);
				std::string filename;
				//std::unique_ptr < FILE, decltype(&fclose)> file;
			};
		}
	}
}

