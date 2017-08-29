#pragma once
#ifndef ICEDB_H_MEM
#define ICEDB_H_MEM
#include "../defs.h"
#include "../dlls/linking.h"
ICEDB_BEGIN_DECL_C


/** \brief Free a char**, and each sub-object. Assumes null-termination of the array. **/
typedef void(*ICEDB_free_charPP_f)(char **);
/** \brief Free a char***, and each sub-object. Assumes null-termination of the array. **/
typedef void(*ICEDB_free_charPPP_f)(char ***);

/** \brief Free memory inside a char**. So, it frees each char* within the array, but does not free the surrounding pointer. Assumes internal null termination. **/
typedef void(*ICEDB_free_charIPP_f)(char ** const);
/** \brief Free memory inside a char***. So, it frees each char** within the array, but does not free the surrounding pointer. Assumes internal null termination. **/
typedef void(*ICEDB_free_charIPPP_f)(char *** const);

	/** \brief Allocate memory in bytes. Generally this is just malloced, but a custom allocator may be substituted. **/
	DL_ICEDB void* ICEDB_malloc(size_t numBytes);

	/** \brief Free memory region. Should not be double-freed. **/
	DL_ICEDB void ICEDB_free(void* obj);

	/** \brief Free memory associated with a character pointer. **/
	DL_ICEDB void ICEDB_free_charP(char*);

	/** \brief Free memory inside a char**. So, it frees each char* within the array, but does not free the surrounding pointer. **/
	DL_ICEDB void ICEDB_free_charIPP(char**);


ICEDB_END_DECL_C
#endif
