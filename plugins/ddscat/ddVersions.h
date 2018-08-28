#pragma once
#include "defs.hpp"
#include <string>

namespace icedb
{
	namespace io
	{
		namespace ddscat
		{
			/// Provide ddscat versioning information and comparisons.
			namespace ddVersions
			{
				/// Parses a string to attempt to guess the DDSCAT version
				DL_ICEDB_IO_DDSCAT size_t getVerId(const std::string&);

				// These functions each from a DDSCAT version string in 
				// slightly different ways

				/// Get ddscat version in form of x.x.x
				DL_ICEDB_IO_DDSCAT std::string getVerString(size_t id);
				/// Get header string in form of DDSCAT 7.0.7 [09.12.11]
				DL_ICEDB_IO_DDSCAT std::string getVerAvgHeaderString(size_t id);


				/// Useful functions for checking whether a ddscat version falls 
				/// within the specified ranges
				DL_ICEDB_IO_DDSCAT bool isVerWithin(size_t ver, const std::string &range);
				DL_ICEDB_IO_DDSCAT bool isVerWithin(size_t ver, size_t min, size_t max);

				// Get the default ddscat version (may be overridden by user in future)
				//size_t getDefaultVer();
			}
		}
	}
}
