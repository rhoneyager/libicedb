#pragma once
#include <memory>
#include <type_traits>
#include <gsl/gsl_assert>

namespace HH {
	namespace Handles {
		// From http://en.cppreference.com/w/cpp/experimental/is_detected
		// and https://stackoverflow.com/questions/257288/is-it-possible-to-write-a-template-to-check-for-a-functions-existence
		namespace detail {
			struct nonesuch {
				nonesuch() = delete;
				~nonesuch() = delete;
				nonesuch(nonesuch const&) = delete;
				void operator=(nonesuch const&) = delete;
			};

			template <class Default, class AlwaysVoid,
				template<class...> class Op, class... Args>
			struct detector {
				using value_t = std::false_type;
				using type = Default;
			};

			template <class Default, template<class...> class Op, class... Args>
			struct detector<Default, std::void_t<Op<Args...>>, Op, Args...> {
				// Note that std::void_t is a C++17 feature
				using value_t = std::true_type;
				using type = Op<Args...>;
			};

		} // namespace detail

		template <template<class...> class Op, class... Args>
		using is_detected = typename detail::detector<detail::nonesuch, void, Op, Args...>::value_t;

		template< template<class...> class Op, class... Args >
		constexpr bool is_detected_v = is_detected<Op, Args...>::value;

		template <template<class...> class Op, class... Args>
		using detected_t = typename detail::detector<detail::nonesuch, void, Op, Args...>::type;

		template <class Default, template<class...> class Op, class... Args>
		using detected_or = detail::detector<Default, void, Op, Args...>;


		template<typename T>
		using valid_t = decltype(std::declval<T&>().valid());

		/// Check if a class has a function matching "valid()".
		template<typename T>
		constexpr bool has_valid = is_detected_v<valid_t, T>;



		/// Ensures that a handle is not invalid.
		template <class T>
		class not_invalid
		{
		private:
			std::unique_ptr<T> _heldObj;
			T& _ptr;
		public:
			static_assert(has_valid<T> == true,
				"To use not_invalid, you must use a class that provides the valid() method.");
			constexpr T& get() const {
				bool isValid = _ptr.valid();
				Expects(isValid);
				return _ptr;
			}
			constexpr T& operator()() const { return get(); }
			constexpr T* operator->() const { return &(get()); }
			constexpr decltype(auto) operator*() const { return (get()); }

			constexpr not_invalid(T&& t) : _ptr(std::forward<T>(t)) {
				Expects(_ptr.valid());
			}

			template <typename U>//, typename = std::enable_if_t<std::is_convertible<U, T>::value>,
								//typename = std::enable_if_t<!std::is_reference_v<U>> >
			/// \note Should this need a move reference? Otherwise, it triggers u's destructor.
			/// Or, is thould be a weak reference -> one that does not trigger a close.
			/// \todo Better support for weak references.
			constexpr not_invalid(U&& u) : _heldObj{ std::make_unique<T>(u) }, _ptr{ *_heldObj.get() }
			{
				bool isValid = _ptr.valid();
				Expects(isValid);
			}
			// Original:
			//constexpr not_invalid(U u) : _heldObj{ std::make_unique<T>(u) }, _ptr{ *_heldObj.get() }
			//{
			//	bool isValid = _ptr.valid();
			//	Expects(isValid);
			//}

			/*
			template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>,
			typename = std::enable_if_t<std::is_reference_v<U>> >
			constexpr not_invalid(U&& u) : _ptr(std::forward<U>(u))
			{
			Expects(_ptr.valid());
			}
			*/

			constexpr not_invalid(T& t) : _ptr(t) {
				Expects(_ptr.valid());
			}

			template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
			constexpr not_invalid(const not_invalid<U>& other) : not_invalid(other.get())
			{
			}
		};

	}
}
