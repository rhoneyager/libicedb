#include <sstream>
#include <string>
#include <set>
#include <boost/math/constants/constants.hpp>
#include <Eigen/Dense>
#include <Eigen/Geometry>

#include "ddpar.h"
#include "rotations.h"
#include "icedb/Utils/splitSet.hpp"

namespace icedb {
	namespace io {
		namespace ddscat {

			void rotations::populateRotations(const std::set<double> &betas,
				const std::set<double> &thetas, const std::set<double> &phis,
				std::map<std::tuple<double, double, double>, size_t> &out)
			{
				// Do NOT clear the output.
				for (auto &beta : betas)
					for (auto &theta : thetas)
						for (auto &phi : phis)
						{
							std::tuple<double, double, double> rot(beta, theta, phi);
							// Find near matches
							std::map<std::tuple<double, double, double>, size_t>::iterator it;
							it = std::find_if(out.begin(), out.end(),
								[&rot](std::pair<const std::tuple<double, double, double>, size_t> &v) -> bool
							{
								// If overall distance is less than 1e-6, then this is the 
								// same point.
								double dsq = pow(std::get<0>(rot) - std::get<0>(v.first), 2.)
									+ pow(std::get<1>(rot) - std::get<1>(v.first), 2.)
									+ pow(std::get<2>(rot) - std::get<2>(v.first), 2.);
								if (dsq < 1.e-3) return true;
								return false;
							});

							if (it == out.end())
							{
								out.insert(std::pair<std::tuple<double, double, double>, size_t>
									(rot, 1));
							}
							else {
								it->second++;
							}
						}
			}

			void rotations::getRots(std::map<std::tuple<double, double, double>, size_t> &out) const
			{
				std::set<double> sb, st, sp;
				betas(sb); thetas(st); phis(sp);
				populateRotations(sb, st, sp, out);
			}

			rotationsBase::rotationsBase()
				:
				_bMin(0), _bMax(360), _bN(6),
				_tMin(0), _tMax(90), _tN(6),
				_pMin(0), _pMax(180), _pN(6)
			{
			}

			rotationsBase::~rotationsBase()
			{
			}

			rotations::rotations()
			{
			}

			rotations::rotations(double bMin, double bMax, size_t bN,
				double tMin, double tMax, size_t tN,
				double pMin, double pMax, size_t pN)
			{
				_bMin = bMin;
				_bMax = bMax;
				_bN = bN;
				_tMin = tMin;
				_tMax = tMax;
				_tN = tN;
				_pMin = pMin;
				_pMax = pMax;
				_pN = pN;
			}

			std::shared_ptr<rotations> rotations::create()
			{
				std::shared_ptr<rotations> res(new rotations());
				return res;
			}

			std::shared_ptr<rotations> rotations::create(
				double bMin, double bMax, size_t bN,
				double tMin, double tMax, size_t tN,
				double pMin, double pMax, size_t pN)
			{
				std::shared_ptr<rotations> res(new rotations(
					bMin, bMax, bN,
					tMin, tMax, tN,
					pMin, pMax, pN));
				return res;
			}

			std::shared_ptr<rotations> rotations::create(const ddPar &src)
			{
				std::shared_ptr<rotations> res(new rotations(src));
				return res;
			}

			rotations::~rotations()
			{
			}

			bool rotations::operator==(const rotations &rhs) const
			{
				if (_bMin != rhs._bMin) return false;
				if (_bMax != rhs._bMax) return false;
				if (_bN != rhs._bN) return false;
				if (_tMin != rhs._tMin) return false;
				if (_tMax != rhs._tMax) return false;
				if (_tN != rhs._tN) return false;
				if (_pMin != rhs._pMin) return false;
				if (_pMax != rhs._pMax) return false;
				if (_pN != rhs._pN) return false;

				return true;
			}

			bool rotations::operator!=(const rotations &rhs) const
			{
				return !(operator==(rhs));
			}

