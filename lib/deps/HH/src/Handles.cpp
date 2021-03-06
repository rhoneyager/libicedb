#include "HH/Handles.hpp"

namespace HH {
	namespace Handles {
		HH_hid_t::~HH_hid_t() {}
		hid_t HH_hid_t::get() const { return *(_h.get()); }
		::std::shared_ptr<hid_t> HH_hid_t::getShared() const { return _h; }
		HH_hid_t::HH_hid_t() : HH_hid_t(-1, HH::Handles::Closers::DoNotClose::CloseP) {}
		HH_hid_t::HH_hid_t(::std::shared_ptr<hid_t> h) : _h(h) {}
		HH_hid_t::HH_hid_t(hid_t val, const std::function<void(hid_t*)>& closer)
		{
			if (closer) {
				_h = ::std::shared_ptr<hid_t>(new hid_t(val), closer);
			}
			else
				_h = ::std::shared_ptr<hid_t>(new hid_t(val));
		}
		hid_t HH_hid_t::operator()() const
		{
			return get();
		}
		HH_hid_t HH_hid_t::dummy() { return HH::Handles::HH_hid_t(-1, HH::Handles::Closers::DoNotClose::CloseP); }
		bool HH_hid_t::isValid() const {
			H5I_type_t typ = H5Iget_type(get());
			if (typ == H5I_BADID) return false;
			return true;
		}

	}
}
