#include "../../germany_api/germany/defs.h"
#include "plugin_ddscat.hpp"
#include "../../germany_api/germany/level_0/ddscat/shape.h"
#include "../../germany_api/germany/dlls/linking.h"
#include "../../germany_api/germany/misc/util.h"
#include <vector>
#include <memory>
#include <cstring>
#include <cstdio>

void writeShapeDat(ICEDB_L0_DDSCAT_SHAPE_p p, char **out, size_t &dataSz) {
	// Write the shape.dat file.
	// Internally, guess the memory space required, and allocate chunks as needed. Then,
	// combine these when copying into the output buffer. The output buffer is allocated here,
	// and needs to be freed after use.
	if (!p) ICEDB_DEBUG_RAISE_EXCEPTION();
	if (!out) ICEDB_DEBUG_RAISE_EXCEPTION();
	const size_t lineSz = 80;
	const size_t chunkSz = (p->_vptrs->size(p) * lineSz) + 1000;
	std::vector<std::unique_ptr<char[]> > data;
	

	char *cbuf = (char*)ICEDB_malloc(sizeof(char)*chunkSz);
	if (cbuf) ICEDB_DEBUG_RAISE_EXCEPTION();
	data.push_back(std::unique_ptr<char[]>(cbuf));
	
	size_t i = 0;
	// Write the header
	//(cbuf, chunkSz - i, p->_vptrs->getDescription(p));

	auto checkHeader = [&cbuf, &data]() {

	};


	// Write header
	char *cur = out[0];
	
}