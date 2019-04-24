#pragma once
#ifndef ICEDB_HPP_VERSIONING_FORWARDS
#define ICEDB_HPP_VERSIONING_FORWARDS
#include "../defs.h"
#include "versioningForwards.h"
#include <memory>
#include <iostream>
ICEDB_BEGIN_DECL_CPP
namespace icedb {
	namespace versioning {
		struct versionInfo {
			enum nums {
				V_VERSIONINFO,
				V_MAJOR, V_MINOR, V_REVISION, V_SVNREVISION,
				V_MSCVER,
				V_GNUC_MAJ, V_GNUC_MIN, V_GNUC_PATCH,
				V_MINGW_MAJ, V_MINGW_MIN,
				V_SUNPRO,
				V_PATHCC_MAJ, V_PATHCC_MIN, V_PATHCC_PATCH,
				V_CLANG_MAJ, V_CLANG_MIN, V_CLANG_PATCH,
				V_INTEL, V_INTEL_DATE,
				V_MAX_INTS
			};
			enum bools {
				V_DEBUG, V_OPENMP, V_AMD64, V_X64, V_UNIX, V_APPLE, V_WIN32,
				V_LLVM,
				V_MAX_BOOLS
			};

			uint64_t vn[V_MAX_INTS];
			bool vb[V_MAX_BOOLS];

			static const uint64_t charmax = 256;
			char vdate[charmax];
			char vtime[charmax];
			char vsdate[charmax];
			char vssource[charmax];
			char vsuuid[charmax];
			char vboost[charmax];
			char vassembly[charmax];
			char vgithash[charmax];
			char vgitbranch[charmax];
		};
		typedef std::shared_ptr<const versionInfo> versionInfo_p;

		

		ICEDB_DL ICEDB_ver_match compareVersions(const versionInfo_p a, const versionInfo_p b);
		ICEDB_DL versionInfo_p getLibVersionInfo();
		/**
		* \brief Provides information about the build environment during compilation.
		*
		* This function is designed to provide information on a compiler's
		* build environment. It is a header function because it is designed
		* to reflect the compiler variables of an external project's code.
		*
		* @param out The output stream that receives the information.
		* @param v is the version information to be printed.
		*/
		ICEDB_DL void debug_preamble(const versionInfo &v, std::ostream &out = std::cerr);
	}
}
ICEDB_END_DECL_CPP
#endif
