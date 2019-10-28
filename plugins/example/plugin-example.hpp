#pragma once
#pragma warning( disable : 4251 ) // warning C4251: dll-interface should be ignored

#include <memory>
#include <string>
#include "icedb/Plugins/plugin.hpp"
#include <cstdio>
#define PLUGINID "Example"


namespace icedb {
	namespace plugins {
		namespace example {

			struct example_handle : public registry::IOhandler
			{
				example_handle(const char* filename, IOtype t);
				virtual ~example_handle() {}
				void open(const char* filename, IOtype t);
				std::shared_ptr<FILE> file;
			};


		}
	}
}

