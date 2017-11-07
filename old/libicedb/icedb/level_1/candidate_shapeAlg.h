#pragma once
#ifndef ICEDB_H_SHAPEALGS_CANDIDATE
#define ICEDB_H_SHAPEALGS_CANDIDATE
#include <stdint.h>
#include "../defs.h"

struct ICEDB_tbl;
struct ICEDB_attr;

/** \brief The actual algorithm function
*
* The icedb library will handle all of the boilerplate
* behind this function call. The ICEDB_L1_shape_alg_def
* structure lists the input and output tables and attributes.
* Tables and attributes are passed into this function in the
* order in which they are defined in inTbls, inAtts, outTbls
* and outAtts. The output objects are pre-allocated, so the
* algorithm function only has to call their respective resize
* functions before entering data.
*
* \returns false upon failure (consult error system), true upon success.
* \param inT is an array of input tables. These tables can be lists of
*	scattering elements, lists of substances, scattering element radii,
*	et cetera. The ordering of the input tables matches this structure's
*	ordering of the objects in inTbls.
* \param inA is an array of input attributes.
* \param outT is an array of pre-allocated output tables, with names
*	matching outTbls and types matching outTblTypes.
* \param outA is an array of pre-allocated output attributes, with
*	names matching outAtts and types matching outAttrTypes.
**/
typedef bool(*algFunction_t)(
	const ICEDB_tbl** inT,
	const ICEDB_attr** inA,
	ICEDB_tbl ** outT,
	ICEDB_attr ** outA);

/** \brief This defines an algorithm that processes particle
*	structural data and adds value.
**/
struct ICEDB_L1_shape_alg_def {
	const char* name; ///< The name of the algorithm
	const char* description; ///< A description of the algorithm
	uint16_t cost; ///< A representation of the computational 'cost' of the algorithm
	// The algorithm inputs - i.e. what the algorithm needs to work
	/// Input table names. A NULL-terminated array.
	const char **inTbls;
	/// Input attribute names. A NULL-terminated array.
	const char **inAtts;
	/// Output table names. A NULL-terminated array.
	const char **outTbls;
	/// Output attribute names. A NULL-terminated array.
	const char **outAtts;
	/// Output table object types. NULL-terminated.
	ICEDB_DATA_TYPES *outTblTypes, *outAttrTypes;

	algFunction_t algFunction;
};

#endif