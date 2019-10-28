#pragma once
#include "../defs.h"
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "../Utils/optionsForwards.hpp"
namespace icedb {
	namespace units {
		class converter;
		namespace implementations {
			struct Unithandler;
			class converter_provider_registry {};
			class Converter_registry_provider
			{
			public:
				Converter_registry_provider();
				~Converter_registry_provider();
				typedef registry::const_options_ptr optsType;
				typedef std::function<bool(optsType)> canConvertType;
				canConvertType canConvert;
				typedef std::function<std::shared_ptr<const Unithandler>(optsType)> constructType;
				constructType constructConverter;
				const char* name;
			};
			typedef std::shared_ptr<const Converter_registry_provider> conv_prov_cp;
			typedef std::shared_ptr<const std::vector<conv_prov_cp> > conv_hooks_t;
			conv_hooks_t getHooks();
			/// Opaque object provided to perform unit manipulations.
			struct Unithandler
			{
			protected:
				const char* id;
				Unithandler(const char* id);
			public:
				virtual ~Unithandler() {}
				virtual double convert(double input) const = 0;
				virtual bool isValid() const = 0;
			};
		}
	}
}



