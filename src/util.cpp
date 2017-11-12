#include <memory>
#include <new>
#include <string>
#include "../icedb/util.hpp"

namespace icedb {
	namespace mem {
		[[gsl::suppress(i.11)]]
		void delete_single(gsl::owner<void *> ptr) noexcept {
			[[gsl::suppress(r.11)]] delete ptr;
		}
		[[gsl::suppress(i.11)]]
		void delete_array(gsl::owner<void *>ptr) noexcept {
			[[gsl::suppress(r.11)]] delete[] ptr;
		}

		[[gsl::suppress(r.11)]]
		void test() {
			auto p = std::unique_ptr<std::string, icedb::mem::icedb_delete<std::string> >(new std::string);
			p->append("Test");
		}

		//void operator delete(void* ptr, std::size_t size) noexcept;
		//void operator delete(void* ptr, std::align_val_t alignment) noexcept;
		//void operator delete(void* ptr, std::size_t size, std::align_val_t alignment) noexcept;
		//void operator delete(void* ptr, const std::nothrow_t&) noexcept;
		//void operator delete(void* ptr, std::align_val_t alignment,
		//	const std::nothrow_t&) noexcept;
	}
}