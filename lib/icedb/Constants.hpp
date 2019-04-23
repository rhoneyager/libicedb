#pragma once
#include "defs.h"
#include "HH/Groups.hpp"
#include <map>
#include <string>
#include <vector>

namespace icedb {
	namespace Constants {
		template <class T>
		T default_val() {
			return static_cast<T>(-999);
		}
		template <class T>
		T unknown() {
			return static_cast<T>(-888);
		}

		extern ICEDB_DL const std::string icedb_object_type;

	}
}
