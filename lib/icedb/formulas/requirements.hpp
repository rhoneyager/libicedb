#pragma once
#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>
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
			
			void* specialty_pointer;
			std::type_index func_type;
			bool isA(const std::type_index &) const;
			
			template <typename T>
			T asA() const;

			static provider_mp generate(
				const std::string &name, const std::string &subst,
				const std::string &source, const std::string &notes,
				void* ptr_to_func, const std::type_index &func_type);
		};
		typedef std::multimap<int, provider_p> provider_collection_type;
		typedef std::shared_ptr<const provider_collection_type > all_providers_p;
		typedef std::shared_ptr<provider_collection_type > all_providers_mp;

		template <typename t>
		struct provider_typed : public provider_s
		{
			virtual ~provider_typed();
			void get() const;
			static provider_mp generate(
				const std::string &name, const std::string &subst,
				const std::string &source, const std::string &notes,
				void* ptr_to_func, const std::type_index &func_params);
		};
	}
}
