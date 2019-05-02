#pragma once
#pragma warning( disable : 4251 ) // warning C4251: dll-interface

#include <memory>
#include <string>
#include <cstdint>
#include "defs.hpp"
#include "icedb/Plugins/plugin.hpp"
#include "HH/Groups.hpp"
#include "HH/Files.hpp"

#define PLUGINID "io-psu"

namespace icedb {
	namespace plugins {
		namespace psu {
			/// Identify the particle type from the filename.
			std::string getParticleID(const std::string &filename, int32_t particle_index);
			struct particle_info {
				std::string particle_id;
				int particle_number;
				std::string particle_type;
				std::string scattMeth;
				std::string freq_number_as_string;
				std::string freq_units;
			};
			particle_info getParticleInfo(const std::string &filename);

			/** This is a handle - an opaque object - that can open the Penn State netCDF files.
			* This handle gets passed to the main program and icedb libraries in a few places. It holds
			* the HDF5 object that is used to read the files. When it is no longer needed, it is released and the
			* Penn State database file is closed.
			**/
			struct psu_handle : public registry::IOhandler
			{
				psu_handle(const char* filename, IOtype t);
				virtual ~psu_handle() {}
				void open(const char* filename, IOtype t);
				HH::File file;
			};
		}
	}
}

