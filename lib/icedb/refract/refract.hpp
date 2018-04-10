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
		
		struct provider_s;
		typedef std::shared_ptr<provider_s> provider_mp;
		typedef std::shared_ptr<const provider_s> provider_p;
		struct provider_s : public std::enable_shared_from_this<provider_s> {
		private:
			provider_s();
		public:
			virtual ~provider_s();
			std::string name;
			std::string substance;
			std::string source;
			std::string notes;
			std::map<std::string, Formulas::requirement_p> reqs;
			provider_mp addReq(const std::string &name, const std::string &units,
				double low, double high);
			provider_mp registerFunc(int priority = 0);
			enum class spt {
				NONE,
				FREQ,
				FREQTEMP,
				OTHER
			} speciality_function_type;
			void* specialty_pointer;
			static provider_mp generate(
				const std::string &name, const std::string &subst,
				const std::string &source, const std::string &notes,
				provider_s::spt sv, void* ptr);
		};
		typedef std::multimap<int, provider_p> provider_collection_type;
		typedef std::shared_ptr<const provider_collection_type > all_providers_p;
		typedef std::shared_ptr<provider_collection_type > all_providers_mp;

		typedef std::function<void(double, std::complex<double>&)> refractFunction_freqonly_t;
		typedef std::function<void(double, double, std::complex<double>&)> refractFunction_freq_temp_t;

		all_providers_p listAllProviders();
		all_providers_p listAllProviders(const std::string &subst);
		void enumProvider(provider_p p, std::ostream &out = std::cerr);
		void enumProviders(all_providers_p p, std::ostream &out = std::cerr);
		void enumSubstances(std::ostream &out = std::cerr);

		provider_p findProviderByName(const std::string &providerName);

		provider_p findProvider(const std::string &subst,
			bool haveFreq = true, bool haveTemp = true, const std::string &startAt = "");
		all_providers_p findProviders(const std::string &subst,
			bool haveFreq = true, bool haveTemp = true);

		void prepRefract(provider_p, const std::string &inFreqUnits,
			refractFunction_freqonly_t&);
		void prepRefract(provider_p,
			const std::string &inFreqUnits, const std::string &inTempUnits,
			refractFunction_freq_temp_t&);
		

	}
}
