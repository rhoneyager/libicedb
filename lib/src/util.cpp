#include <memory>
#include <new>
#include <string>
#include "../icedb/util.hpp"

namespace icedb {
	namespace mem {
		/*
#ifdef _MSC_FULL_VER
		[[gsl::suppress(i.11)]]
#endif
		void delete_single(gsl::owner<void *> ptr) noexcept {
#ifdef _MSC_FULL_VER
			[[gsl::suppress(r.11)]]
#endif
				delete ptr;
		}
#ifdef _MSC_FULL_VER
		[[gsl::suppress(i.11)]]
#endif
		void delete_array(gsl::owner<void *>ptr) noexcept {
#ifdef _MSC_FULL_VER
			[[gsl::suppress(r.11)]]
#endif
				delete[] ptr;
		}
		//void operator delete(void* ptr, std::size_t size) noexcept;
		//void operator delete(void* ptr, std::align_val_t alignment) noexcept;
		//void operator delete(void* ptr, std::size_t size, std::align_val_t alignment) noexcept;
		//void operator delete(void* ptr, const std::nothrow_t&) noexcept;
		//void operator delete(void* ptr, std::align_val_t alignment,
		//	const std::nothrow_t&) noexcept;
		*/
	}
}
