#pragma once
#pragma warning( disable : 4251 ) // warning C4251: dll-interface

#include <memory>
#include <string>
#include <icedb/shape.hpp>
#include <icedb/plugin.hpp>


#define PLUGINID "7375058F-3CBC-405D-8663-FE3267FDCBDD"


namespace icedb {
	namespace plugins {
		namespace qhull {
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

