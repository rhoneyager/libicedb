#include "../icedb/Group.hpp"
#include "../icedb/compat/hdf5_load.h"
#include "../icedb/hdf5_supplemental.hpp"
#include "../icedb/gsl/gsl_assert"

namespace icedb {
	namespace Groups {
		class Group::Group_impl {
			friend class Group;
			std::shared_ptr<H5::Group> grp;
		};

		Group::Group(const std::string &name) : name(name)
		{
			_impl = std::make_shared<Group_impl>();
			// Open the HDF5 group
			this->_setAttributeParent(_impl->grp);
		}
	}
}