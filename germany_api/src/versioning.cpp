#include "../germany/versioning.hpp"
#include "../germany/util.h"
#include <sstream>
namespace icedb {
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
			tryBool(V_HAS_ZLIB);
			tryBool(V_HAS_GZIP);
			tryBool(V_HAS_BZIP2);
			tryBool(V_HAS_SZIP);

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
