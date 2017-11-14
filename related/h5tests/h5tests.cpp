// This program is designed to write a header file that describes
// certain properties of the HDF5 C++ interface.
// The interface is problematic: between versions, some functions
// are moved between unrelated base classes, and this affects compilation.

#include <hdf5.h>
#include <iostream>

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
	using std::cout;

	std::string strVersion(H5_VERSION);
	version v(strVersion);
	version vLocation("1.10.1-patch0");

	bool hasDeprecatedCommonFG = (vLocation <= v);

	cout << "#pragma once\n"
		"#ifndef ICEDB_H5_TESTS_HDR\n"
		"#define ICEDB_H5_TESTS_HDR\n\n";

	if (hasDeprecatedCommonFG) {
		cout << "#define ICEDB_H5_OPENGROUP_IN_H5LOCATION 1\n";
		cout << "#define ICEDB_H5_GROUP_OWNER_PTR H5::H5Location*\n";
	} else {
		cout << "#define ICEDB_H5_OPENGROUP_IN_H5LOCATION 0\n";
		cout << "#define ICEDB_H5_GROUP_OWNER_PTR H5::CommonFG*\n";
	}

	cout << "\n#endif // End ICEDB_H5_TESTS_HDR\n";
}
