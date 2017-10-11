#include "../icedb/data/attrs.h"
#include "../icedb/data/attrs.hpp"
#include "../icedb/fs/fs.h"
#include "../icedb/error/error_context.h"

ICEDB_BEGIN_DECL_C

bool validate_attr_ptr(const ICEDB_attr *attr) {
	if (!attr) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return false;
	}
	if (!attr->parent) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return false;
	}
	if (!attr->name) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return false;
	}
	if (!attr->dims) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return false;
	}
	if (!attr->funcs) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return false;
	}
	return true;
}
bool attr_close(ICEDB_attr *attr)
{
	if (!validate_attr_ptr(attr)) return false;
	delete[] attr->dims;
	delete[] attr->name;
	attr->dims = nullptr;
	attr->name = nullptr;

	ICEDB_funcs_fs.closeHandle(attr->parent);
	attr->parent = nullptr;
	
	delete attr;
	return true;
}
DL_ICEDB ICEDB_attr_close_f ICEDB_attr_close = attr_close;

bool attr_write(ICEDB_attr *attr) {
	if (!validate_attr_ptr(attr)) return false;
	return ICEDB_funcs_fs.attrs.writeAttrData(attr->parent, attr->name, attr->type, attr->numDims,
		attr->dims, attr->data.vt);
}
DL_ICEDB ICEDB_attr_write_f ICEDB_attr_write = attr_write;

ICEDB_attr* attr_copy(
	const ICEDB_attr *oldattr, 
	ICEDB_fs_hnd* newparent,
	const char* newname) {
	if (!validate_attr_ptr(oldattr)) return nullptr;
	if (!newparent) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return nullptr;
	}
	if (!newname) newname = oldattr->name;
	ICEDB_attr *res = ICEDB_funcs_fs.attrs.create(newparent, newname, oldattr->type, oldattr->numDims,
		oldattr->dims);
	if (!res) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return nullptr;
	}
	// There should NEVER be an error with copying memory here if the new object was correctly initialized.
	errno_t err = memcpy_s(res->data.ct, res->sizeBytes, oldattr->data.ct, oldattr->sizeBytes);
	if (err) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return nullptr;
	}
	return res;
}
DL_ICEDB ICEDB_attr_copy_f ICEDB_attr_copy = attr_copy;

/*
const char* attr_getName(const ICEDB_attr *attr) {
	if (!validate_attr_ptr(attr)) return nullptr;
	return attr->name;
}
DL_ICEDB ICEDB_attr_getName_f ICEDB_attr_getName = attr_getName;
*/
/*
/// \note Makes a copy of the handle, which must be freed by the caller.
ICEDB_fs_hnd* attr_getParent(const ICEDB_attr *attr) {
	if (!validate_attr_ptr(attr)) return nullptr;
	return ICEDB_funcs_fs.clone(attr->parent);
}
DL_ICEDB ICEDB_attr_getParent_f ICEDB_attr_getParent = attr_getParent;

ICEDB_DATA_TYPES attr_getType(const ICEDB_attr *attr) {
	if (!validate_attr_ptr(attr)) return ICEDB_DATA_TYPES::ICEDB_TYPE_NOTYPE;
	return attr->type;
}
DL_ICEDB ICEDB_attr_getType_f ICEDB_attr_getType = attr_getType;
*/
bool attr_resize(ICEDB_attr *attr, ICEDB_DATA_TYPES newtype, size_t newNumDims, const size_t* newDims) {
	if (!validate_attr_ptr(attr)) return false;
	if (!newDims || (newtype == ICEDB_DATA_TYPES::ICEDB_TYPE_NOTYPE) ){
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return false;
	}

	if (attr->dims) delete[] attr->dims;
	if (attr->data.ct) delete[] attr->data.ct;
	attr->sizeElems = 1;
	attr->numDims = newNumDims;
	attr->dims = new size_t[newNumDims];
	for (size_t i = 0; i < newNumDims; ++i) {
		attr->sizeElems *= newDims[i];
		attr->dims[i] = newDims[i];
	}
	attr->type = newtype;
	attr->sizeBytes = attr->sizeElems;
	switch (attr->type)
	{
	case ICEDB_TYPE_CHAR:
	case ICEDB_TYPE_INT8:
	case ICEDB_TYPE_UINT8:
		break;
	case ICEDB_TYPE_UINT16:
	case ICEDB_TYPE_INT16:
		attr->sizeBytes *= 2;
		break;
	case ICEDB_TYPE_UINT32:
	case ICEDB_TYPE_INT32:
	case ICEDB_TYPE_FLOAT:
		attr->sizeBytes *= 4;
		break;
	case ICEDB_TYPE_UINT64:
	case ICEDB_TYPE_INT64:
	case ICEDB_TYPE_DOUBLE:
		attr->sizeBytes *= 8;
		break;
	case ICEDB_TYPE_INTMAX:
	case ICEDB_TYPE_INTPTR:
	case ICEDB_TYPE_UINTMAX:
	case ICEDB_TYPE_UINTPTR:
		{
			ICEDB_error_context_create(ICEDB_ERRORCODES_UNIMPLEMENTED);
			return false;
		}
		break;
	case ICEDB_TYPE_NOTYPE:
	default:
		{
			ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
			return false;
		}
		break;
	}
	attr->data.ct = new char[attr->sizeBytes];
	memset(attr->data.ct, NULL, attr->sizeBytes);

	return true;
}
DL_ICEDB ICEDB_attr_resize_f ICEDB_attr_resize = attr_resize;


