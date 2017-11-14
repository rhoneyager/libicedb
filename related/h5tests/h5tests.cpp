// This program is designed to write a header file that describes
// certain properties of the HDF5 C++ interface.
// The interface is problematic: between versions, some functions
// are moved between unrelated base classes, and this affects compilation.

#include <hdf5.h>
#include <iostream>
#include <fstream>
#include <string>

struct version {
	int maj = 0;
	int min = 0;
	int rev = 0;
	int patch = 0;
	version() {}
	version(const std::string &strver) {
		// Versions look like 1.10.0-patch1
		using namespace std;
		size_t delim1 = strver.find_first_of(".");
		size_t delim2 = strver.find_first_of(".", delim1+1);
		size_t delim3 = strver.find_first_of("-");
		size_t delim4 = strver.find_first_of("h");
		string smaj = strver.substr(0, delim1);
		string smin = strver.substr(delim1+1, delim2-delim1-1);
		string srev, spatch;
		if (delim3 != string::npos) {
			srev = strver.substr(delim2+1, delim3-delim2-1);
			spatch = strver.substr(delim4+1);
		} else {
			srev = strver.substr(delim2+1);
		}
		maj = std::stoi(smaj);
		min = std::stoi(smin);
		rev = std::stoi(srev);
		if (spatch.size()) patch = std::stoi(spatch);
	}
	bool operator<(const version &rhs) {
		if (maj != rhs.maj) return maj < rhs.maj;
		if (min != rhs.min) return min < rhs.min;
		if (rev != rhs.rev) return rev < rhs.rev;
		if (patch != rhs.patch) return patch < rhs.patch;
		return false;
	}
	bool operator==(const version &rhs) {
		if (maj != rhs.maj) return false;
		if (min != rhs.min) return false;
		if (rev != rhs.rev) return false;
		if (patch != rhs.patch) return false;
		return true;
	}
	bool operator<=(const version &rhs) {
		if (operator==(rhs)) return true;
		if (operator<(rhs)) return true;
		return false;
	}
	bool operator!=(const version &rhs) {return !operator==(rhs);}
};
int main() {
	std::ofstream out("icedb_h5.h");

	std::string strVersion(H5_VERSION);
	version v(strVersion);
	version vLocation("1.10.1-patch0");

	bool hasDeprecatedCommonFG = (vLocation <= v);

	out << "#pragma once\n"
		"#ifndef ICEDB_H5_TESTS_HDR\n"
		"#define ICEDB_H5_TESTS_HDR\n\n";


	// 1.10.0	H5::CommonFG has getNumObjs.
	// 1.10.1	H5::Group has getNumObjs.
	//
	// 1.10.0	H5::Location has getId. H5::CommonFG does not. Need separate defs for H5::Group and H5::H5File.
	// 1.10.1	
	if (hasDeprecatedCommonFG) {
		out << "#define ICEDB_H5_OPENGROUP_IN_H5LOCATION 1\n";
		out << "#define ICEDB_H5_GROUP_OWNER_PTR H5::Group*\n";
		out << "#define ICEDB_H5_GROUP_OWNER H5::Group\n";
		//out << "#define ICEDB_H5_GROUP_OWNER_PTR H5::H5Location*\n";
		//out << "#define ICEDB_H5_GROUP_OWNER H5::H5Location\n";
		// See https://support.hdfgroup.org/HDF5/doc/cpplus_RM/class_h5_1_1_h5_location.html
		// looks like getNumObjs() should have been moved to H5::Location in 1.10.1,
		// but they seem to have forgotten.
		// It's okay, as H5::Group is a child of H5::Location.
		out << "#define ICEDB_H5_GETNUMOBJS_OWNER H5::Group\n";
		out << "#define ICEDB_H5_GETNUMOBJS_OWNER_PTR H5::Group*\n";
		out << "#define ICEDB_H5_UNIFIED_GROUP_FILE 1\n";
	} else {
		out << "#define ICEDB_H5_OPENGROUP_IN_H5LOCATION 0\n";
		out << "#define ICEDB_H5_GROUP_OWNER_PTR H5::CommonFG*\n";
		out << "#define ICEDB_H5_GROUP_OWNER H5::CommonFG\n";
		out << "#define ICEDB_H5_GETNUMOBJS_OWNER H5::CommonFG\n";
		out << "#define ICEDB_H5_GETNUMOBJS_OWNER_PTR H5::CommonFG*\n";
		out << "#define ICEDB_H5_UNIFIED_GROUP_FILE 0\n";
	}



	out << "\n#endif // End ICEDB_H5_TESTS_HDR\n";
}
