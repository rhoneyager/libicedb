#include <icedb/defs.h>
/// This is a program that converts units
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <exception>
#include <iostream>
#include <icedb/units/units.hpp>
#include <icedb/error.hpp>

int main(int argc, char** argv) {
	using namespace std;
	int retval = 0;
	try {
		using namespace icedb;
		namespace po = boost::program_options;

		po::options_description desc("Allowed options"), cmdline("Command-line options"),
			config("Config options"), hidden("Hidden options"), oall("all options");

		cmdline.add_options()
			("help,h", "produce help message")
			("input,i", po::value< double >(), "Input quantity")
			("input-units,u", po::value< string >(), "Input units")
			("output-units,o", po::value< string >(), "Output units")
			("spec", "Perform spectral interconversion.")
			;

		po::positional_options_description p;
		p.add("input", 1);
		p.add("input-units", 1);
		p.add("output-units", 1);

		desc.add(cmdline).add(config);
		oall.add(cmdline).add(config).add(hidden);

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
			options(oall).positional(p).run(), vm);
		po::notify(vm);

		auto doHelp = [&desc](const std::string &msg) {
			std::cerr << msg << std::endl << desc << std::endl;
			exit(1);
		};
		if (vm.count("help")) doHelp("");

		double inVal, outVal;
		string inUnits, outUnits;
		bool isSpec = false;

		if (vm.count("spec")) isSpec = true;
		if (vm.count("input")) inVal = vm["input"].as<double>();
		else {
			std::string temp;
			cout << "Specify input number (without units): ";
			std::getline(cin, temp);
			try {
				inVal = boost::lexical_cast<double>(temp);
			}
			catch (boost::bad_lexical_cast) {
				ICEDB_throw(icedb::error::error_types::xBadInput)
					.add<std::string>("Reason", "Cannot parse input number")
					.add<string>("Input _Number_", temp);
			}
		}
		if (vm.count("input-units")) inUnits = vm["input-units"].as<string>();
		else {
			cout << "Specify input units (terminate with 'enter'): ";
			std::getline(cin, inUnits);
			if (!inUnits.size()) doHelp("Need to specify input units.");
		}
		if (vm.count("output-units")) outUnits = vm["output-units"].as<string>();
		else{
			cout << "Specify output units (terminate with 'enter'): ";
			std::getline(cin, outUnits);
			if (!outUnits.size()) doHelp("Need to specify output units.");
		}
		bool hasLenUnits = false;
		bool hasFreqUnits = false;
		if (icedb::units::converter(inUnits, "m").isValid()) hasLenUnits = true;
		if (icedb::units::converter(outUnits, "m").isValid()) hasLenUnits = true;
		if (icedb::units::converter(inUnits, "Hz").isValid()) hasFreqUnits = true;
		if (icedb::units::converter(outUnits, "Hz").isValid()) hasFreqUnits = true;

		if (!isSpec && hasLenUnits && hasFreqUnits) {
		//if ((!vm.count("input") || !vm.count("input-units") || !vm.count("output-units")) && !isSpec) {
			cout << "Is this an in-vacuo spectral unit conversion (i.e. GHz to mm) [yes]? ";
			string temp;
			std::getline(cin, temp);
			if (temp.size()) {
				char c = temp.at(0);
				switch (c) {
				case 'N': case 'n': case 'f': case 'F': case '0':
					isSpec = false;
					break;
				case 'Y': case 'y': case 't': case 'T': case '1':
					isSpec = true;
					break;
				default:
					ICEDB_throw(icedb::error::error_types::xBadInput)
						.add<std::string>("Reason", "Cannot parse boolean")
						.add<string>("Input _Bool_", temp);
				}
			}
			else isSpec = true;
		}

		std::shared_ptr<icedb::units::converter> cnv;

		if (isSpec) {
			cnv = std::shared_ptr<icedb::units::converter>(new icedb::units::conv_spec(inUnits, outUnits));
		}
		else {
			cnv = std::shared_ptr<icedb::units::converter>(new icedb::units::converter(inUnits, outUnits));
		}
		if (cnv->isValid()) {
			outVal = cnv->convert(inVal);
			cout << outVal << endl;
		}
		else cerr << "Conversion is invalid or unhandled for \"" << inUnits << "\" to \"" << outUnits << "\"." << endl;
	}
	catch (std::exception &e) {
		cerr << "An exception has occurred: " << e.what() << endl;
		retval = 2;
	}
	return retval;
}
