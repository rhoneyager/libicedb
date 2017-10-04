#include "../libicedb/icedb/level_0/shape.h"
#include "../libicedb/icedb/data/attrs.h"
#include "../libicedb/icedb/data/tables.h"
#include "../libicedb/icedb/error/error.h"
#include "../libicedb/icedb/error/error_context.h"
#include "../libicedb/icedb/fs/fs_backend.hpp"

struct _ICEDB_L0_SHAPE_VOL_SPARSE_impl {
	ICEDB_fs_hnd_p fshnd;
	size_t numpts;
};

bool validateShapePtr(ICEDB_SHAPE_p shp) {
	if (!shp) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	if (!shp->_p) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	if (!shp->_p->fshnd) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	if (!shp->_vptrs) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	return true;
}

bool shape_destructor(ICEDB_SHAPE_p shp, ICEDB_OUT ICEDB_error_code* err) {
	bool good = false;
	if (validateShapePtr(shp)) {
		delete shp->_p;
		delete shp->_vptrs;
		delete shp;
		good = true;
	} else {
		*err = ICEDB_ERRORCODES_TODO;
		// ICEDB_error_context_create(ICEDB_ERRORCODES_TODO); // Context already created
	}
	return good;
}

ICEDB_fs_hnd_p shape_getBackendPtr(ICEDB_SHAPE_p shp) {
	if (validateShapePtr(shp)) {
		return shp->_p->fshnd->_h->clone();
	}
	else return nullptr;
}

bool shape_setDesc(ICEDB_SHAPE_p shp, const char* desc) {
	if (validateShapePtr(shp)) {
		ICEDB_ATTR_p attr = ICEDB_ATTR_create(shp->_p->fshnd, "description", ICEDB_TYPE_CHAR, strlen(desc) + 1, true);
		strncpy(attr->data.ct, desc, strlen(desc) + 1);
		ICEDB_ATTR_write(attr);
		ICEDB_ATTR_close(attr);
	}
	else return false;
}

size_t shape_getNumPoints(ICEDB_SHAPE_p shp) {
	if (validateShapePtr(shp)) {
		// Query the underlying fs plugin
		if (shp->_p->numpts) return shp->_p->numpts;
		else {
			ICEDB_error_code err = 0;
			ICEDB_fs_hnd_p h = shp->_p->fshnd; // shape_getBackendPtr(shp);
			if (h) {
				bool res = ICEDB_ATTR_attrExists(h, "particle_scattering_element_number", &err);
				if (res) {
					ICEDB_ATTR_p attr = ICEDB_ATTR_open(h, "particle_scattering_element_number");
					if (attr) {
						shp->_p->numpts = attr->data.ui64t[0];
						ICEDB_ATTR_close(attr);
					}
				}
			}
		}
		return shp->_p->numpts;
	}
	else return 0;
}

bool shape_getID(ICEDB_SHAPE_p shp, ICEDB_OUT uint64_t* id) {
	if (validateShapePtr(shp)) {
		*id = 0;
	}
	else return false;
}

