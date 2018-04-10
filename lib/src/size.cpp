#include "../icedb/defs.h"
#include "../icedb/conversions/size.hpp"
#include "../icedb/conversions/density.hpp"
#include "../icedb/error/error.hpp"
#include <boost/math/constants/constants.hpp>
#include <mutex>

namespace icedb {
	namespace conversions {
		bool Algorithm::canInvoke(const ParamSet_t &params) const
		{
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

		void Generic_Object::clearParams() {
			params.clear();
		}

		Generic_Object::Generic_Object() {
			generateGenericAlgs();
			_objAlgs = _genericObjAlgs;
		}

		Ellipsoid_Regular::Ellipsoid_Regular()
			: Generic_Object()
		{
			generateEllipsoidAlgs();
			for (const auto &a : _EllipsoidAlgs)
				_objAlgs.push_back(a);
		}

		bool Generic_Object::hasValidParam(const ParamSet_t &params, ParamName name)
		{
			if (!params.count(name)) return false;
			if (params.at(name) < 0) return false;
			return true;
		}

		void Generic_Object::generateGenericAlgs() {
			static bool generated = false;
			static std::mutex mtxGen;
			std::lock_guard<std::mutex> lck(mtxGen);
			if (generated) return;

			using namespace Parameters::Base;
			// Logic goes here

			// SolidSphereRadius, SolidVolume
			{
				_genericObjAlgs.push_back(
					Algorithm::generate(
						[](ParamSet_t &p) {
					constexpr double pi = boost::math::constants::pi<double>();
					p[SolidVolume] = (4. / 3.) * pi * std::pow(p[SolidSphereRadius], 3.);
				},
				{ { SolidSphereRadius } },
				{ SolidVolume }
				));

				_genericObjAlgs.push_back(
					Algorithm::generate(
						[](ParamSet_t &p) {
					constexpr double pi = boost::math::constants::pi<double>();
					p[SolidSphereRadius] = std::pow(3. * p[SolidVolume] / (4. * pi), 1. / 3.);
				},
				{ { SolidVolume } },
				{ SolidSphereRadius }
				));
			}

			// MixedEquivalentRadius, MixedVolume
			{
				_genericObjAlgs.push_back(
					Algorithm::generate(
						[](ParamSet_t &p) {
					constexpr double pi = boost::math::constants::pi<double>();
					p[MixedVolume] = (4. / 3.) * pi * std::pow(p[MixedEquivalentRadius], 3.);
				},
				{ { MixedEquivalentRadius } },
				{ MixedVolume }
				));

				_genericObjAlgs.push_back(
					Algorithm::generate(
						[](ParamSet_t &p) {
					constexpr double pi = boost::math::constants::pi<double>();
					p[MixedEquivalentRadius] = std::pow(3. * p[MixedVolume] / (4. * pi), 1. / 3.);
				},
				{ { MixedVolume } },
				{ MixedEquivalentRadius }
				));
			}

			// SolidVolume, SolidDensity, Mass
			{
				_genericObjAlgs.push_back(
					Algorithm::generate(
						[](ParamSet_t &p) {
					auto &valid = hasValidParam;
					if (!valid(p, Mass) && valid(p, SolidDensity) && valid(p, SolidVolume))
						p[Mass] = p[SolidDensity] * p[SolidVolume];
					else if (!valid(p, SolidVolume) && valid(p, SolidDensity) && valid(p, Mass))
						p[SolidVolume] = p[Mass] / p[SolidDensity];
					else if (!valid(p, SolidDensity) && valid(p, SolidVolume) && valid(p, Mass))
						p[SolidDensity] = p[Mass] / p[SolidVolume];
				},
				{ { SolidVolume, SolidDensity },{ SolidVolume, Mass },{ Mass, SolidDensity } },
				{ SolidVolume, SolidDensity, Mass }
				));
			}

			// SolidVolume, MixedVolume, MixedVolumeFraction
			{
				_genericObjAlgs.push_back(
					Algorithm::generate(
						[](ParamSet_t &p) {
					auto &valid = hasValidParam;
					if (!valid(p, MixedVolumeFraction) && valid(p, SolidVolume) && valid(p, MixedVolume))
						p[MixedVolumeFraction] = p[SolidVolume] / p[MixedVolume];
					else if (!valid(p, MixedVolume) && valid(p, SolidVolume) && valid(p, MixedVolumeFraction))
						p[MixedVolume] = p[SolidVolume] / p[MixedVolumeFraction];
					else if (!valid(p, SolidVolume) && valid(p, MixedVolume) && valid(p, MixedVolumeFraction))
						p[SolidVolume] = p[MixedVolume] * p[MixedVolumeFraction];
				},
				{ { SolidVolume, SolidDensity },{ SolidVolume, Mass },{ Mass, SolidDensity } },
				{ SolidVolume, SolidDensity, Mass }
				));
			}

			// SolidDensity, MixedDensity, MixedVolumeFraction
			{
				_genericObjAlgs.push_back(
					Algorithm::generate(
						[](ParamSet_t &p) {
					auto &valid = hasValidParam;
					if (!valid(p, MixedVolumeFraction) && valid(p, SolidDensity) && valid(p, MixedDensity))
						p[MixedVolumeFraction] = p[SolidDensity] / p[MixedDensity];
					else if (!valid(p, MixedDensity) && valid(p, SolidDensity) && valid(p, MixedVolumeFraction))
						p[MixedDensity] = p[SolidDensity] / p[MixedVolumeFraction];
					else if (!valid(p, SolidDensity) && valid(p, MixedDensity) && valid(p, MixedVolumeFraction))
						p[SolidDensity] = p[MixedDensity] * p[MixedVolumeFraction];
				},
				{ { SolidDensity, MixedDensity },{ SolidDensity, MixedVolumeFraction },{ MixedDensity, MixedVolumeFraction } },
				{ SolidDensity, MixedDensity, MixedVolumeFraction }
				));
			}

			generated = true;
		}

		void Ellipsoid_Regular::generateEllipsoidAlgs() {
			ICEDB_throw(error::error_types::xUnimplementedFunction);
		}
		/*
		void Ellipsoid_Regular::generateEllipsoidAlgs() {
			static bool generated = false;
			static std::mutex mtxGen;
			std::lock_guard<std::mutex> lck(mtxGen);
			if (generated) return;

			// Logic goes here

			// SolidVolume, AspectRatio, SolidSemiPolarAxis, SolidSemiEquatorialAxis
			{
				_genericObjAlgs.push_back(
					Algorithm::generate(
						[](ParamSet_t &params) {
					if (params[ParamName::SolidVolume] > 0 && params[ParamName::AspectRatio]) {
						// AR < 1 is oblate, AR > 1 is prolate

					}


					if (params[ParamName::MixedVolumeFraction] < 0 && params[ParamName::SolidDensity] >= 0 && params[ParamName::MixedDensity] >= 0)
						params[ParamName::MixedVolumeFraction] = params[ParamName::SolidDensity] / params[ParamName::MixedDensity];
					else if (params[ParamName::MixedDensity] < 0 && params[ParamName::SolidDensity] >= 0 && params[ParamName::MixedVolumeFraction] >= 0)
						params[ParamName::MixedDensity] = params[ParamName::SolidDensity] / params[ParamName::MixedVolumeFraction];
					else if (params[ParamName::SolidDensity] < 0 && params[ParamName::MixedDensity] >= 0 && params[ParamName::MixedVolumeFraction] >= 0)
						params[ParamName::SolidDensity] = params[ParamName::MixedDensity] * params[ParamName::MixedVolumeFraction];
				},
				{ { ParamName::SolidDensity, ParamName::MixedDensity },{ ParamName::SolidDensity, ParamName::MixedVolumeFraction },{ ParamName::MixedDensity, ParamName::MixedVolumeFraction } },
				{ ParamName::SolidVolume, ParamName::AspectRatio, ParamName::SolidSemiPolarAxis, ParamName::SolidSemiEquatorialAxis }
				));
			}

			// MixedVolume, AspectRatio, MixedSemiPolarAxis


			// SolidSemiPolarAxis, SolidSemiEquatorialAxis, SolidVolume

			// MixedSemiPolarAxis, MixedSemiEquatorialAxis, MixedVolume

			




			generated = true;
		}
		*/

		double Generic_Object::findParam(ParamName param) {
			throw;
			/// \todo Requires a dependency graph implementation
		}
		
	}
}