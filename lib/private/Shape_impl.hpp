#pragma once
#include "Group_impl.hpp"
#include "../icedb/shape.hpp"
namespace icedb {
	namespace Shapes {
#ifdef _MSC_FULL_VER
#pragma warning( push )
#pragma warning(disable:4250)
#endif
		class Shape_impl : virtual public Shape, virtual public Groups::Group_impl {
		public:
			Shape_impl(const std::string &id, Groups::Group::Group_HDF_shared_ptr grp);
			virtual ~Shape_impl();

		};
#ifdef _MSC_FULL_VER
#pragma warning( pop )
#endif
	}
}