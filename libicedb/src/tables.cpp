#include "../icedb/data/tables.h"
#include "../icedb/fs/fs.h"
#include "../icedb/error/error_context.h"
#include <memory>

bool validate_tbl_ptr(const ICEDB_tbl *tbl) {
	if (!tbl) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return false;
	}
	if (!tbl->funcs || !tbl->parent || !tbl->self || !tbl->dims || !tbl->name) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return false;
	}
	return true;
}

bool tbl_close(ICEDB_tbl *tbl)
{
	if (!validate_tbl_ptr(tbl)) return false;
	delete[] tbl->dims;
	delete[] tbl->name;
	// Release the self and parent pointers
	ICEDB_funcs_fs.close(tbl->self);
	ICEDB_funcs_fs.close(tbl->parent);
	tbl->dims = nullptr;
	tbl->name = nullptr;
	tbl->self = nullptr;
	tbl->parent = nullptr;

	delete tbl;
	return true;
}
DL_ICEDB ICEDB_tbl_close_f ICEDB_tbl_close = tbl_close;

ICEDB_tbl* tbl_copy(const ICEDB_tbl *srctbl,
	ICEDB_fs_hnd* newparent,
	ICEDB_OPTIONAL const char* newname)
{
	if (!validate_tbl_ptr(srctbl)) return false;
	if (!newname) newname = srctbl->name;
	ICEDB_tbl *res = ICEDB_funcs_fs.tbls.create(newparent, newname,
		srctbl->type, srctbl->numDims, srctbl->dims);
	if (!res) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_NULLPTR);
		return nullptr;
	}
	size_t sizeElems = 1;
	for (size_t i = 0; i < srctbl->numDims; ++i) {
		sizeElems *= srctbl->dims[i];
	}
	size_t sizeBytes = sizeElems;
	switch (srctbl->type)
	{
	case ICEDB_TYPE_CHAR:
	case ICEDB_TYPE_INT8:
	case ICEDB_TYPE_UINT8:
		break;
	case ICEDB_TYPE_UINT16:
	case ICEDB_TYPE_INT16:
		sizeBytes *= 2;
		break;
	case ICEDB_TYPE_UINT32:
	case ICEDB_TYPE_INT32:
	case ICEDB_TYPE_FLOAT:
		sizeBytes *= 4;
		break;
	case ICEDB_TYPE_UINT64:
	case ICEDB_TYPE_INT64:
	case ICEDB_TYPE_DOUBLE:
		sizeBytes *= 8;
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
	std::unique_ptr<char[]> data(new char[sizeBytes]);
	if (!srctbl->funcs->readFull(srctbl, data.get())) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		res->funcs->close(res);
		return nullptr;
	}
	if (!res->funcs->writeFull(res, data.get())) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		res->funcs->close(res);
		return nullptr;
	}
	return res;
}
DL_ICEDB ICEDB_tbl_copy_f ICEDB_tbl_copy = tbl_copy;

