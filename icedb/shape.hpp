#pragma once
#include "fs.hpp"

namespace icedb {
	namespace shapes {
		namespace impl {
			class Shape_impl;
		}

		// Common table and attribute names
		struct ShapeObjectNamed {
			std::string name;
			std::string description;
			bool isRequired;
		};
		extern const std::vector<ShapeObjectNamed> knownShapeTables;
		extern const std::vector<ShapeObjectNamed> knownShapeAttributes;

		class Shape : public fs::CanHaveAttributes, fs::CanHaveTables
		{
			std::unique_ptr<impl::Shape_impl> _impl;
		public:
			const std::string name;
			size_t getNumScatteringElements() const;
			Shape(fs::Group &owner, const std::string &name);
		};
	}
}