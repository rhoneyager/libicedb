#include "../icedb/data/tables.h"
#include "../icedb/data/attrs.h"
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

	// Also, copy all attributes!
	size_t numAtts = 0;
	ICEDB_attr ** allAtts;
	ICEDB_funcs_fs.attrs.openAllAttrs(srctbl->self, &numAtts, &allAtts);
	for (size_t i = 0; i < numAtts; ++i) {
		std::shared_ptr<ICEDB_attr> newatt(
			ICEDB_funcs_attr_obj.copy((allAtts)[i], newparent, NULL),
			ICEDB_funcs_attr_obj.close);
		ICEDB_funcs_attr_obj.write(newatt.get());
	}
	ICEDB_funcs_fs.attrs.freeAttrList(&allAtts);
	return res;
}
DL_ICEDB ICEDB_tbl_copy_f ICEDB_tbl_copy = tbl_copy;

bool tbl_readMapped(
	const ICEDB_tbl* tbl,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const ptrdiff_t *imapp,
	ICEDB_OUT void* out)
{
	if (!validate_tbl_ptr(tbl)) return false;
	return ICEDB_funcs_fs.tbls._inner_tbl_readMapped(tbl->self,
		start, count, stride, imapp, out);
}
DL_ICEDB ICEDB_tbl_readMapped_f ICEDB_tbl_readMapped = tbl_readMapped;

bool tbl_writeMapped(
	ICEDB_tbl* tbl,
	const size_t *start,
	const size_t *count,
	const ptrdiff_t *stride,
	const ptrdiff_t *imapp,
	const void* in)
{
	if (!validate_tbl_ptr(tbl)) return false;
	return ICEDB_funcs_fs.tbls._inner_tbl_writeMapped(tbl->self,
		start, count, stride, imapp, in);
}
DL_ICEDB ICEDB_tbl_writeMapped_f ICEDB_tbl_writeMapped = tbl_writeMapped;

