#include "icedb/Algorithms/ShapeAlgs.hpp"
#include "icedb/Algorithms/Algorithms.hpp"
#include "icedb/IO/Shapes.hpp"
#include "BetterThrow/Error.hpp"

namespace icedb {
	namespace ShapeAlgs {
		namespace Algorithms {
			namespace _internal_algs {
				/// Algorithm that determines the maximum particle diameter, in meters, which is set as an attribute.
				Algorithm MaxDistanceTwoPoints{ []()->Algorithm::AlgorithmConstructor {
					Algorithm::AlgorithmConstructor a;
					a.ProvidedAttributes = { "MaximumDistanceAnyTwoElements" };
					a.ProvidedDatasets = { "MaximumDistanceAnyTwoElements_points" };
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
						pts.resizeLike(ipts);
						pts = ipts.cast<double>();

						double maxSqDist = 0;
						Eigen::Array3f furthest_a, furthest_b;
						auto rows = pts.rows();
						auto cols = pts.cols();
						for (int i = 0; i < rows; ++i) {
							for (int j = i + 1; j < rows; ++j) {
								Eigen::Array3d dist = (pts.block(i, 0, 1, 3) - pts.block(j, 0, 1, 3)).transpose();
								double distsq = dist.square().sum();
								if (distsq > maxSqDist) {
									maxSqDist = distsq;
									furthest_a = pts.block(i, 0, 1, 3).transpose().cast<float>();
									furthest_b = pts.block(j, 0, 1, 3).transpose().cast<float>();
								}
							}
						}

						res.atts.add<float>("MaximumDistanceAnyTwoElements", (float)std::sqrt(maxSqDist));
						res.dsets.create<float>("MaximumDistanceAnyTwoElements_points", { 2,3 })
							.write<float>({ furthest_a(0), furthest_a(1), furthest_a(2),
							furthest_b(0), furthest_b(1), furthest_b(2) });
					};
					a.weight = 40;
					return a;
				}() };
			}
		}
	}
}
