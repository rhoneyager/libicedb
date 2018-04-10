#pragma once
#include <memory>
#include <string>
#include <utility>

namespace icedb {
	namespace Formulas {
		struct requirement_s;
		typedef std::shared_ptr<const requirement_s> requirement_p;
		struct requirement_s {
			std::string parameterName;
			std::string parameterUnits;
			bool hasValidRange;
			std::pair<double, double> validRange;
			static requirement_p generate(
				const std::string &name, const std::string& units,
				double low, double high);
		};
	}
}
