#pragma once
#include "../defs.h"
#include <map>
#include <set>
#include <functional>
#include <vector>

namespace icedb {
	namespace conversions {
		enum class ParamName {
			EffectiveRadius,
			Volume,
			ScaledVolume,
			AspectRatio,
			VolumeFraction,
			EquivalentRadius,
			MajorAxis,
			MinorAxis,
			Mass,
			SolidDensity,
			BulkScaledDensity
		};
		typedef std::map<ParamName, double> ParamSet_t;


		class Algorithm {
		protected:
			std::vector<std::set<ParamName> > _neededParams;
			std::set<ParamName> _providesParams;
		public:
			~Algorithm() {}
			typedef std::function<void(ParamSet_t&)> Invoke_Func_t;
			Invoke_Func_t invoke = nullptr;
			Algorithm(Invoke_Func_t invokeFunc,
				std::initializer_list<std::initializer_list<ParamName > > neededParams,
				std::initializer_list<ParamName> providesParams)
				: _providesParams(providesParams), invoke(invokeFunc)
			{
				// Unpack the set
				for (auto &p : neededParams) {
					_neededParams.push_back(p);
				}
			}
			bool canInvoke(const ParamSet_t &params) {
				// Iterate over each combination of parameters needed for the algorithm to function
				for (const auto &n : _neededParams)
				{
					bool good = true;
					for (const auto &p : n) {
						if (!params.count(p)) good = false;
						if (params.at(p) < 0) good = false;
					}
					if (good) return true;
				}
				return false;
			}
			static Algorithm generate(
				Invoke_Func_t alg,
				std::initializer_list<std::initializer_list<ParamName > > neededParams,
				std::initializer_list<ParamName> providesParams)
			{
				return Algorithm{ alg, neededParams, providesParams };
			}
		};

		struct Ellipsoid_Regular {
			
			ParamSet_t _params;

			virtual ~Ellipsoid_Regular() {}

			void genAlgs() {
				auto alg_EffRadToScaledVolume = Algorithm::generate(
					[](ParamSet_t &params) { constexpr double pi = 3.14159265358979; params[ParamName::ScaledVolume] = (4. / 3.) * pi * std::pow(params[ParamName::EffectiveRadius], 3.); },
					{ {ParamName::EffectiveRadius} },
					{ ParamName::ScaledVolume }
				);
				auto alg_ScaledVolumeToEffRad = Algorithm::generate(
					[](ParamSet_t &params) { constexpr double pi = 3.14159265358979; params[ParamName::ScaledVolume] = std::pow(3. * params[ParamName::ScaledVolume] / (4. * pi), 1. / 3.); },
					{ {ParamName::ScaledVolume} },
					{ ParamName::EffectiveRadius }
				);
				auto alg_ScaledVolumeVfToVolume = Algorithm::generate(
					[](ParamSet_t &params) { params[ParamName::Volume] = params[ParamName::ScaledVolume] / params[ParamName::VolumeFraction]; },
					{ {ParamName::ScaledVolume, ParamName::VolumeFraction} },
					{ ParamName::Volume }
				);
				auto alg_VolumeVfToScaledVolume = Algorithm::generate(
					[](ParamSet_t &params) { params[ParamName::ScaledVolume] = params[ParamName::Volume] * params[ParamName::VolumeFraction]; },
					{ {ParamName::Volume, ParamName::VolumeFraction} },
					{ ParamName::ScaledVolume }
				);
				auto alg_VolumeScaledVolumeToVf = Algorithm::generate(
					[](ParamSet_t &params) { params[ParamName::VolumeFraction] = params[ParamName::ScaledVolume] / params[ParamName::Volume]; },
					{ {ParamName::Volume, ParamName::ScaledVolume} },
					{ ParamName::VolumeFraction }
				);
			}
		};
	}
}
