#pragma once

namespace HH {
	/** \brief Implementation of tagged types, for C++-style variadic functions.
	*
	* This implementation allows for developers to pass parameters to functions by type,
	* instead of by parameter order. See Tags::Example::Example() for an example.
	**/
	namespace Tags {
		template <typename T, typename Tuple>
		struct has_type;

		template <typename T>
		struct has_type<T, std::tuple<>> : std::false_type {};

		template <typename T, typename U, typename... Ts>
		struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>> {};

		template <typename T, typename... Ts>
		struct has_type<T, std::tuple<T, Ts...>> : std::true_type {};

		template <class TagName, class DataType>
		struct Tag {
			DataType data;
			Tag(DataType d)
				: data{ std::move(d) } {}
			constexpr const DataType& get() const {
				return data;
			}
			constexpr const DataType& operator()() const { return get(); }
			constexpr const DataType* operator->() const { return &(get()); }
			constexpr decltype(auto) operator*() const { return (get()); }
			Tag<TagName, DataType>& operator=(DataType d) { data = d; return *this; }
			Tag() {}
		};

		namespace Example {
			// These tags produce parameterizable types!!!
			struct tag_string {};
			struct tag_int {};
			struct tag_float {};
			typedef Tag<tag_string, std::string> options_s;
			typedef Tag<tag_int, int> options_i;
			typedef Tag<tag_float, float> options_f;

			template <typename T, class ... Args>
			bool getOptionalValue(T& opt, std::tuple<Args...> vals,
				typename std::enable_if<HH::Tags::has_type<T, std::tuple<Args...> >::value>::type* = 0)
			{
				opt = std::get<T>(vals);
				return true;
			}

			template <typename T, class ... Args>
			bool getOptionalValue(T& opt, std::tuple<Args...> vals,
				typename std::enable_if<!HH::Tags::has_type<T, std::tuple<Args...>>::value>::type* = 0)
			{
				return false;
			}

			template <class ... Args>
			void caller(std::tuple<Args...> vals)
			{
				typedef std::tuple<Args...> vals_t;
				static_assert(HH::Tags::has_type<options_i, vals_t >::value, "Must have an options_i type");
				constexpr bool has_options_i = HH::Tags::has_type<options_i, vals_t >::value;
				//std::cerr << "has_options_i " << has_options_i << std::endl;
				auto opts = std::get<options_i>(vals);
				constexpr bool has_options_f = HH::Tags::has_type<options_f, vals_t >::value;
				//std::cerr << "has_options_f " << has_options_f << std::endl;
				auto optional_float = options_f(-1.f);
				bool gotVal = getOptionalValue(optional_float, vals);
				//std::cerr << "got_options_f " << gotVal << std::endl
				//	<< "opts_f " << optional_float.data << std::endl;
			}
			/// Pack everything into a tuple
			template <class ... Args>
			void caller(Args... args) {
				auto t = std::make_tuple(args...);
				caller(t);
			}
			// Can also allow for ordered parameters via clever function signatures. Trivial to implement.

			inline void Example() {
				auto a = options_f(1);
				auto b = options_i(8);
				caller(a, b);
				caller(options_i(4), options_f(5));
				//caller(a); // This won't work. options_i is a mandatory parameter.
				// Optional parameters can be detected using constexpr bool. Add a template to detect and, optionally,
				// set a value.
				caller(b);
				caller(options_i() = 29);

				caller(options_i() = 29, options_f(5));
			}

		}
	}
}