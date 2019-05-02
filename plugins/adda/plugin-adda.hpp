#pragma once
#pragma warning( disable : 4251 ) // warning C4251: dll-interface

#include <memory>
#include <string>
#include "defs.hpp"
#include "icedb/Plugins/plugin.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <cstdio>
#define PLUGINID "io-adda"

namespace icedb {
	namespace plugins {
		namespace adda {

			/** This is a handle - an opaque object - that can open the ADDA text.
			* This handle gets passed to the main program and icedb libraries in a few places. It holds
			* the HDF5 object that is used to read the files. When it is no longer needed, it is released and the
			* Penn State database file is closed.
			**/
			struct adda_handle : public registry::IOhandler
			{
				adda_handle(const char* filename, IOtype t);
				virtual ~adda_handle() {}
				void open(const char* filename, IOtype t);
				std::string filename;
				//std::unique_ptr < FILE, decltype(&fclose)> file;
			};
		}
	}
}

