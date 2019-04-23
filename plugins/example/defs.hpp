#pragma once
#include <icedb/defs.h>

// If SHARED_(libname) is defined, then the target library both 
// exprts and imports. If not defined, then it is a static library.

// Macros defined as EXPORTING_(libname) are internal to the library.
// They indicate that SHARED_EXPORT_ICEDB should be used.
// If EXPORTING_ is not defined, then SHARED_IMPORT_ICEDB should be used.

#if SHARED_plugin_example
#if EXPORTING_plugin_example
#define ICEDB_DL_PLUGIN_EXAMPLE SHARED_EXPORT_ICEDB
#else
#define ICEDB_DL_PLUGIN_EXAMPLE SHARED_IMPORT_ICEDB
#endif
#else
#define ICEDB_DL_PLUGIN_EXAMPLE SHARED_EXPORT_ICEDB
#endif

// Compiler interface warning suppression
#if defined _MSC_FULL_VER
#pragma warning( disable : 4003 ) // Bug in boost with VS2016.3
#pragma warning( disable : 4251 ) // DLL interface
#pragma warning( disable : 4275 ) // DLL interface
#endif
