#pragma once
#include "../defs.h"
#include <complex>
#include <functional>

namespace icedb {
	namespace refract {

		/// The raw dielectric providers implementations
		namespace implementations {
			/// Water complex refractive index for microwave for 0 to 1000 GHz
			/// Liebe, Hufford and Manabe (1991)
			DL_ICEDB void mWaterLiebe(double f, double t, std::complex<double> &m);
			/// Water complex refractive index for microwave for 0 to 500 GHz, temps from -20 to 40 C.
			/// This one is for pure water (salinity = 0). There is also a model with salinity (TBI).
			/// Meissner and Wentz (2004)
			DL_ICEDB void mWaterFreshMeissnerWentz(double f, double t, std::complex<double> &m);
			/// Ice complex refractive index
			/// Christian Matzler (2006)
			DL_ICEDB void mIceMatzler(double f, double t, std::complex<double> &m);
			/// Ice complex refractive index for microwave/uv
			DL_ICEDB void mIceWarren(double f, double t, std::complex<double> &m);
			/// Water complex refractive index for ir/vis
			DL_ICEDB void mWaterHanel(double lambda, std::complex<double> &m);
			/// Ice complex refractive index for ir/vis
			DL_ICEDB void mIceHanel(double lambda, std::complex<double> &m);
			/// Sodium chloride refractive index for ir/vis
			DL_ICEDB void mNaClHanel(double lambda, std::complex<double> &m);
			/// Sea salt refractive index for ir/vis
			DL_ICEDB void mSeaSaltHanel(double lambda, std::complex<double> &m);
			/// Dust-like particle refractive index for ir/vis
			DL_ICEDB void mDustHanel(double lambda, std::complex<double> &m);
			/// Sand O-ray refractvie index for ir/vis (birefringent)
			DL_ICEDB void mSandOHanel(double lambda, std::complex<double> &m);
			/// Sand E-ray refractive index for ir/vis (birefringent)
			DL_ICEDB void mSandEHanel(double lambda, std::complex<double> &m);
		}

		/// m to e converters
		DL_ICEDB std::complex<double> mToE(std::complex<double> m);
		DL_ICEDB void mToE(std::complex<double> m, std::complex<double> &e);
		DL_ICEDB std::complex<double> eToM(std::complex<double> e);
		DL_ICEDB void eToM(std::complex<double> e, std::complex<double> &m);

		// Temperature-guessing
		DL_ICEDB double guessTemp(double freq, const std::complex<double> &mToEval,
			std::function<void(double freq, double temp, std::complex<double>& mres)> meth
			= icedb::refract::implementations::mIceMatzler,
			double tempGuessA_K = 263, double tempGuessB_K = 233);

	}
}
