#include "icedb/defs.h"
#pragma warning( disable : 4251 ) // DLL interface
#include <string>
#include "icedb/units/units.hpp"
#include "private/unitsBackend.hpp"
#include "icedb/units/unitsPlugins.hpp"
#include "icedb/Utils/options.hpp"
#include "icedb/Errors/error.hpp"
#include "icedb/Utils/logging.hpp"

namespace icedb {
	namespace units {
		converter::converter() { implementations::_init(); }

		std::shared_ptr<const converter> converter::generate(
			const std::string &inUnits, const std::string &outUnits) {
			converter_p res;
			res = implementations::_queryBackend(inUnits, outUnits, "");
			if (!res) {
				res = converter_p(new converter(inUnits, outUnits));
				implementations::_registerBackend(inUnits, outUnits, "", res);
			}
			return res;
		}
		converter::converter(const std::string &inUnits, const std::string &outUnits)
		{
			h = getConverter(inUnits, outUnits);
		}

		bool converter::canConvert(const std::string &inUnits, const std::string &outUnits) {
			implementations::_init(); // Static function that registers the builtin unit converters.
			//SDBR_log("units", scatdb::logging::ICEDB_LOG_NORMAL,
			//	"Creating converter for " << inUnits << " to " << outUnits);
			auto hooks = implementations::getHooks();
			//	hull_provider_registry, hull_provider<convexHull> >::getHooks();
			//std::cerr << hooks->size() << std::endl;
			auto opts = registry::options::generate();
			opts->setVal<std::string>("inUnits", inUnits);
			opts->setVal<std::string>("outUnits", outUnits);
			for (const auto &i : *(hooks.get()))
			{
				if (!i->canConvert) continue;
				if (!i->constructConverter) continue;
				if (!i->canConvert(opts)) continue;
				return true;
			}
			return false;
		}

		bool converter::isValid() const {
			if (h) return h->isValid();
			return false;
		}
		double converter::convert(double in) const {
			if (!h)
				ICEDB_throw(icedb::error::error_types::xNullPointer)
					.add<std::string>("Reason", "Converter handler is null. Probably incompatible units.");
			if (!isValid())
				ICEDB_throw(icedb::error::error_types::xBadInput)
				.add<std::string>("Reason", "Conversion between the two specified units is invalid.");
			return h->convert(in);
		}
		converter::~converter() {}

		/** This works by providing a custom converter **/
		conv_spec::conv_spec(const std::string &in, const std::string &out)
			: converter()
		{
			implementations::_init(); // Static function that registers the builtin unit converters.
			ICEDB_log("units", icedb::logging::ICEDB_LOG_DEBUG_2,
				"Creating spectrum converter for " << in << " to " << out);

			auto opts = registry::options::generate();
			opts->setVal<std::string>("inUnits", in);
			opts->setVal<std::string>("outUnits", out);
			h = implementations::spectralUnits::constructConverter(opts);

		}
		std::shared_ptr<const converter> conv_spec::generate(
			const std::string &inUnits, const std::string &outUnits) {
			converter_p res;
			res = implementations::_queryBackend(inUnits, outUnits, "spec");
			if (!res) {
				res = converter_p(new conv_spec(inUnits, outUnits));
				implementations::_registerBackend(inUnits, outUnits, "spec", res);
			}
			return res;
		}
	}
}

