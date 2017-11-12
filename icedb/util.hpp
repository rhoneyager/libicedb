#pragma once
#include <memory>
#include <type_traits>
#include <new>

namespace icedb {
	namespace mem {
		void delete_single(void *ptr) noexcept;
		void delete_array(void *ptr) noexcept;

		template <class _Ty>
		struct icedb_delete
		{
			constexpr icedb_delete() noexcept = default;

			template<
				class _Ty2,
				class = std::enable_if<std::is_convertible<_Ty2 *, _Ty *>::value>::type
			>
			icedb_delete(const icedb_delete<_Ty2>&) noexcept
			{}	// construct from another icedb_delete

			void operator()(_Ty *_Ptr) const noexcept
			{	// delete a pointer
				static_assert(0 < sizeof(_Ty),
					"can't delete an incomplete type");
				//delete _Ptr;
				::icedb::mem::delete_single(_Ptr);
			}
		};

		template<class _Ty>
		struct icedb_delete<_Ty[]>
		{	// default deleter for unique_ptr to array of unknown size
			constexpr icedb_delete() noexcept = default;

			template<class _Uty,
				class = std::enable_if<std::is_convertible<_Uty(*)[], _Ty(*)[]>::value>::type>
				icedb_delete(const icedb_delete<_Uty[]>&) noexcept
			{	// construct from another icedb_delete
			}

			template<class _Uty,
				class = std::enable_if<std::is_convertible<_Uty(*)[], _Ty(*)[]>::value>::type>
				void operator()(_Uty *_Ptr) const noexcept
			{	// delete a pointer
				static_assert(0 < sizeof(_Uty),
					"can't delete an incomplete type");
				::icedb::mem::delete_array(_Ptr);
			}
		};
	}
}