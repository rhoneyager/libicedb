#pragma once
#include "shape.hpp"
namespace icedb {
	namespace Examples {
		namespace Shapes {
			// Public functions
			ShapeDataBasic readTextFile(const std::string &filename);
			ShapeDataBasic readPSUfile(const std::string &filename);

			// Internals
			ShapeDataBasic readDDSCAT(const char* in);
			void readHeader(const char* in, std::string &desc, size_t &np, size_t &headerEnd);
			void readDDSCATtextContents(const char *iin, size_t numExpectedPoints, size_t headerEnd, ShapeDataBasic& p);
			ShapeDataBasic readRawText(const char *iin);

		}
	}
}
