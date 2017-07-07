#pragma once
#ifndef ICEDB_HPP_VERSIONING_FORWARDS
#define ICEDB_HPP_VERSIONING_FORWARDS
#include "defs.h"
#include "versioningForwards.h"
#include <memory>
ICEDB_BEGIN_DECL_CPP
namespace icedb {
	namespace versioning {
		struct versionInfo;
		typedef std::shared_ptr<const versionInfo> versionInfo_p;

		

		DL_ICEDB ICEDB_ver_match compareVersions(const versionInfo_p a, const versionInfo_p b);
		DL_ICEDB versionInfo_p getLibVersionInfo();
		
	}
}
ICEDB_END_DECL_CPP
#endif
