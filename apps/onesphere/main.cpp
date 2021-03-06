#include "icedb/defs.h"
#include "icedb/refract/refract.hpp"
#include "icedb/units/units.hpp"
#include "icedb/Errors/error.hpp"
#include "icedb/icedb.hpp"
/// This is a program that calculates scattering by individual spheres.
/// This program takes as many options as there are ways to specify target geometry and properties.
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <exception>
#include <iostream>


namespace std
{
	/// Used when listing the program options
	std::ostream& operator<<(std::ostream &os, const std::vector<std::string> &vec)
	{
		for (auto item : vec)
		{
			os << item << " ";
		}
		return os;
	}
}

int main(int argc, char** argv) {
	using namespace std;
	int retval = 0;
	try {
		using namespace icedb;
		namespace po = boost::program_options;

		po::options_description desc("Allowed options"), 
			subst_opts("Substance options"),
			unit_opts("Unit options"),
			environ_opts("Environment options"),
			scatt_matrix_opts("Scattering matrix options"),
			scatt_provider_opts("Scattering method options"),
			cmdline("Command-line options"),
			config("Config options"), hidden("Hidden options"), oall("all options");

		unit_opts.add_options()
			("length-units", po::value<string>()->default_value("um"), "Units of length (um, mm, cm, m)")
			("volume-units", po::value<string>()->default_value("um^3"), "Units of volume (um^3, m^3, cm^3)")
			("frequency-units", po::value<string>()->default_value("GHz"), "Units of frequency (GHz, Hz)")
			("temperature-units", po::value<string>()->default_value("K"), "Units of temperature (K, degC)")
			("temp-units", po::value<string>()->default_value("K"), "Temperature units")
			;
		subst_opts.add_options()
			("refractive-index,r", po::value<vector<string> >()->multitoken(), "Refractive indices of target (may instead specify substance and temperature)")
			("volume,v", po::value<vector<string> >()->multitoken(), "Volumes")
			("radii,a", po::value<vector<string> >()->multitoken(), "Sphere radii")
			("diameters,d", po::value<vector<string> >()->multitoken(), "Sphere diameters")

			("substance,S", po::value<vector<string> >(), "Substances of interest. See icedb-refract for a list of pure substances. If not provided, you must specify refractive indices manually.")
			("temp,T", po::value<vector<string> >()->multitoken(), "Temperatures. Needed to calculate the refractive indices of certain substances.")
			;
		environ_opts.add_options()
			("wavelength,w", po::value<vector<string> >()->multitoken(), "Incident wavelengths")
			("frequency,f", po::value<vector<string> >()->multitoken(), "Incident frequencies")
			("environ-refractive-index", po::value<complex<double> >()->default_value(std::complex<double>(1,0)), 
				"Refractive index of the ambient environment")
			;
		scatt_matrix_opts.add_options()
			("angles", po::value<string>()->default_value("0:19:180:lin"), "Write the output scattering matrices at these scattering angles")
			;
		scatt_provider_opts.add_options()
			("list-scattering-methods", "Generate a list of implemented methods for calculating scattering by spheres.")
			("scattering-method,M", po::value<vector<string > >()->default_value(vector<string> ({"mie"}))->multitoken(),
				"Choose the scattering methods (mie, rayleigh, rayleigh-gans)")
			;
		cmdline.add_options()
			("help,h", "produce help message")
			("help-all", "produce verbose help for all possible options")
			("output-cross-sections,o", po::value<string>(), "Output file for cross sections. If no output file, then these are written to stdout.")
			("output-short", "Write terse output. Useful when writing to screen.")
			("output-scatt-matrices", po::value<string>(), "Path to an HDF5 file with scattering matrix results. If unspecified, then "
				"the scattering matrices will not be calculated.")
			("output-format", po::value<string>()->default_value("text"), "Output cross sections file format: text or HDF5. Must specify output-cross-sections for this to have any effect.")
			;
		config.add(unit_opts).add(subst_opts).add(environ_opts).add(scatt_matrix_opts).add(scatt_provider_opts);
		cmdline.add(config);

		desc.add(cmdline);
		oall.add(cmdline).add(hidden);

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).
			options(oall).run(), vm);
		po::notify(vm);

		auto doHelp = [&](const std::string &m) {
			cerr << desc << "\n" << m << endl; 
			exit(1); 
		};
		if (vm.count("help") || argc < 2) doHelp("");
		if (vm.count("help-all")) { cerr << oall << endl; exit(1); }

	}
	catch (std::exception &e) {
		cerr << "An exception has occurred: " << e.what() << endl;
		retval = 2;
	}
	return retval;
}
