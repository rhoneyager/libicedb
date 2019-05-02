#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <complex>
#include <boost/math/constants/constants.hpp>
#include "ddScattMatrix.h"
#include "ddpar.h"
#include "icedb/units/units.hpp"
#include "phaseFunc.h"
#include "icedb/Errors/error.hpp"

namespace icedb {
	namespace io {
		namespace ddscat {

			ddScattMatrix::ddScattMatrix(double freq, double theta, double phi, double thetan, double phin) :
				_pol(0),
				_pollin(0),
				_freq(freq),
				_theta(theta),
				_thetan(thetan),
				_phi(phi),
				_phin(phin)
			{
			}

			ddScattMatrix::~ddScattMatrix() {}

			bool ddScattMatrix::operator<(const ddScattMatrix &rhs) const
			{
#define comp(x) if (x != rhs.x) return (x < rhs.x);
				comp(_freq);
				comp(_phi);
				comp(_theta);
				comp(_phin);
				comp(_thetan);
				comp(_pol);
#undef comp
				return false;
			}

			bool ddScattMatrix::compareTolHeader(const ddScattMatrix &rhs, double tol) const
			{
				// NOTE: the preprocessor handles math in a different order!
#define tol(x) if ( abs( 100.*x/rhs.x - 100.0 ) > tol) return false;
				tol(_freq);
				tol(_theta);
				tol(_thetan);
				tol(_phi);
				tol(_phin);
				tol(_pol);
				tol(_pollin);
#undef tol
				return true;
			}

			ddScattMatrix::PnnType ddScattMatrix::mueller() const
			{
				return _Pnn;
			}

			void ddScattMatrix::_calcPolLin()
			{
				_pollin = sqrt((pow(_Pnn(1, 0), 2.) +
					pow(_Pnn(2, 0), 2.))
					/ pow(_Pnn(0, 0), 2.));
			}

			void ddScattMatrix::_calcPol()
			{
				_pol = sqrt((pow(_Pnn(1, 0), 2.) +
					pow(_Pnn(2, 0), 2.) +
					pow(_Pnn(3, 0), 2.))
					/ pow(_Pnn(0, 0), 2.));
			}

			ddScattMatrixF::~ddScattMatrixF() {}

			ddScattMatrixF* ddScattMatrixF::clone() const
			{
				ddScattMatrixF* res = new ddScattMatrixF(*this);
				return res;
			}

			ddScattMatrixP* ddScattMatrixP::clone() const
			{
				ddScattMatrixP* res = new ddScattMatrixP(*this);
				return res;
			}

			ddScattMatrixF::PnnType ddScattMatrixF::mueller() const
			{
				_calcS();
				_calcP();
				return _Pnn;
			}

			ddScattMatrixConnector::ddScattMatrixConnector() :
				e01x(0, 0), e01y(1, 0), e01z(0, 0),
				e02x(0, 0), e02y(0, 0), e02z(1, 0)
			{ }

			void ddScattMatrixConnector::calcE2()
			{
				typedef std::complex<double> cpl;
				// e02 = x_lf X e01
				const cpl xlfx(1, 0), xlfy(0, 0), xlfz(0, 0);

				e02x = (xlfy * e01z) - (xlfz * e01y);
				e02y = (xlfz * e01x) - (xlfx * e01z);
				e02z = (xlfx * e01y) - (xlfy * e01x);
			}

			ddScattMatrixConnector::ddScattMatrixConnector(const ddPar &src) :
				e01x(src.PolState(0), src.PolState(1)),
				e01y(src.PolState(2), src.PolState(3)),
				e01z(src.PolState(4), src.PolState(5))
			{
				if (src.OrthPolState() == 2)
					calcE2();
			}

			ddScattMatrixConnector::ddScattMatrixConnector(
				const std::vector<std::complex<double> >& src)
			{
				if (src.size() % 3 != 0 || !src.size()) ICEDB_throw(icedb::error::error_types::xArrayOutOfBounds)
					.add("Reason","Element src has the wrong size or is a "
						"null matrix.");
				e01x = src[0];
				e01y = src[1];
				e01z = src[2];
				if (src.size() == 3)
					calcE2();
				else
				{
					e02x = src[3];
					e02y = src[4];
					e02z = src[5];
				}
			}

			std::shared_ptr<const ddScattMatrixConnector>
				ddScattMatrixConnector::fromVector(
					const std::vector<std::complex<double> >& vec)
			{
				std::shared_ptr<const ddScattMatrixConnector>
					res(new ddScattMatrixConnector(vec));
				return res;
			}

			std::shared_ptr<const ddScattMatrixConnector>
				ddScattMatrixConnector::fromPar(const ddPar &src)
			{
				std::shared_ptr<const ddScattMatrixConnector>
					res(new ddScattMatrixConnector(src));
				return res;
			}

			std::shared_ptr<const ddScattMatrixConnector> ddScattMatrixConnector::defaults()
			{
				static std::shared_ptr<const ddScattMatrixConnector>
					res(new ddScattMatrixConnector());
				return res;
			}

			void ddScattMatrixF::_calcS() const
			{
				using namespace std;
				//const double PI = boost::math::constants::pi<double>();
				//const complex<double> i(0,1);
				//complex<double> e01x(0,0), e01y(1,0), e01z(0,0), e02x(0,0), e02y(0,0), e02z(1,0);
				//complex<double> a = conj(e01y), b=conj(e01z), c=conj(e02y), d=conj(e02z);
				complex<double> a = conj(frame->e01y), b = conj(frame->e01z),
					c = conj(frame->e02y), d = conj(frame->e02z);
				icedb::io::ddscat::phaseFuncs::convertFtoS(_f, _s, _phi, a, b, c, d);
			}

			void ddScattMatrixF::_calcP() const
			{
				// Generates Snn and, by extension, Knn and Pnn
				using namespace std;
				icedb::io::ddscat::phaseFuncs::muellerBH(_s, _Pnn);
			}

			void ddScattMatrixF::setF(const FType& fs)
			{
				_f = fs;
				_calcS();
				_calcP();

				_calcPolLin();
				_calcPol();
			}

			ddScattMatrixS::~ddScattMatrixS() {}

			ddScattMatrixS* ddScattMatrixS::clone() const
			{
				ddScattMatrixS* res = new ddScattMatrixS(*this);
				return res;
			}

			ddScattMatrixS::PnnType ddScattMatrixS::mueller() const
			{
				_calcP();
				return _Pnn;
			}

			void ddScattMatrixS::_calcP() const
			{
				// Generates Snn and, by extension, Knn and Pnn
				using namespace std;
				icedb::io::ddscat::phaseFuncs::muellerBH(_s, _Pnn);
			}

			void ddScattMatrixS::setS(const FType& s)
			{
				_s = s;
				_calcP();

				_calcPolLin();
				_calcPol();
			}

		}
	}
}
