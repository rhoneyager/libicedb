#pragma once

#include "icedb/ShapeAlgs.hpp"

namespace icedb {
	namespace ShapeAlgs {
		namespace Algorithms {
			class Algorithm;
			namespace _internal_algs {
				/// A "dummy" algorithm that exists only as an example.
				extern Algorithm dummy;
				/// A trivial algorithm that copies info about the dimensions to the PPP block.
				extern Algorithm copy_to_ppp;
				/// Find points on the convex hull. Used to speed up many other algorithms.
				extern Algorithm ConvexHull;
				/// Find max distance between any two points.
				extern Algorithm MaxDistanceTwoPoints;
				/// Find the smallest circumscribing sphere (of a set of points).
				extern Algorithm SmallestCircumscribingSphere_Points;
			}
		}
	}
}