bool attr_setData(ICEDB_attr *attr, const void* indata, size_t indataByteSize) {
	if (!validate_attr_ptr(attr)) return false;
	memcpy_s(attr->data.ct, attr->sizeBytes, indata, indataByteSize);
	return true;
}
DL_ICEDB ICEDB_attr_setData_f ICEDB_attr_setData = attr_setData;


DL_ICEDB const struct ICEDB_attr_ftable ICEDB_funcs_attr_obj = {
	attr_close, attr_write, attr_copy, //attr_getParent,
	//attr_getType, 
	attr_resize, 
	attr_setData
};

ICEDB_attr* attr_create(ICEDB_fs_hnd* parent, const char* name, 
	ICEDB_DATA_TYPES type, size_t numDims, const size_t* dims)
{
	ICEDB_attr* res = new ICEDB_attr;
	res->parent = parent;
	size_t nameLen = strlen(name);
	res->name = new char[nameLen + 1];
	strcpy_s(res->name, nameLen + 1, name);
	res->dims = nullptr;
	res->data.ct = nullptr;
	res->numDims = 0;
	attr_resize(res, type, numDims, dims);
	return res;
}
DL_ICEDB ICEDB_attr_create_f ICEDB_attr_create = attr_create;

/// \todo Implement
ICEDB_attr* attr_open(ICEDB_fs_hnd* parent, const char* name)
{
	ICEDB_attr* res = new ICEDB_attr;
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return res;
}
DL_ICEDB ICEDB_attr_open_f ICEDB_attr_open = attr_open;

bool attr_remove(ICEDB_fs_hnd* parent, const char* name) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return false;
}
DL_ICEDB ICEDB_attr_remove_f ICEDB_attr_remove = attr_remove;

size_t attr_count(const ICEDB_fs_hnd* p, ICEDB_OUT ICEDB_error_code* err) {
	*err = 1;
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return 0;
}
DL_ICEDB ICEDB_attr_getNumAttrs_f ICEDB_attr_count = attr_count;

const char* attr_getName(const ICEDB_fs_hnd* p, size_t attrnum, size_t inPathSize,
	size_t *outPathSize, char **bufPath, ICEDB_free_charIPP_f *deallocator) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return nullptr;
}
DL_ICEDB ICEDB_attr_getName_f ICEDB_attr_getName = attr_getName;

bool attr_exists(const ICEDB_fs_hnd* parent, const char* name, ICEDB_OUT ICEDB_error_code* err) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return false;
}
DL_ICEDB ICEDB_attr_exists_f ICEDB_attr_exists = attr_exists;

bool attr_rename(ICEDB_fs_hnd* parent, const char* oldname, const char* newname) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return false;
}
DL_ICEDB ICEDB_attr_rename_f ICEDB_attr_rename = attr_rename;

bool attr_freeAttrList(ICEDB_attr ***p) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return false;
}
DL_ICEDB ICEDB_attr_freeAttrList_f ICEDB_attr_freeAttrList = attr_freeAttrList;

ICEDB_attr *** const attr_openAllAttrs(const ICEDB_fs_hnd* parent, size_t* numAttrs, ICEDB_attr *** list) {
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return nullptr;
}
DL_ICEDB ICEDB_attr_openAllAttrs_f ICEDB_attr_openAllAttrs = attr_openAllAttrs;

bool attr_fs_write_backend(
	ICEDB_fs_hnd* p, const char* name,
	ICEDB_DATA_TYPES type,
	size_t numDims,
	size_t *dims,
	void* data)
{
	ICEDB_DEBUG_RAISE_EXCEPTION();
	return false;
}
DL_ICEDB ICEDB_attr_fs_write_backend_f ICEDB_attr_fs_write_backend = attr_fs_write_backend;


DL_ICEDB const struct ICEDB_attr_container_ftable ICEDB_funcs_attr_container = {
	attr_close,
	attr_create,
	attr_open,
	attr_remove,
	attr_count,
	attr_getName,
	attr_exists,
	attr_rename,
	attr_freeAttrList,
	attr_openAllAttrs,
	attr_fs_write_backend
};

ICEDB_END_DECL_C
