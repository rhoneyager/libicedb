#include <string>
#include <iostream>
#define BOOST_TEST_DYN_LINK
//#include <HH/Handles.hpp>
//#include <HH/Handles_HDF.hpp>
//#include <HH/Handles_Valid.hpp>
#include <HH/Tags.hpp>
//#include <HH/Types.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(HDFforHumans_Tags);

using namespace std;
using namespace HH;

namespace 
{
	template <class ... Args>
	float Test1(std::tuple<Args...> vals)
	{
		using namespace HH::Tags::Example;
		typedef std::tuple<Args...> vals_t;
		static_assert(HH::Tags::has_type<HH::Tags::Example::options_i, vals_t >::value, "Must have an options_i type");
		constexpr bool has_options_i = HH::Tags::has_type<HH::Tags::Example::options_i, vals_t >::value;
		//std::cerr << "has_options_i " << has_options_i << std::endl;
		auto opts = std::get<HH::Tags::Example::options_i>(vals);
		constexpr bool has_options_f = HH::Tags::has_type<HH::Tags::Example::options_f, vals_t >::value;
		//std::cerr << "has_options_f " << has_options_f << std::endl;
		auto optional_float = HH::Tags::Example::options_f(-1.f);
		bool gotVal = getOptionalValue(optional_float, vals);
		//std::cerr << "got_options_f " << gotVal << std::endl
		//	<< "opts_f " << optional_float.data << std::endl;
		return optional_float.data;
	}
	/// Pack everything into a tuple
	template <class ... Args>
	float Test1(Args... args) {
		auto t = std::make_tuple(args...);
		return Test1(t);
	}

	// Verify assertions that certain types are provided for tags.
	BOOST_AUTO_TEST_CASE(tags_has_Type) {
		using namespace HH::Tags::Example;
		auto b = options_i(8);
		auto t = std::make_tuple(b);
		typedef decltype(t) vals_t;
		constexpr bool has_options_i = HH::Tags::has_type<options_i, vals_t >::value;
		BOOST_CHECK_EQUAL(has_options_i, true);
		constexpr bool has_options_f = HH::Tags::has_type<options_f, vals_t >::value;
		BOOST_CHECK_EQUAL(has_options_f, false);
	}

	// Check that an optional value is provided
	BOOST_AUTO_TEST_CASE(tags_getOptionalValue) {
		using namespace HH::Tags::Example;
		auto a = options_f(1);
		auto b = options_i(8);
		float res = Test1(a, b);
		BOOST_CHECK_EQUAL(res, 1.f);
	}

	// Check that an optional value is missing
	BOOST_AUTO_TEST_CASE(tags_getOptionalValue2) {
		using namespace HH::Tags::Example;
		//auto a = options_f(1);
		auto b = options_i(8);
		float res = Test1(b);
		BOOST_CHECK_EQUAL(res, -1.f);
	}


}


BOOST_AUTO_TEST_SUITE_END();

