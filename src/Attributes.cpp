#include "../icedb/Attribute.hpp"
#include "../icedb/compat/hdf5_load.h"
#include "../icedb/hdf5_supplemental.hpp"

namespace icedb {
	namespace Attributes {
		struct Attribute_impl {};

		class CanHaveAttributes::CanHaveAttributes_impl{

		};

		CanHaveAttributes::CanHaveAttributes(std::shared_ptr<H5::H5Object> obj) {}

		void CanHaveAttributes::deleteAttribute(const std::string &attributeName) {}

		bool CanHaveAttributes::doesAttributeExist(const std::string &attributeName) const { return false; }

		std::vector<std::string> CanHaveAttributes::getAttributeNames() const { return std::vector<std::string>(); }
		//type_info CanHaveAttributes::getAttributeTypeId(const std::string &attributeName) const { 
		//	return typeid(std::string); }
	}
}