#include "../germany/errorCodes.h"

ICEDB_SYMBOL_PRIVATE const char* ICEDB_ERRORCODES_MAP[ICEDB_ERRORCODES_TOTAL] = 
{
	"No error.\n",
	"Unimplemented error. TODO.\n",
	"dlopen error. See system message for specifics.\n",
	"The DLL handle does not exist. The dll is probably not open.\n",
	"Cannot open the DLL symbol.\n",
	"Destroying the DLL handle, but references still exist!\n",
	"Trying to decrement dll reference count below zero.\n"
};