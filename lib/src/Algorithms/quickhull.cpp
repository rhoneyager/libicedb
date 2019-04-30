#include "icedb/ShapeAlgs.hpp"
#include "Algorithms.hpp"
#include "quickhull/QuickHull.hpp"
#include <Eigen/Dense>
#include <HH/Groups.hpp>
#include "icedb/Shapes.hpp"

namespace icedb {
	namespace ShapeAlgs {
		namespace Algorithms {
			namespace _internal_algs {
				void getConvexHullInfoAkuukkaQuickhull(
					const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& inPoints,
					Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& outPoints)
				{
					using namespace quickhull;
					QuickHull<double> qh; // Could be double as well


					std::vector<Vector3<double>> pointCloud(inPoints.rows());
					// Add points to point cloud
					for (int row = 0; row < inPoints.rows(); row++)
					{
						pointCloud[row] = Vector3<double>(inPoints(row, 0), inPoints(row, 1), inPoints(row, 2));
					}
					auto hull = qh.getConvexHull(pointCloud, true, false);

					//auto hull = qh.getConvexHull(inPoints.data(), (int) inPoints.size(), true, false);
					auto indexBuffer = hull.getIndexBuffer();
					auto vertexBuffer = hull.getVertexBuffer();


					//std::cout << indexBuffer.size() << std::endl << std::endl << std::endl;

					//for (const auto& i : indexBuffer)
					//	std::cout << i << std::endl;

					//std::cout << std::endl << std::endl << std::endl;

					//std::cout << vertexBuffer.size() << std::endl << std::endl << std::endl;
					//for (const auto& i : vertexBuffer)
					//	std::cout << i.x << "\t" << i.y << "\t" << i.z << std::endl;

					auto outNumPoints = vertexBuffer.size();
					outPoints.resize(outNumPoints, 3);
					for (int i = 0; i < vertexBuffer.size(); ++i) {
						outPoints(i, 0) = vertexBuffer[i].x;
						outPoints(i, 1) = vertexBuffer[i].y;
						outPoints(i, 2) = vertexBuffer[i].z;
					}
				}

				Algorithm ConvexHull
				{ []()->Algorithm::AlgorithmConstructor {
					Algorithm::AlgorithmConstructor a;
					a.RequiredStructuralDatasets = { "particle_scattering_element_coordinates", "particle_scattering_element_number" };
					a.ProvidedDatasets = { "ConvexHullPoints" };
					a.func = [](HH::Group res, const HH::Group & shp, const gsl::span<const HH::Group> & inPPPs)
					{
						const auto& SD = icedb::Shapes::Required_Dsets;
						const auto& SDO = icedb::Shapes::Optional_Dsets;
						using SO = icedb::Shapes::e_Common_Obj_Names;

						auto numPoints = shp.dsets[SD.at(SO::particle_scattering_element_number).first].getDimensions().numElements;
						typedef Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenXXDr;
						EigenXXDr shpPoints((int)numPoints, 3);

						const std::string PointDatasetName = SDO.at(SO::particle_scattering_element_coordinates_int).first;
						auto dPts = shp.dsets[PointDatasetName];
						if (dPts.isOfType<int32_t>()) {
							Eigen::ArrayXXi inPts;
							shp.dsets[SDO.at(SO::particle_scattering_element_coordinates_int).first].readWithEigen(inPts);
							shpPoints = inPts.cast<double>();
						}
						else if (dPts.isOfType<float>()) {
							Eigen::ArrayXXf inPts;
							shp.dsets[SDO.at(SO::particle_scattering_element_coordinates_float).first].readWithEigen(inPts);
							shpPoints = inPts.cast<double>();
						}
						else throw;
						//std::cerr << shpPoints << std::endl;


						EigenXXDr hullPoints;
						getConvexHullInfoAkuukkaQuickhull(shpPoints, hullPoints);

						res.dsets.createWithEigen("ConvexHullPoints", hullPoints);
					};
					a.weight = 100;
					return a;
	}() };

			}
		}
	}
}
