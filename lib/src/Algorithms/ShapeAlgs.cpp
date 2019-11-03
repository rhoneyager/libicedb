#include "icedb/defs.h"
#include <mutex>
#include "icedb/Algorithms/ShapeAlgs.hpp"
#include "icedb/IO/Shapes.hpp"
#include "BetterThrow/Error.hpp"
#include "icedb/Algorithms/Algorithms.hpp"

namespace icedb {
	namespace ShapeAlgs {
		ICEDB_DL std::set<std::string>
			PPP_required_datasets,
			PPP_required_attributes{ "particle_maximum_dimension_standard" };

		namespace Algorithms {
			bool Algorithm::isValidTarget(const HH::Group& shape, const gsl::span<const HH::Group>& inPPPs) const
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

			const std::map<std::string, Algorithm>& get_common_algorithms()
			{
				static std::map<std::string, Algorithm> res
				{
						{"copy_to_ppp", _internal_algs::copy_to_ppp},
						{"dummy", _internal_algs::dummy},
						{"dummy2", _internal_algs::dummy2},
						{"ConvexHull", _internal_algs::ConvexHull},
						{"MaxDistanceTwoPoints", _internal_algs::MaxDistanceTwoPoints},
						{"SmallestCircumscribingSphere_Points", _internal_algs::SmallestCircumscribingSphere_Points}
				};
				return res;
			}

			/*
			std::map<std::string, Algorithm> common_algorithms
			{
#ifdef ICEDB_OS_MACOS
# warning "common_algorithms on AppleClang on macOS is buggy due to compiler problems. TODO: Improve compiler detection / use CMake for this."
#endif
//#else
				{"copy_to_ppp", _internal_algs::copy_to_ppp},
				{"dummy", _internal_algs::dummy},
				{"dummy2", _internal_algs::dummy2},
				{"ConvexHull", _internal_algs::ConvexHull},
				{"MaxDistanceTwoPoints", _internal_algs::MaxDistanceTwoPoints},
				{"SmallestCircumscribingSphere_Points", _internal_algs::SmallestCircumscribingSphere_Points}
//#endif
			};
			*/
		}
	}
}
