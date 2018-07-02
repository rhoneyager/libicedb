#pragma once
#ifndef ICEDB_HPP_VERSIONING
#define ICEDB_HPP_VERSIONING
#include "../defs.h"
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <cstring>
#include <string>
#include "versioningForwards.hpp"
#include "cmake-settings.h"
#include "../util.h"

#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)

struct ICEDB_VersionInfo {
	icedb::versioning::versionInfo_p p;
	std::string vstr;
};

namespace icedb {
	namespace versioning {
		/// Calculates version string based on compile-time version of external code.
		inline void genVersionInfo(versionInfo &out)
		{
			for (size_t i = 0; i < versionInfo::V_MAX_INTS; ++i) out.vn[i] = 0;
			for (size_t i = 0; i < versionInfo::V_MAX_BOOLS; ++i) out.vb[i] = false;
			out.vn[versionInfo::V_VERSIONINFO] = 1; // Change this if the structure is modified. Differences are ALWAYS incompatible.
			out.vsdate[0] = '\0'; out.vssource[0] = '\0'; out.vsuuid[0] = '\0';
			out.vboost[0] = '\0'; out.vassembly[0] = '\0';
			ICEDB_COMPAT_strncpy_s(out.vdate, versionInfo::charmax, __DATE__, versionInfo::charmax);
			ICEDB_COMPAT_strncpy_s(out.vtime, versionInfo::charmax, __TIME__, versionInfo::charmax);
			ICEDB_COMPAT_strncpy_s(out.vgithash, versionInfo::charmax, libicedb_GitHash, versionInfo::charmax);
			ICEDB_COMPAT_strncpy_s(out.vgitbranch, versionInfo::charmax, libicedb_GitBranch, versionInfo::charmax);

			out.vn[versionInfo::V_MAJOR] = libicedb_MAJOR;
			out.vn[versionInfo::V_MINOR] = libicedb_MINOR;
			out.vn[versionInfo::V_REVISION] = libicedb_REVISION;


#ifdef __GNUC__
			out.vn[versionInfo::V_GNUC_MAJ] = __GNUC__;
			out.vn[versionInfo::V_GNUC_MIN] = __GNUC_MINOR__;
			out.vn[versionInfo::V_GNUC_PATCH] = __GNUC_PATCHLEVEL__;
#endif
#ifdef __MINGW32__
			out.vn[versionInfo::V_MINGW_MAJ] = __MINGW32_MAJOR_VERSION;
			out.vn[versionInfo::V_MINGW_MIN] = __MINGW32_MINOR_VERSION;
#endif
#ifdef __SUNPRO_CC
			out.vn[versionInfo::V_SUNPRO] = __SUNPRO_CC;
#endif
#ifdef __PATHCC__
			out.vn[versionInfo::V_PATHCC_MAJ] = __PATHCC__;
			out.vn[versionInfo::V_PATHCC_MIN] = __PATHCC_MINOR__;
			out.vn[versionInfo::V_PATHCC_PATCH] = __PATHCC_PATCHLEVEL__;
#endif
#ifdef __llvm__
			out.vb[versionInfo::V_LLVM] = true;
#endif
#ifdef __clang__
			out.vn[versionInfo::V_CLANG_MAJ] = __clang_major__;
			out.vn[versionInfo::V_CLANG_MIN] = __clang_minor__;
			out.vn[versionInfo::V_CLANG_PATCH] = __clang_patchlevel__;
#endif
#ifdef __INTEL_COMPILER
			out.vn[versionInfo::V_INTEL] = __INTEL_COMPILER;
			out.vn[versionInfo::V_INTEL_DATE] = __INTEL_COMPILER_BUILD_DATE;
#endif
#ifdef _MSC_FULL_VER
			out.vn[versionInfo::V_MSCVER] = _MSC_FULL_VER;
#endif
#ifdef BOOST_LIB_VERSION
			inline_strncpy_s(out.vboost, versionInfo::charmax, BOOST_LIB_VERSION, versionInfo::charmax);
#endif
#ifdef _DEBUG
			out.vb[versionInfo::V_DEBUG] = true;
#endif
#ifdef _OPENMP
			out.vb[versionInfo::V_OPENMP] = true;
#endif
#ifdef __amd64
			out.vb[versionInfo::V_AMD64] = true;
#endif
#ifdef _M_X64
			out.vb[versionInfo::V_X64] = true;
#endif
#ifdef __unix__
			out.vb[versionInfo::V_UNIX] = true;
#endif
#ifdef __APPLE__
			out.vb[versionInfo::V_APPLE] = true;
#endif
#ifdef _WIN32
			out.vb[versionInfo::V_WIN32] = true;
#endif
		}
		inline versionInfo_p genVersionInfo() {
			std::shared_ptr<versionInfo> res(new versionInfo);
			genVersionInfo(*(res.get()));
			versionInfo_p r(res);
			return r;
		}
		/**
		* \brief Provides information about the build environment during compilation.
		*
		* This function is designed to provide information on a compiler's
		* build environment. It is a header function because it is designed
		* to reflect the compiler variables of an external project's code.
		*
		* @param out The output stream that receives the information.
		*/
		inline void debug_preamble(const versionInfo &v, std::ostream &out = std::cerr)
		{
			out << "Compiled on " << v.vdate << " at " << v.vtime << std::endl;
			out << "Version " << v.vn[versionInfo::V_MAJOR] << "." << v.vn[versionInfo::V_MINOR]
				<< "." << v.vn[versionInfo::V_REVISION] << std::endl;
			if (v.vgitbranch[0] != '\0') out << "GIT Branch " << v.vgitbranch << std::endl;
			if (v.vgithash[0] != '\0') out << "GIT Hash " << v.vgithash << std::endl;
			if (v.vn[versionInfo::V_SVNREVISION]) out << "SVN Revision " << v.vn[versionInfo::V_SVNREVISION] << std::endl;
			if (v.vsdate[0] != '\0') out << "SVN Revision Date: " << v.vsdate << std::endl;
			if (v.vssource[0] != '\0') out << "SVN Source: " << v.vssource << std::endl;
			if (v.vsuuid[0] != '\0') out << "SVN UUID: " << v.vsuuid << std::endl;
			if (v.vb[versionInfo::V_DEBUG]) out << "Debug Version" << std::endl;
			else out << "Release Version" << std::endl;
			if (v.vb[versionInfo::V_OPENMP]) out << "OpenMP enabled in Compiler" << std::endl;
			else out << "OpenMP disabled in Compiler" << std::endl;
			if (v.vb[versionInfo::V_AMD64] || v.vb[versionInfo::V_X64]) out << "64-bit build" << std::endl;
			if (v.vb[versionInfo::V_UNIX]) out << "Unix / Linux Compile" << std::endl;
			if (v.vb[versionInfo::V_APPLE]) out << "Mac Os X Compile" << std::endl;
			if (v.vb[versionInfo::V_WIN32]) out << "Windows Compile" << std::endl;

			if (v.vn[versionInfo::V_GNUC_MAJ])
				out << "GNU Compiler Suite " << v.vn[versionInfo::V_GNUC_MAJ] << "."
				<< v.vn[versionInfo::V_GNUC_MIN] << "." << v.vn[versionInfo::V_GNUC_PATCH] << std::endl;
			if (v.vn[versionInfo::V_MINGW_MAJ])
				out << "MinGW Compiler Suite " << v.vn[versionInfo::V_MINGW_MAJ] << "."
				<< v.vn[versionInfo::V_MINGW_MIN] << std::endl;
			if (v.vn[versionInfo::V_SUNPRO])
				out << "Sun Studio Compiler Suite " << v.vn[versionInfo::V_SUNPRO] << std::endl;
			if (v.vn[versionInfo::V_PATHCC_MAJ])
				out << "EKOPath Compiler " << v.vn[versionInfo::V_PATHCC_MAJ] << "."
				<< v.vn[versionInfo::V_PATHCC_MIN] << "." << v.vn[versionInfo::V_PATHCC_PATCH] << std::endl;
			if (v.vb[versionInfo::V_LLVM]) out << "LLVM Compiler Suite" << std::endl;
			if (v.vn[versionInfo::V_CLANG_MAJ])
				out << "clang compiler " << v.vn[versionInfo::V_CLANG_MAJ] << "."
				<< v.vn[versionInfo::V_CLANG_MIN] << "." << v.vn[versionInfo::V_CLANG_PATCH] << std::endl;
			if (v.vn[versionInfo::V_INTEL])
				out << "Intel Suite " << v.vn[versionInfo::V_INTEL]
				<< " Date " << v.vn[versionInfo::V_INTEL_DATE] << std::endl;
			if (v.vn[versionInfo::V_MSCVER])
				out << "Microsoft Visual Studio Compiler Version " << v.vn[versionInfo::V_MSCVER] << std::endl;
			if (v.vboost[0]) out << "Boost version " << v.vboost << std::endl;


			out << std::endl;
			out << std::endl;
		}

		inline void debug_preamble(std::ostream &out = std::cerr)
		{
			versionInfo v;
			genVersionInfo(v);
			debug_preamble(v, out);
		}
	}
}
#undef QUOTE
#undef EXPAND_AND_QUOTE
#endif