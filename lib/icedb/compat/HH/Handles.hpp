#pragma once
#include <hdf5.h>
#include <memory>
#include <functional>
#include <gsl/gsl_assert>
#include "Handles_HDF.hpp"
#include "Handles_Valid.hpp"

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
		//template <typename handle_type = Handle_Types::UNKNOWN>
		class HH_hid_t
		{
			::std::shared_ptr<hid_t> _h;
			Handle_Types _typ;
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
			HH_hid_t(hid_t val, const std::function<void(hid_t*)> &closer = nullptr)
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
		};
	}
	using Handles::HH_hid_t;
}
