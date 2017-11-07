#pragma once
#include <map>
#include <memory>
#include <string>
#include "gsl/span"
#include "Data_Types.hpp"

namespace icedb {

	namespace fs {
		enum class ObjectTypes {
			ANY,
			SHAPE
		};

		enum class IOopenFlags {
			READ_ONLY,
			READ_WRITE,
			TRUNCATE,
			CREATE
		};

	}
}