#pragma once
#include "defs.h"
#include <functional>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <HH/Groups.hpp>
#include <gsl/gsl>

namespace icedb {
	namespace ShapeAlgs {
		/** \brief The better-known shape algorithms are defined in this namespace.
		**/
		namespace Algorithms {
			using std::set;
			using std::string;
			using std::vector;
			using std::function;

			class Algorithm
			{
				set<string> _rsd, _rsa, _rpd, _rpa, _pd, _pa,
					_dsd, _dsa, _dpd, _dpa;
				typedef function<void(HH::Group, const HH::Group&, gsl::span<const HH::Group>)> func_type;
				func_type _f;
				int _weight = 100;
			public:
				struct AlgorithmConstructor
				{
					int weight = 100;
					set<string> RequiredStructuralDatasets,
						RequiredStructuralAttributes,
						RequiredPPPDatasets,
						RequiredPPPAttributes,
						ProvidedDatasets,
						ProvidedAttributes,
						DisallowedStructuralDatasets,
						DisallowedStructuralAttributes,
						DisallowedPPPDatasets,
						DisallowedPPPAttributes;
					func_type func;
					AlgorithmConstructor() {}
					AlgorithmConstructor(
						int weight,
						const set<string>& rsd, const set<string>& rsa,
						const set<string>& rpd, const set<string>& rpa,
						const set<string>& pd, const set<string>& pa,
						const set<string>& dsd, const set<string>& dsa,
						const set<string>& dpd, const set<string>& dpa,
						func_type f)
						: weight(weight), RequiredStructuralDatasets(rsd), RequiredStructuralAttributes(rsa),
						RequiredPPPDatasets(rpd), RequiredPPPAttributes(rpa),
						ProvidedDatasets(pd), ProvidedAttributes(pa),
						DisallowedStructuralDatasets(dsd), DisallowedStructuralAttributes(dsa),
						DisallowedPPPDatasets(dpd), DisallowedPPPAttributes(dpa), func(f)
					{}
				};
				Algorithm() {}
				Algorithm(AlgorithmConstructor a)
					: _weight(a.weight), _rsd(a.RequiredStructuralDatasets), _rsa(a.RequiredStructuralAttributes),
					_rpd(a.RequiredPPPDatasets), _rpa(a.RequiredPPPAttributes),
					_pd(a.ProvidedDatasets), _pa(a.ProvidedAttributes),
					_dsd(a.DisallowedStructuralDatasets), _dsa(a.DisallowedStructuralAttributes),
					_dpd(a.DisallowedPPPDatasets), _dpa(a.DisallowedPPPAttributes), _f(a.func) {}
				~Algorithm() {}

				const set<string>& RequiredStructuralDatasets() const { return _rsd; }
				const set<string>& RequiredStructuralAttributes() const {return _rsa; }
				const set<string>& RequiredPPPDatasets() const { return _rpd; }
				const set<string>& RequiredPPPAttributes() const { return _rpa; }
				const set<string>& ProvidedDatasets() const { return _pd; }
				const set<string>& ProvidedAttributes() const { return _pa; }
				const set<string>& DisallowedStructuralDatasets() const { return _dsd; }
				const set<string>& DisallowedStructuralAttributes() const { return _dsa; }
				const set<string>& DisallowedPPPDatasets() const { return _dpd; }
				const set<string>& DisallowedPPPAttributes() const { return _dpa; }

				int weight() const { return _weight; }
				bool isValidTarget(const HH::Group& shape = {}, gsl::span<const HH::Group> inPPPs = {}) const;
				void apply(HH::Group out,
					const HH::Group &shape = {}, gsl::span<const HH::Group> inPPPs = {}) const
				{
					return _f(out, shape, inPPPs);
				}
			};

			/// These algorithms are searched by default when we want to calculate new properties.
			ICEDB_DL extern std::map<std::string, Algorithm> common_algorithms;
		}
		/// These are the "required" datasets and attributes in a PPP group.
		ICEDB_DL extern std::set<std::string> PPP_required_datasets, PPP_required_attributes;
	}
}
