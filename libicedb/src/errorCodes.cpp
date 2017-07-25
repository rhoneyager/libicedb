#include "../icedb/error/errorCodes.h"

ICEDB_SYMBOL_PRIVATE const wchar_t* ICEDB_ERRORCODES_MAP[ICEDB_ERRORCODES_TOTAL] = 
{
	L"No error.\n",
	L"Unimplemented error. TODO.\n",
	L"dlopen error. See system message for specifics.\n",
	L"The DLL handle does not exist. The dll is probably not open.\n",
	L"Cannot open the DLL symbol.\n",
	L"Destroying the DLL handle, but references still exist!\n",
	L"Trying to decrement dll reference count below zero.\n",
	L"OS-specific general error.\n",
	L"Unimplemented function of function path.\n",
	L"Either not a plugin or an incompatible version.\n",
	L"The path does not exist.\n",
	L"The path is read only. No modifications allowed.\n"
};