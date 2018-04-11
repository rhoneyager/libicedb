#pragma once
#include "shape.hpp"
namespace icedb {
	namespace Examples {
		namespace Shapes {
			ShapeDataBasic readTextFile(const std::string &filename);
			ShapeDataBasic readPSUfile(const std::string &filename);
		}
	}
}
