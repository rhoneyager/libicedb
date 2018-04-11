#include "../icedb/defs.h"
#include "../icedb/formulas/requirements.hpp"

namespace icedb {
	namespace Formulas {
		requirement_p requirement_s::generate(
			const std::string &name, const std::string& units,
			double low, double high) {
			std::shared_ptr<requirement_s> res(new requirement_s);
			res->parameterName = name;
			res->parameterUnits = units;
			res->hasValidRange = true;
			res->validRange = std::pair<double, double>(low, high);
			return res;
		}

		provider_mp provider_s::generate(
			const std::string &name, const std::string &subst,
			const std::string &source, const std::string &notes,
			provider_s::spt sv, void* ptr) {
			provider_mp res(new provider_s);
			res->name = name;
			res->substance = subst;
			res->source = source;
			res->notes = notes;
			res->speciality_function_type = sv;
			res->specialty_pointer = ptr;
			return res;
		}
		provider_mp provider_s::addReq(const std::string &name, const std::string &units,
			double low, double high) {
			auto res = this->shared_from_this();
			auto newreq = Formulas::requirement_s::generate(name, units, low, high);
			reqs[name] = newreq;
			return res;
		}
		provider_mp provider_s::registerFunc(int priority) {
			provider_mp res = this->shared_from_this();
			all_providers_mp block;
			if (implementations::providersSet.count(substance))
				block = implementations::providersSet.at(substance);
			else {
				block = all_providers_mp(new provider_collection_type);
				implementations::providersSet[substance] = block;
			}
			block->insert(std::pair<int, provider_mp>(priority, res));
			implementations::allProvidersSet->insert(std::pair<int, provider_mp>(priority, res));
			implementations::providersByName[res->name] = res;
			implementations::substs.emplace(res->substance);
			return res;
		}
		provider_s::provider_s() {}
		provider_s::~provider_s() {}

	}
}