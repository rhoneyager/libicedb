#pragma once
#include "defs.h"
#include <memory>
#include <type_traits>
#include <new>
//#include <gsl/gsl>

namespace icedb {
	/** Allocate memory in bytes. Generally this is just malloced, but a custom allocator may be substituted. **/
	ICEDB_DL void* _malloc(size_t numBytes);

	template <class T>
	T* malloc(size_t numBytes) {
		T* res = static_cast<T*>(_malloc(numBytes));
		return res;
	}

	/** Free memory region. Should not be double-freed. **/
	ICEDB_DL void _free(void* obj);
	template <class T>
	void free(T* obj) {
		_free(static_cast<void*>(obj));
	}
}
