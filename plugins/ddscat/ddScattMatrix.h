#pragma once
#include "../../../icedb/defs.h"
#include <complex>
#include <vector>
#include <Eigen/Core>
#include <memory>
#include <boost/operators.hpp>
//#include "../common_templates.h"

namespace icedb
{
	namespace io {
		namespace ddscat
		{
			class ddPar;
			class ddScattMatrixConnector;
			class ddScattMatrix;
			class ddScattMatrixF;
			class ddScattMatrixP;
			enum class scattMatrixType
			{
				F,
				S,
				P
			};

			/// Connector object that provides target frame information
			class DL_ICEDB_IO_DDSCAT ddScattMatrixConnector
			{
				ddScattMatrixConnector();
				ddScattMatrixConnector(const ddPar &src);
				ddScattMatrixConnector(const std::vector<std::complex<double> >&);
				/// Calc e2 as x_lf X e1
				void calcE2();
			public:
				/// Predefined defaults
				static std::shared_ptr<const ddScattMatrixConnector> defaults();
				/// Load from a ddscat.par file
				static std::shared_ptr<const ddScattMatrixConnector> fromPar(const ddPar &src);
				/** \brief Directly specify from a ddOutputSingle or elsewhere
				*
				* If the vector has six elements, these, in order are e1x, e1y, e1z, e2x, e2y, e2z.
				* If it has three elements, define e2 as x_lf X e1.
				**/
				static std::shared_ptr<const ddScattMatrixConnector> fromVector(
					const std::vector<std::complex<double> >&);

				std::complex<double> e01x, e01y, e01z;
				std::complex<double> e02x, e02y, e02z;
			};

			/**
			* \brief Provides the Mueller scattering matrix
			*
			* This is now a base class because there are two ways for
			* specifying scattering: the complex scattering amplitude
			* matrix or the scattering phase matrix. ddscat intermediate
			* output gives the complex matrix which is more useful. The
			* P matrix is harder to derive from, but is found in the
			* .avg files and in tmatrix code.
			**/
			class DL_ICEDB_IO_DDSCAT ddScattMatrix
			{
			public:
				typedef Eigen::Matrix4d PnnType;
				typedef Eigen::Matrix2cd FType;
				ddScattMatrix(double freq = 0, double theta = 0, double phi = 0, double thetan = 0, double phin = 0);
				virtual ~ddScattMatrix();
				virtual ddScattMatrix* clone() const = 0;
				//ddScattMatrix & operator = (const ddScattMatrix&); // Assignment needed due to arrays

				virtual PnnType mueller() const;
				inline double pol() const { return _pol; }
				inline void pol(double p) { _pol = p; }
				inline double polLin() const { return _pollin; }
				inline void polLin(double p) { _pollin = p; }
				virtual scattMatrixType id() const { return scattMatrixType::P; }

				inline double freq() const { return _freq; }
				inline double theta() const { return _theta; }
				inline double thetan() const { return _thetan; }
				inline double phi() const { return _phi; }
				inline double phin() const { return _phin; }

				virtual bool operator<(const ddScattMatrix&) const;

				virtual bool compareTolHeader(const ddScattMatrix&, double tolPercent = 0.01) const;
				EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			protected:
				mutable PnnType _Pnn;
				//mutable std::shared_ptr<matrixop> _Pnn;
				double _pol;
				double _pollin;
				double _freq, _theta, _thetan, _phi, _phin;
				/// Calculate polarization from Mueller matrix.
				void _calcPol();
				/// Calculate linear polarization (what ddscat reports)
				void _calcPolLin();
			};


