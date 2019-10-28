#pragma once
#ifndef ICEDB_H_OS_FUNCTIONS
#define ICEDB_H_OS_FUNCTIONS
#include "../defs.h"

ICEDB_BEGIN_DECL_C

ICEDB_DL bool ICEDB_waitOnExitGetDefault();
ICEDB_DL void ICEDB_waitOnExitSet(bool val);
ICEDB_DL bool ICEDB_waitOnExitGet();

/// Parse program options and initialize the library.
ICEDB_DL void ICEDB_load();
ICEDB_DL void ICEDB_load_with_args(int argc, const char* const* argv);

ICEDB_DL void ICEDB_writeDebugString(const char*);

ICEDB_END_DECL_C


#endif
