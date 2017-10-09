#include "../icedb/level_1/candidate_shapeAlg.h"
#include "../icedb/data/attrs.h"
#include "../icedb/data/tables.h"
#include <stdlib.h>
#include <math.h>

// Here is an example implementation of a shape algorithm.
// It is a _really_ naive example that finds the maximum
// distance between any two points.
// It operates in O(N^2) time, and is unsuitable for more than ~300 points.
// A better algorithm bould precondition the inputs, by first determining 
// the convex 3-D hull of the points, and then using these hull points to
// feed into this algorithm. Basically, the most distant points in the shape
// must, by definition, lie on the convex hull. Such an algorithm would be 
// vastly faster.

// Older C standards (and the MSVC compiler) will not let me write these directly
// inside a ICEDB_L1_shape_alg_def structure.
const char* inNaiveTblNames[] = { "particle_scattering_element_coordinates", NULL };
const char* inNaiveAttNames[] = { "particle_scattering_element_spacing", NULL };
const char* outNaiveAttNames[] = { "particle_scattering_element_max_distance_m", NULL };
ICEDB_DATA_TYPES outTblTypes[] = { ICEDB_DATA_TYPES::ICEDB_TYPE_NOTYPE };
ICEDB_DATA_TYPES outAttTypes[] = { ICEDB_DATA_TYPES::ICEDB_TYPE_FLOAT };

bool NaiveAlg(const ICEDB_tbl** inTbls, const ICEDB_attr** inAtts,
	ICEDB_tbl**, ICEDB_attr **outA)
{
	const ICEDB_tbl* inPoints = inTbls[0];
	const float interLatticeSpacing_m = inAtts[0]->data.ft[0];
	const size_t numPoints = inPoints->dims[0];
	ICEDB_attr *outDist = outA[0];
	float *points = (float*)malloc(sizeof(float) * 3 * numPoints);
	if (!points) return false;
	inPoints->funcs->readFull(inPoints, points);
	// Points are read as x_0, y_0, z_0, x_1, y_1, z_1, x_2, y_2, z_2, ...

	float maxDistSq = 0;
	for (size_t i = 0; i < numPoints; ++i) {
		float *ix = points + (3 * i);
		for (size_t j = i + 1; j < numPoints; ++j) {
			float *jx = points + (3 * j);
			float distSq = ((*ix - *jx) * (*ix - *jx))
				+ (*(ix + 1) - *(jx + 1)) * (*(ix + 1) - *(jx + 1))
				+ (*(ix + 2) - *(jx + 2)) * (*(ix + 2) - *(jx + 2));
			if (distSq > maxDistSq) maxDistSq = distSq;
		}
	}
	free(points);
	const size_t outSz = 1;
	outDist->funcs->resize(outDist, ICEDB_DATA_TYPES::ICEDB_TYPE_FLOAT, 1, &outSz);
	*(outDist->data.ft) = sqrtf(maxDistSq);
	outDist->funcs->write(outDist);
	return true;
}

// Eventually, these algorithm definitions will be either included in the
// library directly, or will be registered using plugins to extend library
// functionality.
DL_ICEDB const struct ICEDB_L1_shape_alg_def alg_naive_max_distance = 
{
	// The algorithm name
	"alg_nieve_max_distance",
	// The description
	"Computes the maximum distance between any two scattering centers",
	// This is a very expensive algorithm, so it should really never be used
	10000,
	// The input table names. Must be NULL-terminated.
	inNaiveTblNames,
	// The input attribute names. Must be NULL-terminated.
	// The interlattice spacings are needed for this algorithm.
	inNaiveAttNames,
	// The output table names. No output tables.
	{ NULL },
	// The output attribute names.
	outNaiveAttNames,
	// Output table types
	outTblTypes,
	// Output attribute types
	outAttTypes,
	// The function that implements the algorithm.
	NaiveAlg
};
