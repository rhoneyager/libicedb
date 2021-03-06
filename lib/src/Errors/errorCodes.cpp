#include "icedb/Errors/errorCodes.h"

ICEDB_PRIVATE const char* ICEDB_ERRORCODES_MAP[ICEDB_ERRORCODES_TOTAL] =
{
	"No error.\n",
	"Unimplemented error. TODO.\n",
	"dlopen error. See system message for specifics.\n",
	"The DLL handle does not exist. The dll is probably not open.\n",
	"Cannot open the DLL symbol.\n",
	"Destroying the DLL handle, but references still exist!\n",
	"Trying to decrement dll reference count below zero.\n",
	"OS-specific general error.\n",
	"Unimplemented function of function path.\n",
	"Either not a plugin or an incompatible version.\n",
	"The path does not exist.\n",
	"The path is read only. No modifications allowed.\n",
	"A null pointer was passed to a function. The function prohobits null pointers in this context.\n"
};