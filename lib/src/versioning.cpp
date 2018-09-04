#include "../icedb/versioning/versioning.hpp"
#include "../icedb/util.h"
#include <sstream>
namespace icedb {
	namespace os_functions {
		std::string getLibExecDir() {
			return std::string(libicedb_libexecdir);
		}

	}
	namespace versioning {
			namespace internal {
			versionInfo_p ver_int;
		}
		ICEDB_ver_match compareVersions(const versionInfo_p a, const versionInfo_p b)
		{
			ICEDB_ver_match res = ICEDB_VER_INCOMPATIBLE;
			if (!a || !b) return res;

#define tryNum(x) if (a->vn[versionInfo:: x] != b->vn[versionInfo:: x]) return res;
#define tryBool(x) if (a->vb[versionInfo:: x] != b->vb[versionInfo:: x]) return res;
#define tryStr(x) if (std::strncmp(a-> x, b-> x, versionInfo::charmax ) != 0) return res;
#define tryNumB(x) if ((a->vn[versionInfo:: x] != b->vn[versionInfo:: x]) && a->vn[versionInfo:: x]) return res;
			// First filter the incompatible stuff
			tryNum(V_VERSIONINFO);
			tryStr(vassembly);
			tryNum(V_MAJOR);
			tryNum(V_MINOR);
			tryBool(V_AMD64);
			tryBool(V_X64);
			tryBool(V_UNIX);
			tryBool(V_APPLE);
			tryBool(V_WIN32);
			tryStr(vboost);
			tryNum(V_MSCVER);

			res = ICEDB_VER_COMPATIBLE_1;
			tryNumB(V_GNUC_MAJ);
			tryNumB(V_MINGW_MAJ);
			tryNumB(V_SUNPRO);
			tryNumB(V_PATHCC_MAJ);
			tryNumB(V_CLANG_MAJ);
			tryNumB(V_INTEL);

			res = ICEDB_VER_COMPATIBLE_2;
			tryNumB(V_GNUC_MIN);
			tryNumB(V_MINGW_MIN);
			tryNumB(V_PATHCC_MIN);
			tryNumB(V_CLANG_MIN);
			tryNum(V_REVISION);

			res = ICEDB_VER_COMPATIBLE_3;
			tryBool(V_OPENMP);
			tryNum(V_SVNREVISION);
			tryStr(vgithash);

			res = ICEDB_VER_EXACT_MATCH;
			return res;
#undef tryNum
#undef tryBool
#undef tryStr
#undef tryNumB
		}

		void getLibVersionInfo(versionInfo &out)
		{
			genVersionInfo(out);
		}

		versionInfo_p getLibVersionInfo()
		{
			if (internal::ver_int) return internal::ver_int;
			std::shared_ptr<versionInfo> vi(new versionInfo);
			getLibVersionInfo(*(vi.get()));
			internal::ver_int = vi;
			return internal::ver_int;
		}

		void debug_preamble(const versionInfo &v, std::ostream &out)
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

	}
}


ICEDB_BEGIN_DECL_C

void ICEDB_VersionInfo_Free(ICEDB_VersionInfo_p p) { if (p) delete p; }

ICEDB_VersionInfo_p ICEDB_getLibVersionInfo() {
	ICEDB_VersionInfo_p res(new ICEDB_VersionInfo);
	res->p = icedb::versioning::getLibVersionInfo();
	return res;
}

const char* ICEDB_WriteLibVersionInfoCC(ICEDB_VersionInfo_p p) {
	if (p->vstr.size()) return p->vstr.c_str();
	std::ostringstream out;
	icedb::versioning::debug_preamble(*(p->p.get()), out);
	p->vstr = out.str();
	return p->vstr.c_str();
}
char* ICEDB_WriteLibVersionInfoC(ICEDB_VersionInfo_p p, char* buf, size_t sz) {
	ICEDB_WriteLibVersionInfoCC(p);
	ICEDB_COMPAT_strncpy_s(buf, sz, p->vstr.c_str(), p->vstr.size());
	return buf;
}
ICEDB_ver_match ICEDB_CompareVersions(const ICEDB_VersionInfo_p a, const ICEDB_VersionInfo_p b) {
	return icedb::versioning::compareVersions(a->p, b->p);
}

ICEDB_END_DECL_C
