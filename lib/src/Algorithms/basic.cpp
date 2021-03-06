#include "icedb/Algorithms/ShapeAlgs.hpp"
#include "icedb/Algorithms/Algorithms.hpp"
#include "BetterThrow/Error.hpp"

namespace icedb {
	namespace ShapeAlgs {
		namespace Algorithms {
			namespace _internal_algs {
				Algorithm dummy{ []()->Algorithm::AlgorithmConstructor {
					Algorithm::AlgorithmConstructor a;
					a.ProvidedAttributes = { "dummyVer" };
					a.ProvidedDatasets = { "dummyDset" };
					a.func = [](HH::Group res, const HH::Group & , gsl::span<const HH::Group> )
					{
						res.atts.add<uint16_t>("dummyVer", 1);
						res.dsets.create<int32_t>("dummyDset", { 2 })
							.write<int32_t>({ 3,4 });
					};
					a.weight = 100;
					return a;
				}() };

				Algorithm dummy2{ []()->Algorithm::AlgorithmConstructor {
					Algorithm::AlgorithmConstructor a;
					a.ProvidedAttributes = { "dummyVer2" };
					a.ProvidedDatasets = { "dummyDset2" };
					a.func = [](HH::Group res, const HH::Group & shp, gsl::span<const HH::Group> inPPPs)
					{
						// This algorithm makes sure that the shape and inPPPs are valid objects.
						if (!shp.isGroup()) throw BT_throw.add("Reason", "Shape is invalid.");
						for (const auto& p : inPPPs)
						{
							if (!p.isGroup()) throw BT_throw.add("Reason", "PPP is invalid.");
						}
						res.atts.add<uint16_t>("dummyVer2", 1);
						res.dsets.create<int32_t>("dummyDset2", { 2 })
							.write<int32_t>({ 3,4 });
					};
					a.weight = 100;
					return a;
				}() };


				Algorithm copy_to_ppp{ []()->Algorithm::AlgorithmConstructor {
					Algorithm::AlgorithmConstructor a;
					a.ProvidedAttributes = {
						"scattering_element_coordinates_units",
						"scattering_element_coordinates_scaling_factor"
					};
					a.RequiredStructuralAttributes = {
						"scattering_element_coordinates_units",
						"scattering_element_coordinates_scaling_factor"
					};
					a.func = [](HH::Group res, const HH::Group & shp, const gsl::span<const HH::Group> & inPPPs)
					{
						auto FindAttribute = [&](const std::string & name) -> HH::Attribute {
							if (shp.atts.exists(name)) return shp.atts[name];
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
			}
		}
	}
}
