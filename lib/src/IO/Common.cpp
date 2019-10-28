#include "icedb/defs.h"
#include "icedb/IO/Common.hpp"
#include "icedb/misc/Constants.hpp"
#include "icedb-cmake-settings.h"

namespace icedb {
	//const std::string _icedb_obj_type_identifier = "_icedb_obj_type";
	namespace Constants {
		namespace AttNames {
			ICEDB_DL const std::string icedb_object_type{ "_icedb_obj_type" };
			ICEDB_DL const std::string icedb_git_hash{ "_icedb_git_hash" };
			ICEDB_DL const std::string icedb_version{ "_icedb_version" };
		}

		ICEDB_DL const std::array<uint64_t, 3> version{ libicedb_MAJOR , libicedb_MINOR , libicedb_REVISION };
		ICEDB_DL const std::string GitHash{ libicedb_GitHash };
		ICEDB_DL const std::string GitBranch{ libicedb_GitBranch };
		ICEDB_DL const std::string PackageGitBranch{ libicedb_PackageGitBranch };
	}
}