			bool rotations::operator<(const rotations &rhs) const
			{
				if (_bMin != rhs._bMin) return _bMin < rhs._bMin;
				if (_bMax != rhs._bMax) return _bMax < rhs._bMax;
				if (_bN != rhs._bN) return _bN < rhs._bN;
				if (_tMin != rhs._tMin) return _tMin < rhs._tMin;
				if (_tMax != rhs._tMax) return _tMax < rhs._tMax;
				if (_tN != rhs._tN) return _tN < rhs._tN;
				if (_pMin != rhs._pMin) return _pMin < rhs._pMin;
				if (_pMax != rhs._pMax) return _pMax < rhs._pMax;
				if (_pN != rhs._pN) return _pN < rhs._pN;

				return false;
			}

			rotations::rotations(const ddPar &src)
			{
				std::shared_ptr<const ddParParsers::ddParLine > ob, ot, op;
				rotations defaults;
				if (src.exists(ddParParsers::NBETA))
				{
					src.getKey(ddParParsers::NBETA, ob);
					std::shared_ptr<const ddParParsers::ddParLineMixed<double, size_t> > b =
						std::static_pointer_cast<const ddParParsers::ddParLineMixed<double, size_t>>
						(ob);
					b->get<double>(0, _bMin);
					b->get<double>(1, _bMax);
					b->get<size_t>(2, _bN);
				}
				else {
					_bMin = defaults.bMin();
					_bMax = defaults.bMax();
					_bN = defaults.bN();
				}

				if (src.exists(ddParParsers::NTHETA))
				{
					src.getKey(ddParParsers::NTHETA, ot);
					std::shared_ptr<const ddParParsers::ddParLineMixed<double, size_t> > t =
						std::static_pointer_cast<const ddParParsers::ddParLineMixed<double, size_t>>
						(ot);
					t->get<double>(0, _tMin);
					t->get<double>(1, _tMax);
					t->get<size_t>(2, _tN);
				}
				else {
					_tMin = defaults.tMin();
					_tMax = defaults.tMax();
					_tN = defaults.tN();
				}

				if (src.exists(ddParParsers::NPHI))
				{
					src.getKey(ddParParsers::NPHI, op);
					std::shared_ptr<const ddParParsers::ddParLineMixed<double, size_t> > p =
						std::static_pointer_cast<const ddParParsers::ddParLineMixed<double, size_t>>
						(op);
					p->get<double>(0, _pMin);
					p->get<double>(1, _pMax);
					p->get<size_t>(2, _pN);
				}
				else {
					_pMin = defaults.pMin();
					_pMax = defaults.pMax();
					_pN = defaults.pN();
				}
			}

			void rotations::out(ddPar &dest) const
			{
				using namespace std;
				shared_ptr<ddParParsers::ddParLineMixed<double, size_t> >
					b(new ddParParsers::ddParLineMixed<double, size_t>(2, 3, ddParParsers::NBETA)),
					t(new ddParParsers::ddParLineMixed<double, size_t>(2, 3, ddParParsers::NTHETA)),
					p(new ddParParsers::ddParLineMixed<double, size_t>(2, 3, ddParParsers::NPHI));
				b->set<double>(0, _bMin);
				b->set<double>(1, _bMax);
				b->set<size_t>(2, _bN);
				t->set<double>(0, _tMin);
				t->set<double>(1, _tMax);
				t->set<size_t>(2, _tN);
				p->set<double>(0, _pMin);
				p->set<double>(1, _pMax);
				p->set<size_t>(2, _pN);

				dest.insertKey(ddParParsers::NBETA, static_pointer_cast<ddParParsers::ddParLine>(b));
				dest.insertKey(ddParParsers::NTHETA, static_pointer_cast<ddParParsers::ddParLine>(t));
				dest.insertKey(ddParParsers::NPHI, static_pointer_cast<ddParParsers::ddParLine>(p));
			}

			void rotations::betas(std::string &dest) const
			{
				using namespace std;
				ostringstream out;
				out << bMin() << ":" << bN() << ":" << bMax() << ":LIN";
				dest = out.str();
			}

			void rotations::thetas(std::string &dest) const
			{
				using namespace std;
				ostringstream out;
				out << tMin() << ":" << tN() << ":" << tMax() << ":COS";
				dest = out.str();
			}

			void rotations::phis(std::string &dest) const
			{
				using namespace std;
				ostringstream out;
				out << pMin() << ":" << pN() << ":" << pMax() << ":LIN";
				dest = out.str();
			}

			void rotations::betas(std::set<double> &b) const
			{
				using namespace std;
				string sbetas;
				betas(sbetas);
				splitSet::splitSet<double>(sbetas, b);
			}

