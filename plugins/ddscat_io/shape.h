#pragma once
#ifndef ICEDB_H_L0_DDSCAT_SHAPE
#define ICEDB_H_L0_DDSCAT_SHAPE
#include "../../libicedb/icedb/defs.h"
#include "../../libicedb/icedb/error/error.h"
#include "../../libicedb/icedb/misc/io.h"

struct ICEDB_L0_DDSCAT_SHAPE;
typedef ICEDB_L0_DDSCAT_SHAPE* ICEDB_L0_DDSCAT_SHAPE_p;

typedef ICEDB_error_code(*ICEDB_L0_DDSCAT_SHAPE_read_f)(ICEDB_L0_DDSCAT_SHAPE_p, ICEDB_io_object_handle_p);
typedef ICEDB_error_code(*ICEDB_L0_DDSCAT_SHAPE_write_f)(ICEDB_L0_DDSCAT_SHAPE_p, ICEDB_io_object_handle_p);
typedef ICEDB_error_code(*ICEDB_L0_DDSCAT_SHAPE_resize_f)(ICEDB_L0_DDSCAT_SHAPE_p, size_t np);
typedef size_t(*ICEDB_L0_DDSCAT_SHAPE_size_f)(ICEDB_L0_DDSCAT_SHAPE_p);
typedef void(*ICEDB_L0_DDSCAT_SHAPE_setDescription_f)(ICEDB_L0_DDSCAT_SHAPE_p, const char*);
typedef const char*(*ICEDB_L0_DDSCAT_SHAPE_getDescription_f)(ICEDB_L0_DDSCAT_SHAPE_p);
typedef size_t(*ICEDB_L0_DDSCAT_SHAPE_getDescriptionSize_f)(ICEDB_L0_DDSCAT_SHAPE_p);
typedef void(*ICEDB_L0_DDSCAT_SHAPE_setA1_f)(ICEDB_L0_DDSCAT_SHAPE_p, double*);
typedef void(*ICEDB_L0_DDSCAT_SHAPE_setA2_f)(ICEDB_L0_DDSCAT_SHAPE_p, double*);
typedef void(*ICEDB_L0_DDSCAT_SHAPE_setX0_f)(ICEDB_L0_DDSCAT_SHAPE_p, double*);
typedef void(*ICEDB_L0_DDSCAT_SHAPE_setD0_f)(ICEDB_L0_DDSCAT_SHAPE_p, double*);
typedef void(*ICEDB_L0_DDSCAT_SHAPE_getA1_f)(ICEDB_L0_DDSCAT_SHAPE_p, double*);
typedef void(*ICEDB_L0_DDSCAT_SHAPE_getA2_f)(ICEDB_L0_DDSCAT_SHAPE_p, double*);
typedef void(*ICEDB_L0_DDSCAT_SHAPE_getX0_f)(ICEDB_L0_DDSCAT_SHAPE_p, double*);
typedef void(*ICEDB_L0_DDSCAT_SHAPE_getD0_f)(ICEDB_L0_DDSCAT_SHAPE_p, double*);
// TODO: Add upconvert function to the regular library representation of shapes.

struct ICEDB_L0_DDSCAT_SHAPE_vtable {
	ICEDB_L0_DDSCAT_SHAPE_read_f read;
	ICEDB_L0_DDSCAT_SHAPE_write_f write;
	ICEDB_L0_DDSCAT_SHAPE_resize_f resize;
	ICEDB_L0_DDSCAT_SHAPE_size_f size;
	ICEDB_L0_DDSCAT_SHAPE_setDescription_f setDescription;
	ICEDB_L0_DDSCAT_SHAPE_getDescription_f getDescription;
	ICEDB_L0_DDSCAT_SHAPE_getDescriptionSize_f getDescriptionSize;
	ICEDB_L0_DDSCAT_SHAPE_setA1_f setA1;
	ICEDB_L0_DDSCAT_SHAPE_setA2_f setA2;
	ICEDB_L0_DDSCAT_SHAPE_setX0_f setX0;
	ICEDB_L0_DDSCAT_SHAPE_setD0_f setD0;
	ICEDB_L0_DDSCAT_SHAPE_getA1_f getA1;
	ICEDB_L0_DDSCAT_SHAPE_getA2_f getA2;
	ICEDB_L0_DDSCAT_SHAPE_getX0_f getX0;
	ICEDB_L0_DDSCAT_SHAPE_getD0_f getD0;
};

struct ICEDB_L0_DDSCAT_SHAPE {
	struct ICEDB_L0_DDSCAT_SHAPE_vtable *_vptrs;
	const char* description;
	double a1[3], a2[3], x0[3], d0[3];
	size_t numPoints;
	int *ptArray;
};

DL_ICEDB ICEDB_L0_DDSCAT_SHAPE_p ICEDB_L0_DDSCAT_SHAPE_generate();
DL_ICEDB void ICEDB_L0_DDSCAT_SHAPE_destroy(ICEDB_L0_DDSCAT_SHAPE_p);

#endif
