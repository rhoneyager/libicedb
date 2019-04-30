#include <string>

#include <icedb/defs.h>
#include <icedb/plugin.hpp>
#include "plugin-qhull.hpp"
#include "libqhullcpp/Qhull.h"
#include "libqhullcpp/RboxPoints.h"
#include "libqhullcpp/QhullFacetList.h"
#include "QuickHull.hpp"
#include <Eigen/Dense>
#include <HH/Groups.hpp>
#include <icedb/ShapeAlgs.hpp>
#include <icedb/Shapes.hpp>
#include <iostream>

namespace icedb
{
	namespace plugins
	{
		namespace qhull
		{
			void getConvexHullInfoQHull(
				const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &inPoints,
				Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &outPoints,
				double& volume,
				double& area)
			{
				using namespace orgQhull;
				//PointCoordinates points;
				RboxPoints rbox;
				int inDimensionality = (int) inPoints.cols();
				int inNumPoints = (int) inPoints.rows();
				rbox.setDimension(inDimensionality);
				rbox.append(inNumPoints * inDimensionality, inPoints.data());
				Qhull qhull;
				qhull.runQhull(rbox, "");
				
				//qhull.outputQhull();
				volume = qhull.volume();
				area = qhull.area();
				
				QhullFacetList facets = qhull.facetList();

				auto outQPoints = qhull.points();

				auto qhpsv = outQPoints.toStdVector();
				auto outNumPoints = qhpsv.size();
				outPoints.resize(outNumPoints, inDimensionality);
				for (int i = 0; i < qhpsv.size(); ++i) {
					for (int col = 0; col < inDimensionality; ++col)
						outPoints(i, col) = qhpsv[i][col];
				}

			}

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


			using namespace ShapeAlgs::Algorithms;

			Algorithm algConvxHullQhull
			{ []()->Algorithm::AlgorithmConstructor {
				Algorithm::AlgorithmConstructor a;
				a.RequiredStructuralDatasets = { "particle_scattering_element_coordinates", "particle_scattering_element_number" };
				a.ProvidedAttributes = { "ConvexHullVolume", "ConvexHullArea" };
				a.ProvidedDatasets = { "ConvexHullPoints" };
				a.func = [](HH::Group res, const HH::Group & shp, const gsl::span<const HH::Group> &inPPPs)
				{
					const auto& SD = icedb::Shapes::Required_Dsets;
					const auto& SDO = icedb::Shapes::Optional_Dsets;
					using SO = icedb::Shapes::e_Common_Obj_Names;

					auto numPoints = shp.dsets[SD.at(SO::particle_scattering_element_number).first].getDimensions().numElements;
					typedef Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenXXDr;
					EigenXXDr shpPoints((int) numPoints, 3);
					
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
					double volume = 0, area = 0;
					getConvexHullInfoQHull(shpPoints, hullPoints, volume, area);

					res.atts.add("ConvexHullVolume", volume);
					res.atts.add("ConvexHullArea", area);
					res.dsets.createWithEigen("ConvexHullPoints", hullPoints);
				};
				a.weight = 9900;
				return a;
}() };

			Algorithm algConvxHullAkuukkaQuickhull
			{ []()->Algorithm::AlgorithmConstructor {
				Algorithm::AlgorithmConstructor a;
				a.RequiredStructuralDatasets = { "particle_scattering_element_coordinates", "particle_scattering_element_number" };
				a.ProvidedDatasets = { "ConvexHullPoints" };
				a.func = [](HH::Group res, const HH::Group & shp, const gsl::span<const HH::Group> &inPPPs)
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

D_icedb_start()
{
	using namespace icedb::registry;
	using namespace icedb::plugins::qhull;
	// Tell icedb about the plugin.
	static const icedb::registry::DLLpreamble id(
		"plugin-qhull",
		"Quickhull calculations",
		PLUGINID);
	dllInitResult res = icedb_registry_register_dll(id, (void*)dllStart);
	if (res != SUCCESS) return res;

	icedb::ShapeAlgs::Algorithms::common_algorithms.emplace(std::make_pair(
		std::string("ConvexHullQHull"),
		icedb::plugins::qhull::algConvxHullQhull
	));
	icedb::ShapeAlgs::Algorithms::common_algorithms.emplace(std::make_pair(
		std::string("ConvexHullAkuukkaQuickhull"),
		icedb::plugins::qhull::algConvxHullAkuukkaQuickhull
	));

	return SUCCESS;
}
