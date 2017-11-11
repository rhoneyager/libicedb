#include <memory>
#include <new>
#include <string>
#include "../icedb/util.hpp"

namespace icedb {
	namespace mem {
		void delete_single(void *ptr) noexcept {
			delete ptr;
		}
		void delete_array(void *ptr) noexcept {
			delete[] ptr;
		}

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