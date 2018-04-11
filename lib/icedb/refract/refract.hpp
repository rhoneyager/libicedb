#pragma once
#include "../defs.h"

#include <complex>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "../units/units.hpp"
#include "refractBase.hpp"
#include "../optionsForwards.hpp"
#include "../formulas/requirements.hpp"
namespace icedb {
	namespace refract {
		
		
		typedef std::function<void(double, std::complex<double>&)> refractFunction_freqonly_t;
		typedef std::function<void(double, double, std::complex<double>&)> refractFunction_freq_temp_t;

		Formulas::all_providers_p listAllProviders();
		Formulas::all_providers_p listAllProviders(const std::string &subst);
		void enumProvider(Formulas::provider_p p, std::ostream &out = std::cerr);
		void enumProviders(Formulas::all_providers_p p, std::ostream &out = std::cerr);
		void enumSubstances(std::ostream &out = std::cerr);

		Formulas::provider_p findProviderByName(const std::string &providerName);

		Formulas::provider_p findProvider(const std::string &subst,
			bool haveFreq = true, bool haveTemp = true, const std::string &startAt = "");
		Formulas::all_providers_p findProviders(const std::string &subst,
			bool haveFreq = true, bool haveTemp = true);

		void prepRefract(Formulas::provider_p, const std::string &inFreqUnits,
			refractFunction_freqonly_t&);
		void prepRefract(Formulas::provider_p,
			const std::string &inFreqUnits, const std::string &inTempUnits,
			refractFunction_freq_temp_t&);
		

	}
}
