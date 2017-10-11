#include "../libicedb/icedb/level_0/shape.h"
#include "../libicedb/icedb/data/attrs.h"
#include "../libicedb/icedb/data/tables.h"
#include "../libicedb/icedb/error/error.h"
#include "../libicedb/icedb/error/error_context.h"
#include "../libicedb/icedb/fs/fs_backend.hpp"
#include "../libicedb/icedb/misc/hash.h"
#include <memory>
#include <cassert>
ICEDB_BEGIN_DECL_C

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
		ICEDB_funcs_fs.closeHandle(shp->fsSelf);
		delete shp;
		return true;
	}
	return false;
}
DL_ICEDB const ICEDB_shape_close_f ICEDB_shape_close = shape_close;

ICEDB_fs_hnd* shape_getFSself(const ICEDB_L0_SHAPE_VOL_SPARSE* shp) {
	if (validateShapePtr(shp)) {
		return ICEDB_funcs_fs.cloneHandle(shp->fsSelf);
	}
	else return nullptr;
}
DL_ICEDB const ICEDB_shape_getFSPtr_f ICEDB_shape_getFSself = shape_getFSself;

/*
ICEDB_fs_hnd* shape_getFSparent(const ICEDB_L0_SHAPE_VOL_SPARSE* shp) {
	if (validateShapePtr(shp)) {
		return ICEDB_funcs_fs.clone(shp->fsParent);
	}
	else return nullptr;
}
DL_ICEDB const ICEDB_shape_getFSPtr_f ICEDB_shape_getFSparent = shape_getFSparent;
*/

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
DL_ICEDB const ICEDB_shape_getNumPoints_f ICEDB_shape_getNumPoints = shape_getNumPoints;

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
			ICEDB_HASH_t hash = ICEDB_HASH(data.get(), (int) (memSize * sizeof(float)));
			return (uint64_t)hash.low;
		}
		else {
			// Unable to get the number of points. The object is bad.
			ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
			return 0;
		}
	}
}
DL_ICEDB const ICEDB_shape_getID_f ICEDB_shape_getID = shape_getID;

ICEDB_shape* shape_copy_open(const ICEDB_L0_SHAPE_VOL_SPARSE* shp, ICEDB_fs_hnd* newparent) {
	if (!validateShapePtr(shp)) return nullptr;
	if (!newparent) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return nullptr;
	}
	// Create a new shape, and copy over all tables and attributes.
	ICEDB_shape* res = ICEDB_funcs_fs_shp.generate(newparent);

	// Copy all attributes
	ICEDB_error_code err = 0;
	size_t numAtts = 0;
	ICEDB_attr ** allAtts;
	ICEDB_funcs_fs.attrs.openAllAttrs(shp->fsSelf, &numAtts, &allAtts);
	for (size_t i = 0; i < numAtts; ++i) {
		std::shared_ptr<ICEDB_attr> newatt(
			ICEDB_funcs_attr_obj.copy((allAtts)[i], res->fsSelf, NULL),
			ICEDB_funcs_attr_obj.close);
		ICEDB_funcs_attr_obj.write(newatt.get());
	}
	ICEDB_funcs_fs.attrs.freeAttrList(&allAtts);

	// Copy all tables
	size_t numTbls = ICEDB_funcs_fs.tbls.count(shp->fsSelf, &err);
	for (size_t i = 0; i < numTbls; ++i) {
		char tblName[1000] = "";
		ICEDB_funcs_fs.tbls.getName(shp->fsSelf, i, 1000, NULL, (char**)(&tblName), NULL);
		std::shared_ptr<ICEDB_tbl> tbl(
			ICEDB_funcs_fs.tbls.open(shp->fsSelf, tblName),
			ICEDB_funcs_fs.tbls.close);
		// This gets freed pretty much instantly, but I don't want to
		// change the function definition, and I might want to slightly modify the table.
		std::shared_ptr<ICEDB_tbl> newtbl(
			tbl->funcs->copy(tbl.get(), res->fsSelf, nullptr),
			ICEDB_funcs_fs.tbls.close);
	}

	return res;
}
DL_ICEDB const ICEDB_shape_copy_open_f ICEDB_shape_copy_open = shape_copy_open;

bool shape_copy(const ICEDB_L0_SHAPE_VOL_SPARSE* shp, ICEDB_fs_hnd* newparent) {
	std::shared_ptr<ICEDB_shape> newshp(
		shape_copy_open(shp, newparent),
		shape_close
	);
	if (newshp) return true;
	return false;
}
DL_ICEDB const ICEDB_shape_copy_f ICEDB_shape_copy = shape_copy;

ICEDB_shape* shape_clone(const ICEDB_L0_SHAPE_VOL_SPARSE* src) {
	if (!validateShapePtr(src)) return nullptr;
	ICEDB_shape* res = new ICEDB_shape;
	res->fsSelf = src->fsSelf->_h->clone();
	res->funcs = &ICEDB_funcs_shp_obj;
	return res;
}
DL_ICEDB const ICEDB_shape_clone_f ICEDB_shape_clone = shape_clone;


