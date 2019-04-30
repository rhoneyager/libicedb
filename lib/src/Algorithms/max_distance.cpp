#include "icedb/ShapeAlgs.hpp"
#include "Algorithms.hpp"
#include <icedb/Shapes.hpp>
#include "BetterThrow/Error.hpp"

namespace icedb {
	namespace ShapeAlgs {
		namespace Algorithms {
			namespace _internal_algs {
				/// Algorithm that determines the maximum particle diameter, in meters, which is set as an attribute.
				Algorithm MaxDistanceTwoPoints{ []()->Algorithm::AlgorithmConstructor {
					Algorithm::AlgorithmConstructor a;
					a.ProvidedAttributes = { "MaximumDistanceAnyTwoElements" };
					a.RequiredPPPDatasets = { "ConvexHullPoints" };
					a.func = [](HH::Group res, const HH::Group & shp, const gsl::span<const HH::Group> & inPPPs)
					{
						const auto& SD = icedb::Shapes::Required_Dsets;
						const auto& SDO = icedb::Shapes::Optional_Dsets;
						using SO = icedb::Shapes::e_Common_Obj_Names;

						const std::string sDConvex{ "ConvexHullPoints" };
						HH::Dataset dCvx = [&]() -> HH::Dataset {
							for (const auto& p : inPPPs)
							{
								if (p.dsets.exists(sDConvex)) return p.dsets[sDConvex];
							}
							throw BT_throw.add("Reason", "Invalid assumption: dataset does not exist in inPPPs")
								.add("Dataset", sDConvex);
						}();

						auto numPoints = dCvx.getDimensions().dimsCur.at(0);
						typedef Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenXXDr;
						EigenXXDr pts;
						Eigen::ArrayXXf ipts;
						dCvx.readWithEigen(ipts);
						pts = ipts.cast<double>();

						double maxSqDist = 0;
						for (int i = 0; i < pts.rows(); ++i) {
							for (int j = i + 1; j < pts.rows(); ++j) {
								Eigen::ArrayXd dist = pts.block(i, 0, 1, 3) - pts.block(j, 0, 1, 3);
								double distsq = dist.square().sum();
								if (distsq > maxSqDist) maxSqDist = distsq;
							}
						}

						res.atts.add<float>("MaximumDistanceAnyTwoElements", (float)std::sqrt(maxSqDist));
					};
					a.weight = 40;
					return a;
				}() };
			}
		}
	}
}
