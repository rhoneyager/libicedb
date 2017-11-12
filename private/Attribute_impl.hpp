#pragma once
#include "../icedb/Attribute.hpp"

namespace icedb {
	namespace Attributes {
		class CanHaveAttributes_impl : virtual public CanHaveAttributes {
			std::shared_ptr<H5::H5Object> parent;
		protected:
			virtual void _setAttributeParent(std::shared_ptr<H5::H5Object> obj) override;
			virtual std::shared_ptr<H5::H5Object> _getAttributeParent() const override;
		public:
			CanHaveAttributes_impl(std::shared_ptr<H5::H5Object>);
			CanHaveAttributes_impl();
			virtual ~CanHaveAttributes_impl();
		};
	}
}
