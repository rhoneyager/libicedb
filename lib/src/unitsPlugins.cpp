#include <string>
#include <map>
#include <mutex>
#include <vector>
#include "../icedb/units/units.hpp"
#include "../icedb/units/unitsPlugins.hpp"
#include "../private/unitsBackend.hpp"
#include "../icedb/logging.hpp"
#include "../icedb/error/error.hpp"
#include "../private/options.hpp"

namespace icedb {
	namespace units {

		namespace implementations {
			implementations::Unithandler::Unithandler(const char* id) : id(id) {}
			Converter_registry_provider::Converter_registry_provider() {}
			Converter_registry_provider::~Converter_registry_provider() {}

			std::mutex m_backends;
			std::map<std::string, converter_p> _backends;
			std::shared_ptr<std::vector<conv_prov_cp > > _providers;

			inline std::string _mangle_string(const std::string &inUnits, const std::string &outUnits,
				const std::string &family) {
				std::string res(family);
				res.append("___");
				res.append(inUnits);
				res.append("___");
				res.append(outUnits);
				return res;
			}

			void _registerBackend(const std::string &inUnits, const std::string &outUnits,
				const std::string &family, converter_p p) {
				std::lock_guard<std::mutex> mlock(m_backends);
				std::string h = _mangle_string(inUnits, outUnits, family);
				_backends.emplace(std::pair<std::string, converter_p>(h, p));
			}
			converter_p _queryBackend(const std::string &inUnits, const std::string &outUnits,
				const std::string &family) {
				std::string h = _mangle_string(inUnits, outUnits, family);
				std::lock_guard<std::mutex> mlock(m_backends);
				converter_p res;
				if (_backends.count(h)) res = _backends.at(h);
				return res;
			}

			void _init() {
				static bool inited = false;
				if (inited) return;
				inited = true;
				//template <class base, class reg, class obj>
				//	void doRegisterHook(const obj& res)
				std::shared_ptr<implementations::Converter_registry_provider> res(
					new implementations::Converter_registry_provider);
				res->canConvert = simpleUnits::canConvert;
				res->constructConverter = simpleUnits::constructConverter;
				static const char* name = "1simple";
				res->name = name;
				_providers = std::shared_ptr<std::vector<conv_prov_cp > >(new std::vector<conv_prov_cp >);
				_providers->push_back(res);
			}
			conv_hooks_t getHooks() {
				return _providers;
			}
		}

		std::shared_ptr<const implementations::Unithandler> converter::getConverter(
			const std::string &inUnits, const std::string &outUnits) {
			implementations::_init(); // Static function that registers the builtin unit converters.

			const auto &hooks = implementations::getHooks();
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
				return i->constructConverter(opts);
			}
			// Only return nullptr if unable to find a usable hook.
			ICEDB_log("units", icedb::logging::ICEDB_LOG_DEBUG_1, "No handler for unit conversion found for "
				"conversions between " << inUnits << " and " << outUnits << ".");
			// We don;t want to throw an error here. Only throw if a conversion is actually attempted.
			// Otherwise, we quash the purpose of isValid().
			//ICEDB_throw(icedb::error::error_types::xBadInput)
			//	.add<std::string>("Reason", "There is no code which can handle this unit conversion.")
			//	.add<std::string>("inUnits", inUnits)
			//	.add<std::string>("outUnits", outUnits)
			//	;
			return nullptr;
		}


	}
}

