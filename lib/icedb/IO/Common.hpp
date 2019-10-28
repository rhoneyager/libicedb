#pragma once
#include "../defs.h"
#include <string>
#include "../misc/Constants.hpp"
#include "../Utils/util.hpp"

namespace icedb {
	//extern const std::string _icedb_obj_type_identifier;
	typedef std::pair<std::string, HH::HH_hid_t> Name_Type_t;

	template <class T, typename CN, typename Name_Type_t = Name_Type_t>
	struct Common_HH_Base
	{
		typedef const std::map<CN, Name_Type_t> Validator_map_t;
		static Validator_map_t getRequiredAttributes() {
			throw; //static_assert(false, "Common_HH_Base must be template specialized");
		}
		static Validator_map_t getRequiredDatasets() {
			throw; //static_assert(false, "Common_HH_Base must be template specialized");
		}
		static std::string getIdentifier() {
			throw; //static_assert(false, "Common_HH_Base must be template specialized");
		}
	};

	template <class T, typename CN, typename Name_Type_t = Name_Type_t>
	struct HH_Base : public HH::Group, Common_HH_Base<T, CN, Name_Type_t>
	{
		typedef const std::map<CN, Name_Type_t> Validator_map_t;
	private:
		//Validator_map_t _req_atts, _req_dsets;
	public:
		virtual ~HH_Base() {}
		HH_Base(HH::HH_hid_t hnd_grp = HH::HH_hid_t::dummy())
			: HH::Group(hnd_grp) {}
		HH_Base(Group& g) : HH::Group(g) {}

		/// \brief Is this HDF5 object a T?
		/// \returns true if it is a T, false otherwise.
		static bool isA(HH::HH_hid_t object)
		{
			HH::Group g(object);
			if (!g.isGroup()) return false;
			if (!g.atts.exists(Constants::AttNames::icedb_object_type)) return false;
			std::string expectedIdentifier = Common_HH_Base<T, CN, Name_Type_t>::getIdentifier();
			/// \todo Enable attribute string reads again!!!!!
			//if (!(g.atts[Constants::icedb_object_type].read<std::string>() == expectedIdentifier)) return false;
			return true;
		}
		bool isA() const { return isA(get()); }

		/// \brief Is the object valid according to the spec.?
		/// \param object is the HDF5 object
		/// \throws if the object pointed to by group is not a valid HDF5 group.
		/// \param out is an output stream to which diagnostic messages can be written. Diagnostics include why an object is not a Swath (e.g. missing an essential parameter).
		/// \throws if the output stream is somehow invalid
		static bool isValid(HH::HH_hid_t object, std::ostream* out = nullptr)
		{
			if (!isA(object)) {
				if (out)
					* out << Constants::AttNames::icedb_object_type << " test failed." << std::endl;
				return false;
			}

			auto reqAtts = Common_HH_Base<T, CN, Name_Type_t>::getRequiredAttributes();
			auto reqDsets = Common_HH_Base<T, CN, Name_Type_t>::getRequiredDatasets();

			auto testRes = Validate::TestGroupIsValid(object, reqAtts, reqDsets);

			if (testRes.first) return true;
			if (out)
				* out << testRes.second << std::endl;
			return false;
		}
		bool isValid(std::ostream* out = nullptr) const {
			return isValid(get(), out);
		}
	};
}
