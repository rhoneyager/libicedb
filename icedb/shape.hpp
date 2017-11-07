#pragma once
#include "fs.hpp"
#include "Table.hpp"
#include "Attribute.hpp"
//#include "Groups.hpp"

namespace icedb {
	namespace shapes {
		// Common table and attribute names
		struct ShapeObjectNamed {
			std::string name;
			std::string description;
			bool isRequired;
		};
		extern const std::vector<ShapeObjectNamed> knownShapeTables;
		extern const std::vector<ShapeObjectNamed> knownShapeAttributes;

		class Shape : public Attributes::CanHaveAttributes, Tables::CanHaveTables
		{
			class Shape_impl;
			std::shared_ptr<Shape_impl> _impl;
		public:
			const std::string name;
			size_t getNumScatteringElements() const;
			Shape(Groups::Group &owner, const std::string &name);
		};
	}
}