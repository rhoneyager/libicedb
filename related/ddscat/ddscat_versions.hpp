#pragma once
#include <string>

namespace icedb
{
	namespace DDSCAT
	{
		/// Provide ddscat versioning information and comparisons.
		namespace versioning
		{
			/// Parses a string to attempt to guess the DDSCAT version
			size_t getVerId(const std::string&);

			// These functions each from a DDSCAT version string in 
			// slightly different ways

			/// Get ddscat version in form of "x.x.x"
			std::string getVerString(size_t id);
			/// Get header string in form of "DDSCAT 7.0.7 [09.12.11]"
			std::string getVerAvgHeaderString(size_t id);


			/// Useful functions for checking whether a ddscat version falls 
			/// within the specified ranges
			bool isVerWithin(size_t ver, const std::string &range);
			bool isVerWithin(size_t ver, size_t min, size_t max);

			/// Get the default ddscat version (may be overridden by user; 
			/// defaults to most recent known version)
			size_t getDefaultVer();
			/// Set the default ddscat version
			void setDefaultVer(size_t);
		}
	}
}