ICEDB_shape* shape_generate(ICEDB_fs_hnd* objBackend) {
	ICEDB_shape* res = new ICEDB_shape;
	res->fsSelf = objBackend->_h->clone();
	res->funcs = &ICEDB_funcs_shp_obj;
	// Create some basic tables and attributes.
	const size_t typelen = 17;
	const char objtype[typelen] = "shape_vol_sparse";
	using namespace std;
	shared_ptr<ICEDB_attr> aType(ICEDB_funcs_fs.attrs.create(objBackend, "icedb_obj_type",
		ICEDB_DATA_TYPES::ICEDB_TYPE_CHAR, 1, &typelen), ICEDB_funcs_fs.attrs.close);
	memcpy_s(aType->data.ct, aType->sizeBytes, objtype, typelen);
	aType->funcs->write(aType.get());

	const size_t uidlen = 1;
	shared_ptr<ICEDB_attr> aID(ICEDB_funcs_fs.attrs.create(objBackend, "particle_id",
		ICEDB_DATA_TYPES::ICEDB_TYPE_UINT64, 1, &uidlen), ICEDB_funcs_fs.attrs.close);
	aID->data.ui64t[0] = 0;
	aID->funcs->write(aID.get());

	shared_ptr<ICEDB_attr> aSpacing(ICEDB_funcs_fs.attrs.create(objBackend, "particle_scattering_element_spacing",
		ICEDB_DATA_TYPES::ICEDB_TYPE_FLOAT, 1, &uidlen), ICEDB_funcs_fs.attrs.close);
	aSpacing->data.ft[0] = -1;
	aSpacing->funcs->write(aSpacing.get());

	// Tables
	const size_t tblPtDims[] = { 1, 3 };
	const float tblPtData[] = { -1, -1, -1 };
	shared_ptr<ICEDB_tbl> tCoords(ICEDB_funcs_fs.tbls.create(objBackend, "particle_scattering_element_coordinates",
		ICEDB_DATA_TYPES::ICEDB_TYPE_FLOAT, 2, tblPtDims), ICEDB_funcs_fs.tbls.close);
	tCoords->funcs->writeFull(tCoords.get(), tblPtData);

	const size_t tblPtConst[] = { 4 };
	shared_ptr<ICEDB_tbl> tConstName(ICEDB_funcs_fs.tbls.create(objBackend, "particle_constituent_name",
		ICEDB_DATA_TYPES::ICEDB_TYPE_CHAR, 1, tblPtConst), ICEDB_funcs_fs.tbls.close);
	const char* tblPtConstData = "Ice";
	tConstName->funcs->writeFull(tConstName.get(), tblPtConstData);

	const size_t tblPtConstFracsSz[] = { 1, 1 };
	const float tblPtConstFracsData[] = { 1.0f };
	shared_ptr<ICEDB_tbl> tConstFracs(ICEDB_funcs_fs.tbls.create(objBackend, "particle_scattering_element_composition",
		ICEDB_DATA_TYPES::ICEDB_TYPE_FLOAT, 2, tblPtConstFracsSz), ICEDB_funcs_fs.tbls.close);
	tConstFracs->funcs->writeFull(tConstFracs.get(), tblPtConstFracsData);

	return res;
}
DL_ICEDB const ICEDB_shape_generate_f ICEDB_shape_generate = shape_generate;

ICEDB_shape* shape_openPathSingle(const char* filename, ICEDB_file_open_flags flags) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return nullptr;
}
DL_ICEDB const ICEDB_shape_open_single_file_f ICEDB_shape_openPathSingle = shape_openPathSingle;

ICEDB_shape*** const openPathAll(const char* path, ICEDB_path_iteration pit, ICEDB_file_open_flags flags, size_t *numShapes) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return nullptr;
}
DL_ICEDB const ICEDB_shape_open_path_all_f ICEDB_shape_openPathAll = openPathAll;

void openPathAllFree(ICEDB_shape*** const shps) {
	delete shps;
	ICEDB_DEBUG_RAISE_EXCEPTION();
}
DL_ICEDB const ICEDB_shape_open_path_all_free_f ICEDB_shape_openPathAllFree = openPathAllFree;


DL_ICEDB const struct ICEDB_shp_ftable ICEDB_funcs_fs_shp = {
	shape_generate,
	shape_openPathSingle,
	openPathAll,
	openPathAllFree,
	shape_close
};

DL_ICEDB const struct ICEDB_L0_SHAPE_VOL_SPARSE_ftable ICEDB_funcs_shp_obj = {
	shape_close,
	shape_getFSself,
	shape_getNumPoints,
	shape_getID,
	shape_copy_open,
	shape_copy,
	shape_clone
};

ICEDB_END_DECL_C
