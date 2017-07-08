#pragma once
#ifndef ICEDB_HPP_UNITS
#define ICEDB_HPP_UNITS
#include "defs.h"
#include "units.h"
#include <memory>
ICEDB_BEGIN_DECL_CPP

namespace icedb {
	namespace units {
		class DL_ICEDB converter {
		private:
			typedef std::shared_ptr<ICEDB_UNIT_CONVERTER> _unit_converter_p;
			_unit_converter_p p;
			converter();
		public:
			typedef std::shared_ptr<converter> converter_p;
			~converter();
			bool isValid() const;
			double convert(double val) const;
			static converter_p generate(const char* type, const char* inUnits, const char* outUnits);
		};
	}
}

ICEDB_END_DECL_CPP
#endif
