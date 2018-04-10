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
	}
}