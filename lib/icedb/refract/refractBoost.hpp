#pragma once
#include "../defs.h"

#include <complex>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/parameter/keyword.hpp>
#include <boost/parameter/name.hpp>
#include <boost/parameter/preprocessor.hpp>
#include <boost/lexical_cast.hpp>
#include "../units/units.hpp"
#include "refractBase.hpp"
#include "../optionsForwards.hpp"

namespace icedb {
	namespace refract {
		namespace boost {

			/// Dielectric providers - these use f and T to automatically determine the correct
			/// base dielectric function to use.
			BOOST_PARAMETER_NAME(frequency)
			BOOST_PARAMETER_NAME(temperature)
			BOOST_PARAMETER_NAME(salinity)
			BOOST_PARAMETER_NAME(temp_units)
			BOOST_PARAMETER_NAME(freq_units)
			BOOST_PARAMETER_NAME(salinity_units)
			BOOST_PARAMETER_NAME(m)
			BOOST_PARAMETER_NAME(provider)

			/// Really generic (has everything and hides details from user)
#define standardGenericProvider(name) \
		BOOST_PARAMETER_FUNCTION( \
			(void), \
				name, \
				tag, \
				(required \
				(frequency, (double)) \
				(temperature, (double)) \
				(in_out(m), *)) \
				(optional \
				(freq_units, *, std::string("GHz")) \
				(temp_units, *, std::string("K")) \
				(provider, (const char*), "")) \
				) \
					{ \
				double freq = rtmath::units::conv_spec(freq_units, "GHz").convert(frequency); \
				double temp = rtmath::units::converter(temp_units, "degK").convert(temperature); \
				implementations:: name(freq, temp, m, provider); \
					}

			standardGenericProvider(mWater);
			standardGenericProvider(mIce);
			standardGenericProvider(mOther);

#define standardFTmProvider(name) \
		BOOST_PARAMETER_FUNCTION( \
			(void), \
				name, \
				tag, \
				(required \
				(frequency, (double)) \
				(temperature, (double)) \
				(in_out(m), *)) \
				(optional \
				(freq_units, *, std::string("GHz")) \
				(temp_units, *, std::string("degK"))) \
				) \
			{ \
				double freq = rtmath::units::conv_spec(freq_units, "GHz").convert(frequency); \
				double temp = rtmath::units::converter(temp_units, "degK").convert(temperature); \
				implementations:: name(freq, temp, m); \
			}

			standardFTmProvider(mWaterLiebe);
			standardFTmProvider(mIceMatzler);
			standardFTmProvider(mIceWarren);
			standardFTmProvider(mWaterFreshMeissnerWentz);

#define standardLmProvider(name) \
		BOOST_PARAMETER_FUNCTION( \
			(void), \
				name, \
				tag, \
				(required \
					(frequency, (double)) \
					(in_out(m), *) ) \
				(optional \
					(temperature, (double), 0) \
					(freq_units, *, std::string("GHz")) \
					(temp_units, *, std::string("degK")) ) \
				) \
					{ \
				double lambda = rtmath::units::conv_spec(freq_units, "um").convert(frequency); \
				double temp = rtmath::units::converter(temp_units, "degK").convert(temperature); \
				implementations:: name(lambda, m); \
					}
			
			standardLmProvider(mWaterHanel);
			standardLmProvider(mIceHanel);
			standardLmProvider(mNaClHanel);
			standardLmProvider(mSeaSaltHanel);
			standardLmProvider(mDustHanel);
			standardLmProvider(mSandOHanel);
			standardLmProvider(mSandEHanel);

		}
	}
}

