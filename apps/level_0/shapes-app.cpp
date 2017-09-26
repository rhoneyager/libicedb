#include <iostream>
#include <memory>
#include <string>
#include <boost/program_options.hpp>
#include "../../libicedb/icedb/misc/os_functions.h"
#include "../../libicedb/icedb/level_0/shape.h"
int main(int argc, char** argv) {
	using namespace std;
	ICEDB_libEntry(argc, argv);

	namespace po = boost::program_options;
	po::options_description desc("Allowed options");

	desc.add_options()
		("help,h", "produce help message")
		("input,i", po::value<vector<string> >()->multitoken(), "Input shape file(s)")
		("output,o", po::value<string>(), "Output shape file")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
		options(desc).run(), vm);
	po::notify(vm);

	auto doHelp = [&](const std::string& s)
	{
		cout << s << endl;
		cout << desc << endl;
		exit(3);
	};
	if (vm.count("help") || argc <= 1) doHelp("");
	if (!vm.count("input") || !vm.count("output")) doHelp("Must specify input and output files.");
	string sOutput = vm["output"].as<string>();
	vector<string> sInputs = vm["input"].as<vector<string>>();


	return 0;
}
