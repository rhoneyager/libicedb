#pragma once
#include "defs.h"
#include <memory>
#include <type_traits>
#include <new>
//#include <gsl/gsl>

namespace icedb {
	/** Allocate memory in bytes. Generally this is just malloced, but a custom allocator may be substituted. **/
	DL_ICEDB void* _malloc(size_t numBytes);

	template <class T>
	T* malloc(size_t numBytes) {
		T* res = static_cast<T*>(_malloc(numBytes));
		return res;
	}

	/** Free memory region. Should not be double-freed. **/
	DL_ICEDB void _free(void* obj);
	template <class T>
	void free(T* obj) {
		_free(static_cast<void*>(obj));
	}
	namespace mem {

		/*
		void delete_single(gsl::owner<void *>ptr) noexcept;
		void delete_array(gsl::owner<void *> ptr) noexcept;

		template <class _Ty>
		struct icedb_delete
		{
			constexpr icedb_delete() noexcept = default;

			template<
				class _Ty2,
				class = typename std::enable_if<std::is_convertible<_Ty2 *, _Ty *>::value>::type
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
				class = typename std::enable_if<std::is_convertible<_Uty(*)[], _Ty(*)[]>::value>::type>
				icedb_delete(const icedb_delete<_Uty[]>&) noexcept
			{	// construct from another icedb_delete
			}

			template<class _Uty,
				class = typename std::enable_if<std::is_convertible<_Uty(*)[], _Ty(*)[]>::value>::type>
				void operator()(_Uty *_Ptr) const noexcept
			{	// delete a pointer
				static_assert(0 < sizeof(_Uty),
					"can't delete an incomplete type");
				::icedb::mem::delete_array(_Ptr);
			}
		};
		*/
	}
}
