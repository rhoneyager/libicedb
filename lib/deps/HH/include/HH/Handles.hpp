#pragma once
#include "defs.hpp"
#include <hdf5.h>
#include <memory>
#include <functional>
#include "Handles_HDF.hpp"

namespace HH {

	namespace Handles {
		enum class Handle_Types {
			ATTRIBUTE,
			DATASET,
			DATASPACE,
			DATATYPE,
			FILE,
			GROUP,
			LINK,
			PROPERTYLIST,
			REFERENCE,
			UNKNOWN
		};

		class HH_hid_t
		{
			::std::shared_ptr<hid_t> _h;
			//Handle_Types _typ;
		public:
			~HH_hid_t() {}
			hid_t get() const { return *(_h.get()); }
			::std::shared_ptr<hid_t> getShared() const { return _h; }

			// Future handle type safety implementation:
			//Handle_Types get_type() const { return _typ; }
			//bool isA(Handle_Types t) const { return (_typ == t); }
			//template <typename T>
			//bool isA() const { return (_typ == ); }

			HH_hid_t(::std::shared_ptr<hid_t> h) : _h(h) {}
			HH_hid_t(hid_t val, const std::function<void(hid_t*)>& closer = nullptr)
			{
				if (closer) {
					_h = ::std::shared_ptr<hid_t>(new hid_t(val), closer);
				}
				else
					_h = ::std::shared_ptr<hid_t>(new hid_t(val));
			}
			hid_t operator()() const
			{
				return get();
			}
			static HH_hid_t dummy() { return HH::Handles::HH_hid_t(-1, HH::Handles::Closers::DoNotClose::CloseP); }
		};
	}
	using Handles::HH_hid_t;
}
