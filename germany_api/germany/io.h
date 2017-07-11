#pragma once
#ifndef ICEDB_H_IO
#define ICEDB_H_IO
#include "../../defs.h"
#include "../../error.h"
#include "../../plugins.h"

/// An opaque pointer that indicates a file that is open for reading or writing.
/// Different providers can handle different file formats.
/// Some formats may act as containers for sub-objects. To access these objects,
/// \see ICEDB_io_context_handle.
struct ICEDB_file_handle;
typedef ICEDB_file_handle* ICEDB_file_handle_p;

/// An opaque pointer to a particular object within a file.
struct ICEDB_io_object_handle;
typedef ICEDB_io_object_handle* ICEDB_io_object_handle_p;

// Function for navigating to a path within an object
// Function for recursing over all sub-paths under a node
// Function for determining which plugin should be used when reading a particular object
// Open a file
// Close a file
// Create a file
// Truncate a file
// Create a path
// Get an io_object handle from the path
// Write an object to the path
// Read a path into an object

#endif
