#pragma once
#pragma warning( disable : 4251 ) // warning C4251: dll-interface

#include <memory>
#include <string>
#include <icedb/shape.hpp>
#include <icedb/plugin.hpp>


#define PLUGINID "f8340412-f146-47c4-8b32-a395d829f7b2"


namespace icedb {
	namespace plugins {
		namespace silo {
			class siloFile;

			struct silo_handle : public registry::IOhandler
			{
				silo_handle(const char* filename, IOtype t);
				virtual ~silo_handle() {}
				void open(const char* filename, IOtype t);
				std::shared_ptr<siloFile> file;
			};


		}
	}
}

