#pragma once
#include "defs.h"
#include <array>
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

		namespace AttNames {
			extern ICEDB_DL const std::string icedb_object_type;
			extern ICEDB_DL const std::string icedb_git_hash;
			extern ICEDB_DL const std::string icedb_version;
		}

		extern ICEDB_DL const std::array<uint64_t, 3> version;
		extern ICEDB_DL const std::string GitHash;
		extern ICEDB_DL const std::string GitBranch;
		extern ICEDB_DL const std::string PackageGitBranch;
	}
}
