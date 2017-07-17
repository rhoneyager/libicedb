#pragma once
#ifndef ICEDB_HPP_MEM
#define ICEDB_HPP_MEM
#include "../defs.h"
ICEDB_BEGIN_DECL

namespace icedb {
	/** Allocate memory in bytes. Generally this is just malloced, but a custom allocator may be substituted. **/
	ICEDB_SYMBOL_SHARED void* _malloc(size_t numBytes);

	template <class T>
	T* malloc(size_t numBytes) {
		T* res = static_cast<T*>(_malloc(numBytes));
		return res;
	}

	/** Free memory region. Should not be double-freed. **/
	ICEDB_SYMBOL_SHARED void _free(void* obj);
	template <class T>
	void free(T* obj) {
		_free(static_cast<void*>(obj));
	}

}

ICEDB_END_DECL
#endif
