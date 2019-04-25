The OS Info Functions (Info.hpp) {#Info}
---------------------------

These functions are defined in include/BetterThrow/Info.hpp.

An example is at Examples/Info_Query/Info_query.cpp.

## The typedefs

BT::native_path_string_t is the native string type used on your operating system.
Microsoft Windows uses wide characters (wchar_t), so the string type is std::wstring.
For all other operating systems, the string type is std::string.

This distinction is necessary because Windows and other operating systems implement
different versions of Unicode internally. 

BT::native_path_stream_t is a typedef to a stream type that can output the
native string type. 

## The return types

All of the information-querying functions either return error codes or throw.
There are three main return types.

- int - used in BT::getPID() and BT::getPPID(int pid). Return values less than zero denote an error.
- BT::PairwiseValue - acts as a substitute for std::optional<T> in C++17. This
struct has two members: success (a bool) and value (a type T). Always check success.
- BT::Error_Res_t contains an error code and an error message. Nonzero error codes denote an error.

Other return types include static objects, or lists of objects. Functions that
return these types may throw, but they never return an error code.

## General functions
- const char* BT::getOSfamilyName()
- BT::Error_Res_t BT::getOSerror()
- BT::Error_Res_t BT::getOSerrno(int e = -1)

## Process ID-querying functions
- int BT::getPID() gets the current process' identifier.
- int BT::getPPID(int pid = BT::getPID()) gets the parent process for a given PID.
- BT::PairwiseValue <bool> BT::pidExists(int pid)
- BT::ProcessInfo BT::ProcessInfo::get(int pid)

## Functions to get the runtime environment of the current process
- const BT::RuntimeInfo& BT::getRuntimeInfo()
- BT::ModuleInfo_t BT::getLoadedModules();
- BT::native_path_string_t BT::getModule(void *address = nullptr);

## The structures

- BT::ProcessInfo expresses basic information about a process, incluting it's name,
executable path, current working directory, environment variables, command line arguments,
and start time.
- BT::RuntimeInfo expresses basic information about the current login environment,
including the host name, user name, home directory, and the per-user application
configuration base directory (e.g. ~/.config, or C:\\Users\\username\\AppData).