			/**
			* \brief Provides the amplitude scattering matrix precursor, which can
			* be converted into the Mueller matrix.
			*
			* \todo Add extinction calculations
			* \todo Add scattering cross-sections
			**/
			class DL_ICEDB_IO_DDSCAT ddScattMatrixF :
				public ddScattMatrix //,
				//boost::additive<ddScattMatrixF>,
				//boost::multiplicative<ddScattMatrixF, double>
			{
			public:
				/// Needs frequency (GHz) and phi (degrees) for P and K calculations

				ddScattMatrixF(double freq = 0, double theta = 0, double phi = 0, double thetan = 0, double phin = 0,
					std::shared_ptr<const ddScattMatrixConnector> frame = ddScattMatrixConnector::defaults())
					: ddScattMatrix(freq, theta, phi, thetan, phin), frame(frame) {}
				virtual ~ddScattMatrixF();
				virtual ddScattMatrixF* clone() const;
				//ddScattMatrixF operator+(const ddScattMatrixF&) const;
				//ddScattMatrixF operator*(double) const;
				//ddScattMatrixF & operator = (const ddScattMatrixF&);
				virtual scattMatrixType id() const { return scattMatrixType::F; }
				// matrixop extinction() const;
				/// Calculate Mueller matrix from F matrix
				virtual PnnType mueller() const;
				void setF(const FType& fs);
				//void setF(std::istream &lss); // Include this higher up
				inline FType getF() const { return _f; }
				inline FType getS() const { return _s; }
			protected:
				/// Calculate scattering amplitude matrix from the scattering (f_ml) matrix
				void _calcS() const;
				/// Calculate Mueller matrix from scattering amplitude matrix
				void _calcP() const;
				mutable FType _f, _s;
				std::shared_ptr<const ddScattMatrixConnector> frame;
				//boost::shared_array<std::complex<double> > _f, _s;
				//std::shared_ptr<matrixop> _fRe, _fIm; // Should store as shared_array
				//std::shared_ptr<matrixop> _sRe, _sIm;
			};

			/**
			* \brief Provides the amplitude scattering matrix, which can
			* be converted into the Mueller matrix.
			*
			* \todo Add extinction calculations
			* \todo Add scattering cross-sections
			**/
			class DL_ICEDB_IO_DDSCAT ddScattMatrixS :
				public ddScattMatrix //,
				//boost::additive<ddScattMatrixF>,
				//boost::multiplicative<ddScattMatrixF, double>
			{
			public:
				/// Needs frequency (GHz) and phi (degrees) for P and K calculations

				ddScattMatrixS(double freq = 0, double theta = 0, double phi = 0, double thetan = 0, double phin = 0,
					std::shared_ptr<const ddScattMatrixConnector> frame = ddScattMatrixConnector::defaults())
					: ddScattMatrix(freq, theta, phi, thetan, phin), frame(frame) {}
				virtual ~ddScattMatrixS();
				virtual ddScattMatrixS* clone() const;
				virtual scattMatrixType id() const { return scattMatrixType::S; }
				// matrixop extinction() const;
				/// Calculate Mueller matrix from F matrix
				virtual PnnType mueller() const;
				void setS(const FType& s);
				inline FType getS() const { return _s; }
			protected:
				/// Calculate Mueller matrix from scattering amplitude matrix
				void _calcP() const;
				mutable FType _s;
				std::shared_ptr<const ddScattMatrixConnector> frame;
			};


			/// Represents data for just a Mueller matrix
			class DL_ICEDB_IO_DDSCAT ddScattMatrixP :
				public ddScattMatrix //,
				//boost::additive<ddScattMatrixP>,
				//boost::multiplicative<ddScattMatrixP, double>
			{
			public:
				ddScattMatrixP(double freq = 0, double theta = 0, double phi = 0, double thetan = 0, double phin = 0)
					: ddScattMatrix(freq, theta, phi, thetan, phin) {}
				virtual ~ddScattMatrixP() {}
				virtual ddScattMatrixP* clone() const;
				//ddScattMatrixP operator+(const ddScattMatrixP&) const;
				//ddScattMatrixP operator*(double) const;

				//ddScattMatrixP & operator = (const ddScattMatrixP&);
				virtual scattMatrixType id() const { return scattMatrixType::P; }
				inline void setP(const PnnType& v) { _Pnn = v; }
				PnnType getP() const { return _Pnn; }
			};
		}
	}
}