			void rotations::thetas(std::set<double> &t) const
			{
				using namespace std;
				string sthetas;
				thetas(sthetas);
				splitSet::splitSet<double>(sthetas, t);
			}

			void rotations::phis(std::set<double> &p) const
			{
				using namespace std;
				string sphis;
				phis(sphis);
				splitSet::splitSet<double>(sphis, p);
			}

			template<class T>
			void a1(T thetad, T phid, Eigen::Matrix<T, 3, 1, 0, 3, 1> &a1)
			{
				T scale = (boost::math::constants::pi<T>()) / (T) 180.0;
				T thetar = thetad * scale;
				T phir = phid * scale;

				using namespace Eigen;
				typedef Matrix<T, 3, 1, 0, 3, 1> vt;
				/// DDSCAT uses a strange z-x-x" convention when doing rotations.

				a1 = (vt::UnitX() * cos(thetar))
					+ (vt::UnitY() * sin(thetar)*cos(phir))
					+ (vt::UnitZ() * sin(thetar)*sin(phir));
			}

			template void a1(double thetad, double phid,
				Eigen::Matrix<double, 3, 1, 0, 3, 1> &a1);
			template void a1(float thetad, float phid,
				Eigen::Matrix<float, 3, 1, 0, 3, 1> &a1);

			template<class T>
			void rotationMatrix(T thetad, T phid, T betad,
				Eigen::Matrix<T, 3, 3, 0, 3, 3> &Reff)
			{
				T scale = (boost::math::constants::pi<T>()) / (T) 180.0;
				T betar = betad * scale;
				T thetar = thetad * scale;
				T phir = phid * scale;

				using namespace Eigen;
				typedef Matrix<T, 3, 1, 0, 3, 1> vt;
				/// DDSCAT uses a strange z-x-x" convention when doing rotations.

				vt a1 = (vt::UnitX() * cos(thetar))
					+ (vt::UnitY() * sin(thetar)*cos(phir))
					+ (vt::UnitZ() * sin(thetar)*sin(phir));

				Reff = AngleAxis<T>(thetar, vt::UnitZ())
					* AngleAxis<T>(phir, vt::UnitX())
					* AngleAxis<T>(betar, a1);
			}

			template void rotationMatrix(double thetad, double phid, double betad,
				Eigen::Matrix3d &Reff);
			template void rotationMatrix(float thetad, float phid, float betad,
				Eigen::Matrix3f &Reff);

			/*
			template<class T>
			void decomposeRotationMatrix(const Eigen::Matrix<T, 3, 1, 0, 3, 1> &norm,
				T eff_deg, T &thetad, T &phid, T &betad)
			{
				using namespace Eigen;
				typedef Matrix<T, 3, 1, 0, 3, 1> vt;
				typedef Matrix<T, 3, 3, 0, 3, 3> vtm;
				vtm Reff = AngleAxis<T>(eff_deg, norm).toRotationMatrix();
				// Take the rotation matrix and decompose it into three gimbal rotations,
				// following ddscat conventions.
				decomposeRotationMatrix<T>(Reff, thetad, phid, betad);
			}

			template<class T>
			void decomposeRotationMatrix(const Eigen::Matrix<T, 3, 3, 0, 3, 3> &Reff,
				T &thetad, T &phid, T &betad)
			{
				using namespace Eigen;
				Matrix3f a;
				Vector3f ang = a.eulerAngles(2, 0, 2);
			}

			template void decomposeRotationMatrix(
				const Eigen::Matrix<float, 3, 1, 0, 3, 1> &norm,
				float eff_deg, float &thetad, float &phid, float &betad);
			template void decomposeRotationMatrix(
				const Eigen::Matrix<double, 3, 1, 0, 3, 1> &norm,
				double eff_deg, double &thetad, double &phid, double &betad);
			template void decomposeRotationMatrix(
				const Eigen::Matrix<float, 3, 3, 0, 3, 3> &Reff,
				float &thetad, float &phid, float &betad);
			template void decomposeRotationMatrix(
				const Eigen::Matrix<double, 3, 3, 0, 3, 3> &Reff,
				double &thetad, double &phid, double &betad);
				*/

		}
	}
}

