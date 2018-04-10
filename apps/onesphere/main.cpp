#include <icedb/defs.h>
/// This is a program that calculates scattering by individual spheres.
/// This program takes as many options as there are ways to specify target geometry and properties.
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <exception>
#include <iostream>
#include <vector>
#include <string>
#include <icedb/refract/refract.hpp>
#include <icedb/units/units.hpp>
#include <icedb/error/error.hpp>
#include <icedb/splitSet.hpp>

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

/*
 * Things I need to look at scattering by spheres:
 * refractive index of the target
 * volume of the target
 * wavelength of light
 * polarization of light
 *
 * options:
 * selection of angles
 * do I want cross sections
*/
namespace target {
	struct Geometry_base {
		double volume = -1; ///< Units of the incident light's wavelength, cubed.
		std::complex<double> refractiveIndex; ///< Imaginary component should be negative, by convention.
	};
	struct Has_orientation {
		bool isIsotropic = true;
		std::tuple<double, double, double> eulerRotationAngles = { 0, 0, 0 };
	};

	struct sphere_target : public Geometry_base {
	};
	struct simple_ellipsoid_target : public Geometry_base, public Has_orientation {
		double aspectRatio = 1.0;
	};
}

struct incident_wave_options {
	double wavelength = -1;
};
struct result_options {
	bool wantCrossSections = true; ///< \note if possible, the cross section calculations are done independently from the phase function calculations.
	bool wantPhaseFunctions = true;
	std::set<double> scatteredAngles = icedb::splitSet::splitSet<double>("0:30:180:cos");
};
struct result {
	target::Has_orientation orientation;
	incident_wave_options wave;
	std::array<std::complex<double >, 4> complexScatteringAmplitudeMatrix;
	std::array<double, 16> MuellerMatrix;
	std::array<double, 16> normalizedPhaseMatrix;
};

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
			//("volume-units", po::value<string>()->default_value("um^3"), "Units of volume (um^3, m^3, cm^3)")
			("frequency-units", po::value<string>()->default_value("GHz"), "Units of frequency (GHz, Hz)")
			("temperature-units", po::value<string>()->default_value("K"), "Units of temperature (K, degC)")
			;
		subst_opts.add_options()
			("refractive-index,r", po::value<vector<string> >(), "Refractive indices of target (may instead specify substance and temperature). Cannot separate by commas.")
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
			("angles", po::value<string>()->default_value("0:1:180"), "Write the output scattering matrices at these scattering angles")
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
		/// \todo check ("list-scattering-methods", "Generate a list of implemented methods for calculating scattering by spheres.")

		struct {
			struct {
				string length, frequency, temperature; // volume
				void populate(const po::variables_map& vm) {
					length = vm["length-units"].as<string>();
					//volume = vm["volume-units"].as<string>();
					frequency = vm["frequency-units"].as<string>();
					temperature = vm["temperature-units"].as<string>();
				}
			} units;
			struct {
				set<double> temperatures, radii, diameters, volumes;
				set<string> substances;
				set<complex<double>> refractiveIndices;
				void populate(const po::variables_map& vm) {
					vector<string> vsRefrs, vsVolumes, vsRadii, vsDiameters, vsSubstances, vsTemps;
					if (vm.count("refractive-index"))
						vsRefrs = vm["refractive-index"].as<vector<string>>();
					if (vm.count("volume"))
						vsVolumes = vm["volume"].as<vector<string>>();
					if (vm.count("radii"))
						vsRadii = vm["radii"].as<vector<string>>();
					if (vm.count("diameters"))
						vsDiameters = vm["diameters"].as<vector<string>>();
					if (vm.count("substance"))
						vsSubstances = vm["substance"].as<vector<string>>();
					if (vm.count("temp"))
						vsTemps = vm["temp"].as<vector<string>>();

					for (const auto &R : vsRefrs) icedb::splitSet::splitSet<complex<double> >(R, refractiveIndices);
					for (const auto &v : vsVolumes) icedb::splitSet::splitSet<double>(v, volumes);
					for (const auto &r : vsRadii) icedb::splitSet::splitSet<double>(r, radii);
					for (const auto &d : vsDiameters) icedb::splitSet::splitSet<double>(d, diameters);
					for (const auto &s : vsSubstances) icedb::splitSet::splitSet<string>(s, substances);
					for (const auto &t : vsTemps) icedb::splitSet::splitSet<double>(t, temperatures);
				}
			} targets;
			struct {
				set<double> wavelengths;
				complex<double> environmentRefractiveIndex = complex<double>(1, 0);
				void populate(const po::variables_map& vm, const string &freqUnits, const string &wvlenUnits) {
					vector<string> vsWavelengths, vsFrequencies;
					if (vm.count("wavelength")) vsWavelengths = vm["wavelength"].as<vector<string>>();
					if (vm.count("frequency")) vsFrequencies = vm["frequency"].as<vector<string>>();
					for (const auto &w : vsWavelengths) icedb::splitSet::splitSet<double>(w, wavelengths);
					if (vsFrequencies.size()) {
						auto specConverter = icedb::units::conv_spec::generate(freqUnits, wvlenUnits);
						if (!specConverter->isValid())
							ICEDB_throw(icedb::error::error_types::xAssert)
							.add("Description", "Cannot convert user-specified frequencies to wavelengths with the specified units.")
							.add("frequency-units", freqUnits)
							.add("wavelength-units", wvlenUnits);
						set<double> frequencies;
						for (const auto &sf : vsFrequencies)
							icedb::splitSet::splitSet<double>(sf, frequencies);
						for (const auto &f : frequencies)
							wavelengths.emplace(specConverter->convert(f));
					}
					environmentRefractiveIndex = vm["environ-refractive-index"].as<complex<double> >();
				}
			} environment;
			struct {
				set<double> angles;
				void populate(const po::variables_map& vm) {
					icedb::splitSet::splitSet<double>(vm["angles"].as<string>(), angles);
				}
			} scatt_m;
			struct {
				set<string> methods;
				void populate(const po::variables_map& vm) {
					vector<string> vsmethods;
					if (vm.count("scattering-method")) vsmethods = vm["scattering-method"].as<vector<string> >();
					for (const auto &s : vsmethods)
						icedb::splitSet::splitSet(s, methods);
				}
			} providers;
			void populate(const po::variables_map& vm) {
				units.populate(vm);
				targets.populate(vm);
				environment.populate(vm, units.frequency, units.length);
				scatt_m.populate(vm);
				providers.populate(vm);
			}
		} user;

		
		user.populate(vm);
	}
	catch (std::exception &e) {
		cerr << "An exception has occurred: " << e.what() << endl;
		retval = 2;
	}
	return retval;
}
