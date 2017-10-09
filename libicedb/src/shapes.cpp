#include "../libicedb/icedb/level_0/shape.h"
#include "../libicedb/icedb/data/attrs.h"
#include "../libicedb/icedb/data/tables.h"
#include "../libicedb/icedb/error/error.h"
#include "../libicedb/icedb/error/error_context.h"
#include "../libicedb/icedb/fs/fs_backend.hpp"
#include "../libicedb/icedb/misc/hash.h"
#include <memory>
#include <cassert>

bool validateShapePtr(const ICEDB_L0_SHAPE_VOL_SPARSE* shp) {
	if (!shp) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	if (!shp->fsSelf) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	if (!shp->funcs) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	return true;
}

bool shape_close(ICEDB_L0_SHAPE_VOL_SPARSE* shp) {
	if (validateShapePtr(shp)) {
		//delete shp->_p;
		ICEDB_funcs_fs.close(shp->fsSelf);
		delete shp;
		return true;
	}
	return false;
}
DL_ICEDB ICEDB_shape_close_f ICEDB_shape_close = shape_close;

ICEDB_fs_hnd* shape_getFSself(const ICEDB_L0_SHAPE_VOL_SPARSE* shp) {
	if (validateShapePtr(shp)) {
		return ICEDB_funcs_fs.clone(shp->fsSelf);
	}
	else return nullptr;
}
DL_ICEDB ICEDB_shape_getFSPtr_f ICEDB_shape_getFSself = shape_getFSself;

uint64_t shape_getNumPoints(const ICEDB_L0_SHAPE_VOL_SPARSE* shp) {
	if (!validateShapePtr(shp)) return 0;

	ICEDB_error_code err = 0; // Not really used here. I only care if objects exist, not if they cannot be read due to an error.
	bool exists = ICEDB_funcs_fs.attrs.exists(shp->fsSelf, "particle_scattering_element_number", &err);
	if (exists) {
		std::shared_ptr<ICEDB_attr> aNumScatt(
			ICEDB_funcs_fs.attrs.open(shp->fsSelf, "particle_scattering_element_number"),
			ICEDB_funcs_fs.attrs.close);
		assert(aNumScatt->type == ICEDB_DATA_TYPES::ICEDB_TYPE_UINT64);
		assert(aNumScatt->sizeElems == 1);
		return aNumScatt->data.ui64t[0];
	} else {
		// Someone forgot to set this attribute. It is easy to calculate from the
		// "particle_scattering_element_coordinates" table.
		exists = ICEDB_funcs_fs.tbls.exists(shp->fsSelf, "particle_scattering_element_coordinates", &err);
		if (exists) {
			std::shared_ptr<ICEDB_tbl> tSEC(
				ICEDB_funcs_fs.tbls.open(shp->fsSelf, "particle_scattering_element_coordinates"),
				ICEDB_funcs_fs.tbls.close);
			assert(tSEC->numDims == 2);
			// Ordering is rows, cols
			return (uint64_t) tSEC->dims[0];
		} else {
			// Unable to get the number of points. The object is bad.
			ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
			return 0;
		}
	}

}
DL_ICEDB ICEDB_shape_getNumPoints_f ICEDB_shape_getNumPoints = shape_getNumPoints;

uint64_t shape_getID(const ICEDB_L0_SHAPE_VOL_SPARSE* shp) {
	if (!validateShapePtr(shp)) return 0;
	ICEDB_error_code err = 0; // Not really used here. I only care if objects exist, not if they cannot be read due to an error.
	bool exists = ICEDB_funcs_fs.attrs.exists(shp->fsSelf, "particle_id", &err);
	if (exists) {
		std::shared_ptr<ICEDB_attr> aNumScatt(
			ICEDB_funcs_fs.attrs.open(shp->fsSelf, "particle_id"),
			ICEDB_funcs_fs.attrs.close);
		assert(aNumScatt->type == ICEDB_DATA_TYPES::ICEDB_TYPE_UINT64);
		assert(aNumScatt->sizeElems == 1);
		return aNumScatt->data.ui64t[0];
	}
	else {
		// Someone forgot to set this attribute.
		// By default, calculate from the "particle_scattering_element_coordinates" table.
		exists = ICEDB_funcs_fs.tbls.exists(shp->fsSelf, "particle_scattering_element_coordinates", &err);
		if (exists) {
			std::shared_ptr<ICEDB_tbl> tSEC(
				ICEDB_funcs_fs.tbls.open(shp->fsSelf, "particle_scattering_element_coordinates"),
				ICEDB_funcs_fs.tbls.close);
			assert(tSEC->numDims == 2);
			// Ordering is rows, cols
			assert(tSEC->type == ICEDB_DATA_TYPES::ICEDB_TYPE_FLOAT);
			size_t memSize = tSEC->dims[0] * tSEC->dims[1];
			std::unique_ptr<float[]> data(new float[memSize]);
			tSEC->funcs->readFull(tSEC.get(), data.get());
			ICEDB_HASH_t hash = ICEDB_HASH(data.get(), memSize * sizeof(float));
			return (uint64_t)hash.low;
		}
		else {
			// Unable to get the number of points. The object is bad.
			ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
			return 0;
		}
	}
}
DL_ICEDB ICEDB_shape_getID_f ICEDB_shape_getID = shape_getID;

ICEDB_shape* shape_copy_open(const ICEDB_L0_SHAPE_VOL_SPARSE* shp, ICEDB_fs_hnd* newparent) {

}
DL_ICEDB ICEDB_shape_copy_open_f ICEDB_shape_copy_open = shape_copy_open;
