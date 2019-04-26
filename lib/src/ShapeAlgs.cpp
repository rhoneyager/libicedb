#include "icedb/ShapeAlgs.hpp"

namespace icedb {
	namespace ShapeAlgs {
		ICEDB_DL std::set<std::string>
			PPP_required_datasets,
			PPP_required_attributes{ "particle_maximum_dimension_standard" };

		namespace Algorithms {
			bool Algorithm::isValidTarget(const HH::Group& shape, gsl::span<const HH::Group> inPPPs) const
			{
				if (_rsd.size() && !shape.isGroup()) return false;
				if (_rsa.size() && !shape.isGroup()) return false;
				if (_dsd.size() && !shape.isGroup()) return false;
				if (_dsa.size() && !shape.isGroup()) return false;
				if (_rpa.size() && !inPPPs.size()) return false;
				if (_rpd.size() && !inPPPs.size()) return false;
				if (_dpa.size() && !inPPPs.size()) return false;
				if (_dpd.size() && !inPPPs.size()) return false;

				for (const auto& a : _rsa)
					if (!shape.atts.exists(a)) return false;
				for (const auto& d : _rsd)
					if (!shape.dsets.exists(d)) return false;
				for (const auto& a : _dsa)
					if (shape.atts.exists(a)) return false;
				for (const auto& d : _dsd)
					if (shape.dsets.exists(d)) return false;
				for (const auto& a : _rpa) {
					bool good = false;
					for (const auto& p : inPPPs) {
						if (p.atts.exists(a)) {
							good = true;
							continue;
						}
					}
					if (!good) return false;
				}
				for (const auto& d : _rpd) {
					bool good = false;
					for (const auto& p : inPPPs) {
						if (p.dsets.exists(d)) {
							good = true;
							continue;
						}
					}
					if (!good) return false;
				}
				for (const auto& a : _dpa)
					for (const auto& p : inPPPs)
						if (p.atts.exists(a)) return false;
				for (const auto& d : _dpd)
					for (const auto& p : inPPPs)
						if (p.dsets.exists(d)) return false;

				return true;
			}

			// Define a common algorithm that sets a dummy property.
			// No inputs needed.
			Algorithm dummy{ []()->Algorithm::AlgorithmConstructor {
				Algorithm::AlgorithmConstructor a;
				a.ProvidedAttributes = { "dummyVer" };
				a.ProvidedDatasets = { "dummyDset" };
				a.func = [](HH::Group res, const HH::Group & shp, gsl::span<const HH::Group> inPPPs)
				{
					res.atts.add<uint16_t>("dummyVer", 1);
					res.dsets.create<int32_t>("dummyDset", { 2 })
						.write<int32_t>({ 3,4 });
				};
				a.weight = 100;
				return a;
}() };

			std::map<std::string, Algorithm> common_algorithms
			{ {"dummy", dummy} };

		}
	}
}
