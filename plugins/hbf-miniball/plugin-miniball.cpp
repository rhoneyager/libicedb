#include <string>

#include <icedb/defs.h>
#include <icedb/plugin.hpp>
#include "plugin-miniball.hpp"
#include <Eigen/Dense>
#include <HH/Groups.hpp>
#include <icedb/ShapeAlgs.hpp>
#include <icedb/Shapes.hpp>
#include <iostream>
#include <BetterThrow/Error.hpp>

namespace icedb
{
	namespace plugins
	{
		namespace hbf_miniball
		{
			void getMiniball(
				const Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>& inPoints,
				Eigen::Array3d& center,
				double& volume,
				double& surface_area,
				double& radius,
				double& diameter)
			{

			}

			using namespace ShapeAlgs::Algorithms;

			void MiniballFunction(HH::Group res, const HH::Group& shp, gsl::span<const HH::Group> inPPPs)
			{
				const auto& SD = icedb::Shapes::Required_Dsets;
				const auto& SDO = icedb::Shapes::Optional_Dsets;
				using SO = icedb::Shapes::e_Common_Obj_Names;
				typedef Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> EigenXXDr;
				EigenXXDr shpPoints; // (0, 3);

				// Prefer to calculate the miniball based on the convex hull points. If those are unavailable,
				// then calculate directly from the raw coordinates.

				const std::string PointDatasetName = SDO.at(SO::particle_scattering_element_coordinates_int).first;
				const std::string ConvexHullPointsDatasetName{ "ConvexHullPoints" };
				HH::Dataset source;
				for (const auto& inPPP : inPPPs) {
					if (inPPP.dsets.exists(ConvexHullPointsDatasetName)) {
						source = inPPP.dsets[ConvexHullPointsDatasetName];
						break;
					}
				}
				if (!source.isDataset() && shp.isGroup())
					source = shp.dsets[PointDatasetName];
				else throw BT_throw.add("Reason", "Algorithm is missing the sufficient datasets for it to continue.");

				if (source.isOfType<int32_t>()) {
					Eigen::ArrayXXi inPts;
					source.readWithEigen(inPts);
					shpPoints = inPts.cast<double>();
				}
				else if (source.isOfType<float>()) {
					Eigen::ArrayXXf inPts;
					source.readWithEigen(inPts);
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

			Algorithm algHbfMiniball1
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
			Algorithm algHbfMiniball2
			{ []()->Algorithm::AlgorithmConstructor {
				Algorithm::AlgorithmConstructor a;
				a.RequiredStructuralDatasets = { "particle_scattering_element_coordinates"};
				a.ProvidedAttributes = {
					"MaximumDimension_MiniballPoints",
					"SurfaceArea_MiniballPoints",
					"Volume_MiniballPoints",
					"Radius_MiniballPoints",
					"Center_MiniballPoints"
				};
				a.func = MiniballFunction;
				a.weight = 900;
				return a;
}() };

		}
	}
}

D_icedb_start()
{
	using namespace icedb::registry;
	using namespace icedb::plugins::hbf_miniball;
	// Tell icedb about the plugin.
	static const icedb::registry::DLLpreamble id(
		"plugin-hbf-miniball",
		"Miniball calculations",
		PLUGINID);
	dllInitResult res = icedb_registry_register_dll(id, (void*)dllStart);
	if (res != SUCCESS) return res;

	icedb::ShapeAlgs::Algorithms::common_algorithms.emplace(std::make_pair(
		std::string("HbfMiniball1"),
		icedb::plugins::hbf_miniball::algHbfMiniball1
	));
	icedb::ShapeAlgs::Algorithms::common_algorithms.emplace(std::make_pair(
		std::string("HbfMiniball2"),
		icedb::plugins::hbf_miniball::algHbfMiniball2
	));

	return SUCCESS;
}
