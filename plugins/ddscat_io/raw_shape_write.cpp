#include "../../germany_api/germany/defs.h"
#include "plugin_ddscat.hpp"
#include "shape.h"
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
	std::shared_ptr< interface_ICEDB_core_util> iutil(create_ICEDB_core_util(icedb::plugins::ddscat_io::h), destroy_ICEDB_core_util);
	std::shared_ptr< interface_ICEDB_core_mem> imem(create_ICEDB_core_mem(icedb::plugins::ddscat_io::h), destroy_ICEDB_core_util);

	const size_t lineSz = 80;
	const size_t chunkSz = (p->_vptrs->size(p) * lineSz) + 1000;
	std::vector<std::unique_ptr<char[]> > data;
	
	char *cbuf = (char*)imem->malloc(imem.get(),sizeof(char)*chunkSz);
	if (cbuf) ICEDB_DEBUG_RAISE_EXCEPTION();
	data.push_back(std::unique_ptr<char[]>(cbuf));
	
	size_t i = 0;
	// Write the header
	i = iutil->strncpy_s(iutil.get(), cbuf, chunkSz - i,
		p->_vptrs->getDescription(p), p->_vptrs->getDescriptionSize(p));
	cbuf[i] = '\n'; ++i;
	i += snprintf(cbuf, chunkSz - i, "%d\t= Number of lattice points\n", p->_vptrs->size(p));
	double v[3];
	p->_vptrs->getA1(p, v);
	i += snprintf(cbuf, chunkSz - i, "%f\t%f\t%f\t= target vector a1 (in TF)\n", v[0], v[1], v[2]);
	p->_vptrs->getA2(p, v);
	i += snprintf(cbuf, chunkSz - i, "%f\t%f\t%f\t= target vector a2 (in TF)\n", v[0], v[1], v[2]);
	p->_vptrs->getD0(p, v);
	i += snprintf(cbuf, chunkSz - i, "%f\t%f\t%f\t= d_x/d d_y/d	d_z/d (normally 1 1 1)\n", v[0], v[1], v[2]);
	p->_vptrs->getX0(p, v);
	i += snprintf(cbuf, chunkSz - i, "%f\t%f\t%f\t= = X0(1-3) = location in lattice of target origin\n", v[0], v[1], v[2]);
	i += snprintf(cbuf, chunkSz - i, "\tNo.\tix\tiy\tiz\tICOMP(x, y, z)\n");

	// Write the points
	size_t n = 0;
	size_t mx = p->_vptrs->size(p) * 7;
	while (n < mx) {
		// Attempt to write the row into the current chunk.
		// If the bounds would be exceeded, then append a new chunk.
		size_t old_i = i;
		i += snprintf(cbuf, chunkSz - i, "\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n");
		if (chunkSz - i == 0) {
			// Write likely failed. Allocate a new chunk and repeat.
			cbuf[old_i] = '\0';
			cbuf = (char*)imem->malloc(imem.get(), sizeof(char)*chunkSz);
			if (cbuf) ICEDB_DEBUG_RAISE_EXCEPTION();
			data.push_back(std::unique_ptr<char[]>(cbuf));
			i = 0;
			i += snprintf(cbuf, chunkSz - i, "\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n");
		}
		n += 7;
	}

	// Concatenate all of the chunks
	dataSz = 0;
	for (const auto & s : data) dataSz += strlen(s.get());
	*out = (char*)imem->malloc(imem.get(), (sizeof(char)*dataSz) + 1);
	i = 0;
	for (const auto & s : data) {
		i += iutil->strncpy_s(iutil.get(), *out, dataSz - i, s.get(), strlen(s.get()));
	}
	
}