bool tbl_readSingle(
	const ICEDB_tbl* tbl,
	const size_t *index,
	ICEDB_OUT void* out)
{
	if (!validate_tbl_ptr(tbl)) return false;
	if (tbl->numDims == 0) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	std::unique_ptr<size_t[]> unitvec(new size_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitstride(new ptrdiff_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitmap(new ptrdiff_t[tbl->numDims]);
	for (size_t i = 0; i < tbl->numDims; ++i) {
		unitvec[i] = 1;
		unitstride[i] = 1;
	}
	
	ptrdiff_t runmap = 1;
	for (size_t i = tbl->numDims; i > 0; i--) {
		unitmap[i-1] = runmap;
		runmap *= tbl->dims[i - 1];
	}
	return ICEDB_funcs_fs.tbls._inner_tbl_readMapped(tbl->self,
		index, unitvec.get(), unitstride.get(), unitmap.get(), out);
}
DL_ICEDB ICEDB_tbl_readSingle_f ICEDB_tbl_readSingle = tbl_readSingle;

bool tbl_readArray(
	const ICEDB_tbl* tbl,
	const size_t *index,
	const size_t *count,
	ICEDB_OUT void* out)
{
	if (!validate_tbl_ptr(tbl)) return false;
	if (tbl->numDims == 0) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	//std::unique_ptr<size_t[]> unitvec(new size_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitstride(new ptrdiff_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitmap(new ptrdiff_t[tbl->numDims]);
	for (size_t i = 0; i < tbl->numDims; ++i) {
		//unitvec[i] = 1;
		unitstride[i] = 1;
	}

	ptrdiff_t runmap = 1;
	for (size_t i = tbl->numDims; i > 0; i--) {
		unitmap[i - 1] = runmap;
		runmap *= tbl->dims[i - 1];
	}
	return ICEDB_funcs_fs.tbls._inner_tbl_readMapped(tbl->self,
		index, count, unitstride.get(), unitmap.get(), out);
}
DL_ICEDB ICEDB_tbl_readArray_f ICEDB_tbl_readArray = tbl_readArray;

bool tbl_readStride(
	const ICEDB_tbl* tbl,
	const size_t *index,
	const size_t *count,
	const ptrdiff_t *stride,
	ICEDB_OUT void* out)
{
	if (!validate_tbl_ptr(tbl)) return false;
	if (tbl->numDims == 0) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	std::unique_ptr<ptrdiff_t[]> unitmap(new ptrdiff_t[tbl->numDims]);

	ptrdiff_t runmap = 1;
	for (size_t i = tbl->numDims; i > 0; i--) {
		unitmap[i - 1] = runmap;
		runmap *= tbl->dims[i - 1];
	}
	return ICEDB_funcs_fs.tbls._inner_tbl_readMapped(tbl->self,
		index, count, stride, unitmap.get(), out);
}
DL_ICEDB ICEDB_tbl_readStride_f ICEDB_tbl_readStride = tbl_readStride;

bool tbl_readFull(
	const ICEDB_tbl* tbl,
	ICEDB_OUT void* out)
{
	if (!validate_tbl_ptr(tbl)) return false;
	if (tbl->numDims == 0) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	std::unique_ptr<size_t[]> index(new size_t[tbl->numDims]);
	std::unique_ptr<size_t[]> unitvec(new size_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitstride(new ptrdiff_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitmap(new ptrdiff_t[tbl->numDims]);
	for (size_t i = 0; i < tbl->numDims; ++i) {
		index[i] = 0;
		unitvec[i] = 1;
		unitstride[i] = 1;
	}

	ptrdiff_t runmap = 1;
	for (size_t i = tbl->numDims; i > 0; i--) {
		unitmap[i - 1] = runmap;
		runmap *= tbl->dims[i - 1];
	}
	return ICEDB_funcs_fs.tbls._inner_tbl_readMapped(tbl->self,
		index.get(), tbl->dims, unitstride.get(), unitmap.get(), out);
}
DL_ICEDB ICEDB_tbl_readFull_f ICEDB_tbl_readFull = tbl_readFull;

bool tbl_writeSingle(
	ICEDB_tbl* tbl,
	const size_t *index,
	const void* out)
{
	if (!validate_tbl_ptr(tbl)) return false;
	if (tbl->numDims == 0) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	std::unique_ptr<size_t[]> unitvec(new size_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitstride(new ptrdiff_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitmap(new ptrdiff_t[tbl->numDims]);
	for (size_t i = 0; i < tbl->numDims; ++i) {
		unitvec[i] = 1;
		unitstride[i] = 1;
	}

	ptrdiff_t runmap = 1;
	for (size_t i = tbl->numDims; i > 0; i--) {
		unitmap[i - 1] = runmap;
		runmap *= tbl->dims[i - 1];
	}
	return ICEDB_funcs_fs.tbls._inner_tbl_writeMapped(tbl->self,
		index, unitvec.get(), unitstride.get(), unitmap.get(), out);
}
DL_ICEDB ICEDB_tbl_writeSingle_f ICEDB_tbl_writeSingle = tbl_writeSingle;

bool tbl_writeArray(
	ICEDB_tbl* tbl,
	const size_t *index,
	const size_t *count,
	const void* out)
{
	if (!validate_tbl_ptr(tbl)) return false;
	if (tbl->numDims == 0) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	//std::unique_ptr<size_t[]> unitvec(new size_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitstride(new ptrdiff_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitmap(new ptrdiff_t[tbl->numDims]);
	for (size_t i = 0; i < tbl->numDims; ++i) {
		//unitvec[i] = 1;
		unitstride[i] = 1;
	}

	ptrdiff_t runmap = 1;
	for (size_t i = tbl->numDims; i > 0; i--) {
		unitmap[i - 1] = runmap;
		runmap *= tbl->dims[i - 1];
	}
	return ICEDB_funcs_fs.tbls._inner_tbl_writeMapped(tbl->self,
		index, count, unitstride.get(), unitmap.get(), out);
}
DL_ICEDB ICEDB_tbl_writeArray_f ICEDB_tbl_writeArray = tbl_writeArray;

bool tbl_writeStride(
	ICEDB_tbl* tbl,
	const size_t *index,
	const size_t *count,
	const ptrdiff_t *stride,
	const void* out)
{
	if (!validate_tbl_ptr(tbl)) return false;
	if (tbl->numDims == 0) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	std::unique_ptr<ptrdiff_t[]> unitmap(new ptrdiff_t[tbl->numDims]);

	ptrdiff_t runmap = 1;
	for (size_t i = tbl->numDims; i > 0; i--) {
		unitmap[i - 1] = runmap;
		runmap *= tbl->dims[i - 1];
	}
	return ICEDB_funcs_fs.tbls._inner_tbl_writeMapped(tbl->self,
		index, count, stride, unitmap.get(), out);
}
DL_ICEDB ICEDB_tbl_writeStride_f ICEDB_tbl_writeStride = tbl_writeStride;

bool tbl_writeFull(
	ICEDB_tbl* tbl,
	const void* out)
{
	if (!validate_tbl_ptr(tbl)) return false;
	if (tbl->numDims == 0) {
		ICEDB_error_context_create(ICEDB_ERRORCODES_TODO);
		return false;
	}
	std::unique_ptr<size_t[]> index(new size_t[tbl->numDims]);
	std::unique_ptr<size_t[]> unitvec(new size_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitstride(new ptrdiff_t[tbl->numDims]);
	std::unique_ptr<ptrdiff_t[]> unitmap(new ptrdiff_t[tbl->numDims]);
	for (size_t i = 0; i < tbl->numDims; ++i) {
		index[i] = 0;
		unitvec[i] = 1;
		unitstride[i] = 1;
	}

	ptrdiff_t runmap = 1;
	for (size_t i = tbl->numDims; i > 0; i--) {
		unitmap[i - 1] = runmap;
		runmap *= tbl->dims[i - 1];
	}
	return ICEDB_funcs_fs.tbls._inner_tbl_writeMapped(tbl->self,
		index.get(), tbl->dims, unitstride.get(), unitmap.get(), out);
}
DL_ICEDB ICEDB_tbl_writeFull_f ICEDB_tbl_writeFull = tbl_writeFull;


DL_ICEDB const struct ICEDB_tbl_ftable ICEDB_funcs_tbl_obj = {
	tbl_close,
	tbl_copy,
	tbl_readSingle,
	tbl_readMapped,
	tbl_readArray,
	tbl_readStride,
	tbl_readFull,
	tbl_writeSingle,
	tbl_writeMapped,
	tbl_writeArray,
	tbl_writeStride,
	tbl_writeFull
};
