#include "icedb/Algorithms/ShapeAlgs.hpp"
#include "icedb/Algorithms/Algorithms.hpp"
#include <Eigen/Dense>
#include <HH/Groups.hpp>
#include "icedb/IO/Shapes.hpp"
#include "miniball.h"
#include "BetterThrow/Error.hpp"

namespace icedb {
	namespace ShapeAlgs {
		namespace Algorithms {
			namespace _internal_algs {
				/// \todo Use standard constants for pi.
				void getMiniball(
					const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& inPoints,
					Eigen::Array3d& center,
					double& volume,
					double& surface_area,
					double& radius,
					double& diameter)
				{
					//using namespace Seb;
					typedef Seb::Point<double> Point;
					typedef std::vector<Point> PointVector;
					typedef Seb::Smallest_enclosing_ball<double> Miniball;
					PointVector points;
					points.reserve(inPoints.rows());
					for (size_t i = 0; i < (int)inPoints.rows(); ++i)
						points.push_back(Point(3, inPoints.data() + (3 * i)));

					// Compute the miniball by inserting each value
					Miniball mb(3, points);

					radius = mb.radius();
					diameter = radius * 2.;
					const double pi = 3.141592654;
					volume = pi * (4. / 3.) * radius * radius * radius;
					surface_area = 4. * pi * radius * radius;

					//FT rad_squared = mb.squared_radius();
					//cout << "Running time: " << Seb::Timer::instance().lapse("all") << "s" << endl
					//	<< "Radius = " << rad << " (squared: " << rad_squared << ")" << endl
					//	<< "Center:" << endl;
					Miniball::Coordinate_iterator center_it = mb.center_begin();
					//for (int j = 0; j < d; ++j)
					//	cout << "  " << center_it[j] << endl;
					center(0) = center_it[0];
					center(1) = center_it[1];
					center(2) = center_it[2];
				}

				void MiniballFunction(
					HH::Group res,
					const HH::Group & shp,
					const gsl::span<const HH::Group> & inPPPs)
				{
					const auto& SD = icedb::Shapes::Required_Dsets;
					const auto& SDO = icedb::Shapes::Optional_Dsets;
					using SO = icedb::Shapes::e_Common_Obj_Names;
					typedef Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenXXDr;
					EigenXXDr shpPoints; // (0, 3);

					// Prefer to calculate the miniball based on the convex hull points. If those are unavailable,
					// then calculate directly from the raw coordinates.

					const std::string ConvexHullPointsDatasetName{ "ConvexHullPoints" };
					HH::Dataset source;
					for (const auto& inPPP : inPPPs) {
						if (inPPP.dsets.exists(ConvexHullPointsDatasetName)) {
							source = inPPP.dsets[ConvexHullPointsDatasetName];
							break;
						}
					}
					if (!source.isDataset()) throw BT_throw.add("Reason", "Algorithm is missing the sufficient datasets for it to continue.");

					if (source.isOfType<int32_t>()) {
						Eigen::ArrayXXi inPts;
						source.readWithEigen(inPts);
						shpPoints.resizeLike(inPts);
						shpPoints = inPts.cast<double>();
					}
					else if (source.isOfType<float>()) {
						Eigen::ArrayXXf inPts;
						source.readWithEigen(inPts);
						shpPoints.resizeLike(inPts);
						shpPoints = inPts.cast<double>();
					}
					else if (source.isOfType<double>())
						source.readWithEigen(shpPoints);
					else throw BT_throw.add("Reason", "Dataset has the wrong type");



					EigenXXDr hullPoints;
					double volume = 0, surface_area = 0, radius = 0, diameter = 0;
					Eigen::Array3d center;
					getMiniball(shpPoints, center, volume, surface_area, radius, diameter);

					res.atts.add("MaximumDimension_MiniballPoints", diameter);
					res.atts.add("SurfaceArea_MiniballPoints", surface_area);
					res.atts.add("Volume_MiniballPoints", volume);
					res.atts.add("Radius_MiniballPoints", radius);
					res.atts.add("Center_MiniballPoints", { center(0), center(1), center(2) });
				}

				Algorithm SmallestCircumscribingSphere_Points
				{ []()->Algorithm::AlgorithmConstructor {
					Algorithm::AlgorithmConstructor a;
					a.RequiredPPPDatasets = { "ConvexHullPoints" };
					a.ProvidedAttributes = {
						"MaximumDimension_MiniballPoints",
						"SurfaceArea_MiniballPoints",
						"Volume_MiniballPoints",
						"Radius_MiniballPoints",
						"Center_MiniballPoints"
					};
					a.func = MiniballFunction;
					a.weight = 100;
					return a;
	}() };
			}
		}
	}
}
