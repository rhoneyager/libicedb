#include "../../germany_api/germany/defs.h"
#include "plugin_ddscat.hpp"
#include "shape.h"

// From https://tinodidriksen.com/uploads/code/cpp/speed-string-to-int.cpp
inline int naiveAtoI(const char *p) {
	int x = 0;
	bool neg = false;
	if (*p == '-') {
		neg = true;
		++p;
	}
	while (*p >= '0' && *p <= '9') {
		x = (x * 10) + (*p - '0');
		++p;
	}
	if (neg) {
		x = -x;
	}
	return x;
}

void readShapeHeader(ICEDB_L0_DDSCAT_SHAPE_p p, const char* data, size_t dataSz, size_t &headerEnd) {
	using namespace std;
	
	const char* pend = data;
	const char* pstart = data;
	size_t np = 0;
	// The header is seven lines long
	for (size_t i = 0; i < 7; i++)
	{
		pstart = pend;
		// Using memchr for safety. Accepts a max length argument.
		pend = (const char*) memchr((void*)pend, '\n', dataSz);
		if (!pend) ICEDB_DEBUG_RAISE_EXCEPTION();
		pend++; // Get rid of the newline
				//pend = in.find_first_of("\n", pend+1);
		if (pend > data+dataSz) ICEDB_DEBUG_RAISE_EXCEPTION();
		string lin(pstart, pend - pstart - 1);
		if (*(lin.rbegin()) == '\r') lin.pop_back();

		size_t posa = 0, posb = 0;
		double v[3];
		//Eigen::Array3f *v = nullptr;
		switch (i)
		{
		case 0: // Title line
			p->_vptrs->setDescription(p, lin.c_str());
			break;
		case 1: // Number of dipoles
		{
			// Seek to first nonspace character
			posa = lin.find_first_not_of(" \t\n", posb);
			// Find first space after this position
			posb = lin.find_first_of(" \t\n", posa);
			size_t len = posb - posa;
			np = (size_t) naiveAtoI(&(lin.data()[posa]));
			p->_vptrs->resize(p, np);
		}
		break;
		case 6: // Junk line
		default:
			break;
		case 2: // a1
		case 3: // a2
		case 4: // d
		case 5: // x0
				// These all have the same structure. Read in three doubles, then assign.
		{
			for (size_t j = 0; j < 3; j++)
			{
				// Seek to first nonspace character
				posa = lin.find_first_not_of(" \t\n,", posb);
				// Find first space after this position
				posb = lin.find_first_of(" \t\n,", posa);
				size_t len = posb - posa;
				v[j] = atof(&(lin.data()[posa]));
			}
			if (i == 2) p->_vptrs->setA1(p, v);
			if (i == 3) p->_vptrs->setA2(p, v);
			if (i == 4) p->_vptrs->setD0(p, v);
			if (i == 5) p->_vptrs->setX0(p, v);
		}
		break;
		}
	}

	headerEnd = (pend - data) / sizeof(char);
}
// Function to read a shape.dat or target.out file.
// Already assumes that the file has been loaded into a char array.
// Will avoid buffered and formatted operations because these are rather slow.
void readShapeDat(ICEDB_L0_DDSCAT_SHAPE_p ptr, const char* data, size_t dataSz) {
	if (!ptr || !data) ICEDB_DEBUG_RAISE_EXCEPTION();
	// First seven lines are the header.
	size_t headerEnd = 0;
	readShapeHeader(ptr, data, dataSz, headerEnd);

	// Subsequent lines contain the space-separated data. Seven columns. Already know the
	// number of points from the header.
	size_t i = 0, mx = ptr->_vptrs->size(ptr) * 7;
	//p->ptArray; // Already preallocated by the readShapeHeader function.

	const char* p = data + headerEnd;
	const char* pmax = data + dataSz;
	while (i< mx) {
		int x = 0;
		// Advance to the next number
		while (*p < '0' && *p > '9') { if (*p == '\0' || p >= pmax) ICEDB_DEBUG_RAISE_EXCEPTION(); ++p; }
		while (*p >= '0' && *p <= '9') {
			x = (x * 10) + (*p - '0');
			++p;
		}
		ptr->ptArray[i] = x;
		++i;
	}
}
