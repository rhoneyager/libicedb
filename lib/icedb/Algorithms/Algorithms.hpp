#pragma once
#include "../defs.h"
#include "ShapeAlgs.hpp"
#include <map>
#include <string>

namespace icedb {
	namespace ShapeAlgs {
		namespace Algorithms {
			class Algorithm;
			namespace _internal_algs {
				/// A "dummy" algorithm that exists only as an example.
				extern Algorithm dummy;
				/// A "dummy" algorithm that exists only for testing.
				extern Algorithm dummy2;
				/// A trivial algorithm that copies info about the dimensions to the PPP block.
				extern Algorithm copy_to_ppp;
				/// Find points on the convex hull. Used to speed up many other algorithms.
				extern Algorithm ConvexHull;
				/// Find max distance between any two points.
				extern Algorithm MaxDistanceTwoPoints;
				/// Find the smallest circumscribing sphere (of a set of points).
				extern Algorithm SmallestCircumscribingSphere_Points;
			}

			namespace PPPnames {
				enum class Datasets {
					Dummy, // used
					Dummy2, // used
					ConvexHullPoints, // used
					ConvexHullTriangles,
					FurthestPoints_Points, // used
					InertiaTensor_MassCentered,
					ProjectedArea_CoordinateAxes,
					ProjectedArea_PrincipalAxes,
					ProjectedAxisRatio_CoordinateAxes_XY,
					ProjectedAxisRatio_CoordinateAxes_FittedEllipsoid,
					ProjectedAxisRatio_PrincipalAxes_XY,
					ProjectedAxisRatio_PrincipalAxes_FittedEllipsoid
				};
				enum class Attributes {
					MiniballPoints_Center, // used
					MiniballPoints_Diameter, // used
					MiniballPoints_Radius, // used
					MiniballPoints_SurfaceArea, // used
					MiniballPoints_Volume, // used
					MiniballPoints_VolumeFraction,
					MiniballPoints_EffectiveDensity,

					MiniballSpheres_Center,
					MiniballSpheres_Diameter,
					MiniballSpheres_Radius,
					MiniballSpheres_SurfaceArea,
					MiniballSpheres_Volume,
					MiniballSpheres_VolumeFraction,
					MiniballSpheres_EffectiveDensity,

					MinApproxEllipsoidPoints_Center,
					MinApproxEllipsoidPoints_Diameter,
					MinApproxEllipsoidPoints_Radius,
					MinApproxEllipsoidPoints_SurfaceArea,
					MinApproxEllipsoidPoints_Volume,
					MinApproxEllipsoidPoints_VolumeFraction,
					MinApproxEllipsoidPoints_EffectiveDensity,

					//ConvexHull_Center, // Have to define better
					ConvexHull_SurfaceArea,
					ConvexHull_Volume,
					ConvexHull_VolumeFraction,
					ConvexHull_EffectiveDensity,

					FurthestPoints_Distance, // used
					FurthestPointsCircumscribingSphere_Center,
					FurthestPointsCircumscribingSphere_Radius,
					FurthestPointsCircumscribingSphere_SurfaceArea,
					FurthestPointsCircumscribingSphere_Volume,
					FurthestPointsCircumscribingSphere_VolumeFraction,
					FurthestPointsCircumscribingSphere_EffectiveDensity,

					Mass_Center,
					Volume_Center,

					Mass_ElementwiseDensity,

					LengthUnits,
					LengthScaleFactor,
					MassUnits,
					MassScaleFactor
				};
				extern std::map<Datasets, std::string> DNames;
				extern std::map<Attributes, std::string> ANames;
			}
		}
	}
}
