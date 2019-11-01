#pragma once
#if defined(_MSC_FULL_VER)
#pragma warning( disable : 4251 ) // warning C4251: dll-interface
#endif
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

