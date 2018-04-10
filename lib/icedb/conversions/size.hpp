#pragma once
#include "../defs.h"
#include <map>
#include <set>
#include <functional>
#include <vector>
#include <memory>

namespace icedb {
	namespace conversions {

		/// \brief An ID field for convertible parameters.
		/// \see namespace icedb:conversions::Parameters for a list of the types.
		typedef unsigned short ParamName;
		/** \brief Summaries all of the convertible parameters.
		*
		* All of the conversion classes can handle the Base conversions. More specialized
		* classes add support for further conversions, such as the Regular Ellipsoid conversions.
		* The symbols for these conversions are designed to not clash, so they can be carried
		* by the same ParamSet_t types.
		*
		* \see ParamSet_t
		**/
		namespace Parameters {
			namespace Base {
				enum BaseParamName : ParamName {
					// These parameters describe a fully-solid object.
					// A fully solid object cannot be "squeezed" into a smaller, more dense shape.
					SolidSphereRadius, ///< Radius of a maximum-density sphere. A.k.a. Effective Radius
					SolidVolume, ///< The volume, assuming maximum density
					SolidDensity, ///< The density of the fully-compressed object (usually, the density of water or solid ice)
					Mass, ///< The mass of the object. Mass = Solid Density * Solid Volume.

					// These parameters describe an object that does not have to be a fully-solid sphere.
					// For example, a mixture of ice and air. Or, a shape that is not a sphere.

					// Mixtures:
					MixedVolume, ///< The bulk "volume" of the object. Not-trivially defined for a non-ideal shape.
					MixedDensity, ///< The bulk "density" of the object.
					MixedVolumeFraction, ///< The ratio of Solid Volume / Mixed Volume.
					MixedEquivalentRadius, ///< Radius of an equal-mixed-density sphere

					NUM_BASE_PARAMS
				};
			}
			namespace RegularEllipsoids {
				enum RegularEllipsoidParamName : ParamName {
					// Non-spherical shapes:
					AspectRatio = Base::NUM_BASE_PARAMS+1, ///< Under 1 for oblate (polar < equatorial) , over 1 for prolate (equatorial < polar).
					MixedSemiPolarAxis, ///< Half-length of the axis of rotational symmetry.
					MixedSemiEquatorialAxis, ///< Half-length of the non-rotationally-symmetric axis.
					SolidSemiPolarAxis, ///< Half-length of the axis of rotational symmetry of an equivalent solid object
					SolidSemiEquatorialAxis, ///< Half-length of the axis of rotational symmetry of an equivalent solid object
					// There is no SolidEquivalentRadius. See SolidSphereRadius.
				};
			}
		}
		
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
			bool canInvoke(const ParamSet_t &params) const;
			static std::shared_ptr<const Algorithm > generate(
				Invoke_Func_t alg,
				std::initializer_list<std::initializer_list<ParamName > > neededParams,
				std::initializer_list<ParamName> providesParams)
			{ return std::make_shared<Algorithm>(alg, neededParams, providesParams); }
		};

		class Generic_Object {
		protected:
			std::vector<std::shared_ptr<const Algorithm > > _objAlgs;
			static std::vector<std::shared_ptr<const Algorithm> > _genericObjAlgs;
		public:
			ParamSet_t params;
			virtual ~Generic_Object() {}
			static void generateGenericAlgs();
			static bool hasValidParam(const ParamSet_t &params, ParamName name);
			Generic_Object();
			void clearParams();
			/// \brief Figure out how to solve for the specified parameter.
			/// \returns The value of the parameter on success, negative on failure.
			double findParam(ParamName);
			/// \brief Figure out the specified parameters.
			/// \returns A tuple with the parameters, in order, on success.
			/// \returns Negative values in place on each failure.
			//template <typename Args...>
			//std::tuple<Args...> findParams(Args);
		};

		/// \todo This is a work in progress. Do not use.
		class Ellipsoid_Regular : public Generic_Object {
		protected:
			static std::vector<std::shared_ptr<const Algorithm > > _EllipsoidAlgs;
		public:
			virtual ~Ellipsoid_Regular() {}
			Ellipsoid_Regular();
			static void generateEllipsoidAlgs();
		};
	}
}
