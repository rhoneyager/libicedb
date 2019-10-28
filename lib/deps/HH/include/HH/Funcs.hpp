#pragma once
#include "defs.hpp"
#include <string>
#include <vector>

namespace HH {
	HH_DL std::vector<std::string> splitPaths(const std::string& p);

	HH_DL std::string condensePaths(const std::vector<std::string>& p, size_t start = 0, size_t end = 0);
}
