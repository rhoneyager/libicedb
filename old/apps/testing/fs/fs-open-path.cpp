/*! \file fs-open-path.cpp
	\brief Example program to open a path.

	This program illustrates how to use the icedb filesystem functions to open a path.
*/

#include <stdio.h>
#include <stdlib.h>
#include "../../../libicedb/icedb/error/error.h"
#include "../../../libicedb/icedb/fs/fs.h"

int errline = 0;

void handleError(ICEDB_error_code err) {
	size_t errSize = 0;
	struct ICEDB_error_context* errContext = nullptr;
	errSize = ICEDB_error_code_to_message_size(err);
	char *errMessage = (char*)malloc(sizeof(char)*(errSize + 1));
	ICEDB_error_code_to_message(err, errSize, errMessage);
	printf("Error code means: %s\n", errMessage);
	errContext = ICEDB_get_error_context_thread_local();

	free(errMessage);

	errSize = ICEDB_error_context_to_message_size(errContext);
	errMessage = (char*)malloc(sizeof(char)*(errSize + 1));

	ICEDB_error_context_to_message(errContext, errSize, errMessage);
	printf("Full error description: %s\n", errMessage);
	free(errMessage);

	ICEDB_error_context_deallocate(errContext);
}

#define hitError errline = __LINE__ - 1; goto errFail;
int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: fs-open-path path [path type]\n");
		return 1;
	}
	const char *path = argv[1];
	const char *pathtype = "";
	if (argc >= 3) pathtype = argv[2];

	ICEDB_error_code err = 0;
	ICEDB_free_charIPPP_f ICEDB_path_canOpen_deallocator = nullptr;
	size_t numHandlersThatCanOpen = 0;
	char*** pluginIDs = (char***) malloc(sizeof(char***));
	pluginIDs[0] = nullptr;

	if (!pluginIDs) hitError;
	// First, see if the path can be opened
	bool canOpen = ICEDB_path_canOpen(
		path,								// The path to potentially open
		pathtype,							// The user-identified type of path
		nullptr,							// No pluginid provided
		nullptr,							// No base handle provided
		ICEDB_flags_readonly,				// Opening only for read-only access	
		&numHandlersThatCanOpen,			// Stores the number of handlers that can read this path
		pluginIDs,							// Stores the list of plugins that can read this path
		&ICEDB_path_canOpen_deallocator,	// Stores the function that can close this path
		&err								// Stores any error code
	);
	if (err) hitError;

	printf("Can path be opened: %d\n", canOpen);


	// Second, to actually open the path. canOpen is ignored here deliberately.



	goto freeMem;

errFail:
	printf("Error on line %d.\n", errline);
	handleError(err);
freeMem:

	if (pluginIDs[0]) ICEDB_path_canOpen_deallocator(pluginIDs);
	if (pluginIDs) free(pluginIDs);

	return 0;
}