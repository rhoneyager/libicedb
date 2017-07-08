#include <iostream>
#include <memory>
#include <string>
#include "../../germany_api/germany/os_functions.h"
#include "../../germany_api/germany/units.hpp"
int main(int argc, char** argv) {
	using namespace std;
	ICEDB_libEntry(argc, argv);

	cout << "Enter input value: ";
	double ival = 0;
	cin >> ival;
	cout << "Enter input units: ";
	string iunits, ounits;
	cin >> iunits;
	cout << "Enter output units: ";
	cin >> ounits;

	auto cnv = icedb::units::converter::generate("", iunits.c_str(), ounits.c_str());
	if (!cnv) {
		cerr << "Cannot establish converter\n";
		return 1;
	}
	if (!cnv->isValid()) {
		cerr << "Cannot establish converter\n";
		return 1;
	}
	cout << cnv->convert(ival) << endl;

	return 0;
}
