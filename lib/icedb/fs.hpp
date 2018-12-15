#pragma once
#include <map>
#include <memory>
#include <string>
//#include "Data_Types.hpp"

namespace icedb {

	namespace fs {
		enum class ObjectTypes {
			ANY,
			SHAPE
		};

		enum class IOopenFlags {
			DEFAULT,
			READ_ONLY,
			READ_WRITE,
			TRUNCATE,
			CREATE,
			UNKNOWN
		};

		enum class RecursionType {
			UNKNOWN,
			BASE,
			ONE,
			SUBTREE
		};

		RecursionType getRecursionType(const ::std::string&);
		IOopenFlags getIOType(const ::std::string&);
		/// \returns true on success, false on failure.
		bool explodeHDF5Path(const ::std::string &inPath, ::std::string &fsPath, ::std::string &h5path);
	}
}
