#include "icedb/ShapeAlgs.hpp"
#include "icedb/Shapes.hpp"
#include "BetterThrow/Error.hpp"

namespace icedb {
	namespace ShapeAlgs {
		ICEDB_DL std::set<std::string>
			PPP_required_datasets,
			PPP_required_attributes{ "particle_maximum_dimension_standard" };

		namespace Algorithms {
			bool Algorithm::isValidTarget(const HH::Group& shape, const gsl::span<const HH::Group>& inPPPs) const
			{
				if (_rsd.size() && !shape.isGroup()) return false;
				if (_rsa.size() && !shape.isGroup()) return false;
				if (_dsd.size() && !shape.isGroup()) return false;
				if (_dsa.size() && !shape.isGroup()) return false;
				if (_rpa.size() && !inPPPs.size()) return false;
				if (_rpd.size() && !inPPPs.size()) return false;
				if (_dpa.size() && !inPPPs.size()) return false;
				if (_dpd.size() && !inPPPs.size()) return false;

				for (const auto& a : _rsa)
					if (!shape.atts.exists(a)) return false;
				for (const auto& d : _rsd)
					if (!shape.dsets.exists(d)) return false;
				for (const auto& a : _dsa)
					if (shape.atts.exists(a)) return false;
				for (const auto& d : _dsd)
					if (shape.dsets.exists(d)) return false;
				for (const auto& a : _rpa) {
					bool good = false;
					for (const auto& p : inPPPs) {
						if (p.atts.exists(a)) {
							good = true;
							continue;
						}
					}
					if (!good) return false;
				}
				for (const auto& d : _rpd) {
					bool good = false;
					for (const auto& p : inPPPs) {
						if (p.dsets.exists(d)) {
							good = true;
							continue;
						}
					}
					if (!good) return false;
				}
				for (const auto& a : _dpa)
					for (const auto& p : inPPPs)
						if (p.atts.exists(a)) return false;
				for (const auto& d : _dpd)
					for (const auto& p : inPPPs)
						if (p.dsets.exists(d)) return false;

				return true;
			}

			/// Define a common algorithm that sets a dummy property.
			/// No inputs needed.
			Algorithm dummy{ []()->Algorithm::AlgorithmConstructor {
				Algorithm::AlgorithmConstructor a;
				a.ProvidedAttributes = { "dummyVer" };
				a.ProvidedDatasets = { "dummyDset" };
				a.func = [](HH::Group res, const HH::Group & shp, gsl::span<const HH::Group> inPPPs)
				{
					res.atts.add<uint16_t>("dummyVer", 1);
					res.dsets.create<int32_t>("dummyDset", { 2 })
						.write<int32_t>({ 3,4 });
				};
				a.weight = 100;
				return a;
}() };

			/// Algorithm that determines the maximum particle diameter, in meters, which is set as an attribute.
			Algorithm maximum_distance_any_two_elements_ConvexHull{ []()->Algorithm::AlgorithmConstructor {
				Algorithm::AlgorithmConstructor a;
				a.ProvidedAttributes = { "MaximumDistanceAnyTwoElements" };
				a.RequiredPPPDatasets = { "ConvexHullPoints" };
				a.func = [](HH::Group res, const HH::Group & shp, const gsl::span<const HH::Group>& inPPPs)
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

					res.atts.add<float>("MaximumDistanceAnyTwoElements", (float) std::sqrt(maxSqDist));
				};
				a.weight = 40;
				return a;
}() };

			/// Algorithm that determines the maximum particle diameter, in meters, which is set as an attribute.
			Algorithm maximum_distance_any_two_elements_naive{ []()->Algorithm::AlgorithmConstructor {
				Algorithm::AlgorithmConstructor a;
				a.ProvidedAttributes = { "MaximumDistanceAnyTwoElements" };
				a.RequiredStructuralDatasets = {"particle_scattering_element_coordinates"};
				a.func = [](HH::Group res, const HH::Group & shp, const gsl::span<const HH::Group>& inPPPs)
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

					double maxSqDist = 0;
					for (int i = 0; i < shpPoints.rows(); ++i) {
						for (int j = i + 1; j < shpPoints.rows(); ++j) {
							Eigen::ArrayXd dist = shpPoints.block(i, 0, 1, 3) - shpPoints.block(j, 0, 1, 3);
							double distsq = dist.square().sum();
							if (distsq > maxSqDist) maxSqDist = distsq;
						}
					}

					res.atts.add<float>("MaximumDistanceAnyTwoElements", (float)std::sqrt(maxSqDist));
				};
				a.weight = 10000;
				return a;
}() };

			/// Propagate units into the PPP block
			Algorithm copy_units{ []()->Algorithm::AlgorithmConstructor {
				Algorithm::AlgorithmConstructor a;
				a.ProvidedAttributes = {
					"scattering_element_coordinates_units",
					"scattering_element_coordinates_scaling_factor"
				};
				a.RequiredStructuralAttributes = { 
					"scattering_element_coordinates_units",
					"scattering_element_coordinates_scaling_factor"
				};
				a.func = [](HH::Group res, const HH::Group & shp, const gsl::span<const HH::Group>& inPPPs)
				{
					auto FindAttribute = [&](const std::string & name) -> HH::Attribute {
						for (const auto& p : inPPPs)
						{
							if (p.atts.exists(name)) return p.atts[name];
						}
						throw BT_throw.add("Reason", "Invalid assumption: attribute does not exist in inPPPs")
							.add("Attribute", name);
					};

					HH::Attribute aUnits = FindAttribute("scattering_element_coordinates_units");
					HH::Attribute aScale = FindAttribute("scattering_element_coordinates_scaling_factor");

					float sf = aScale.read<float>();
					res.atts.add<float>("scattering_element_coordinates_scaling_factor", sf);

					std::string sUnits = aUnits.read<std::string>();
					res.atts.add<std::string>("scattering_element_coordinates_scaling_factor", sUnits);
				};
				a.weight = 10;
				return a;
}() };

			std::map<std::string, Algorithm> common_algorithms
			{
				{"dummy", dummy},
				{"maximum_distance_any_two_elements_ConvexHull", maximum_distance_any_two_elements_ConvexHull},
				{"maximum_distance_any_two_elements_naive", maximum_distance_any_two_elements_naive},
				{"copy_units", copy_units}
			};

			/*
					auto FindAttribute = [&](const std::string & name) -> HH::Attribute {
						for (const auto& p : inPPPs)
						{
							if (p.atts.exists(name)) return p.atts[name];
						}
						throw BT_throw.add("Reason", "Invalid assumption: attribute does not exist in inPPPs")
							.add("Attribute", name);
					};

					HH::Attribute aUnits = FindAttribute("scattering_element_coordinates_units");
					HH::Attribute aScale = FindAttribute("scattering_element_coordinates_scaling_factor");
					*/
		}
	}
}
