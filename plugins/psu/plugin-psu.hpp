#pragma once
#pragma warning( disable : 4251 ) // warning C4251: dll-interface

#include <memory>
#include <string>
#include "defs.hpp"
#include <icedb/plugin.hpp>

#define PLUGINID "io-psu"


namespace icedb {
	namespace plugins {
		namespace psu {
			/*
			class siloFile;

			struct silo_handle : public registry::IOhandler
			{
				silo_handle(const char* filename, IOtype t);
				virtual ~silo_handle() {}
				void open(const char* filename, IOtype t);
				std::shared_ptr<siloFile> file;
			};
			*/


		}
	}
}

