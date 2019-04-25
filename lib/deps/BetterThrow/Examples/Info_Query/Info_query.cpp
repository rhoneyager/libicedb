/** \file
 * \brief Example program that gets information about the runtime environment.
 *
 * This program's output should look similar to this:
 *
~~~~~
The OS is Windows
Process Information
Name: Info_query.exe
Path: C:\Users\ryan\CMakeBuilds\922b566f-4200-f03c-a3ff-20d977d0cd19\build\x64-Debug\Examples\Info_Query\Info_query.exe
CWD: C:\Users\ryan\CMakeBuilds\922b566f-4200-f03c-a3ff-20d977d0cd19\build\x64-Debug\Examples\Info_Query
PID: 18148
PPID: 7624
Admin? 0
Start: 2019-2-8 13:25:36.551
Cmdline:
		C:\Users\ryan\CMakeBuilds\922b566f-4200-f03c-a3ff-20d977d0cd19\build\x64-Debug\Examples\Info_Query\Info_query.exe
Environment:
		ALLUSERSPROFILE =       C:\ProgramData
		APPDATA =       C:\Users\ryan\AppData\Roaming
		(more environment variables follow.....)


Executable Path: C:\Users\ryan\CMakeBuilds\922b566f-4200-f03c-a3ff-20d977d0cd19\build\x64-Debug\Examples\Info_Query\Info_query.exe

Loaded Modules:
C:\Users\ryan\CMakeBuilds\922b566f-4200-f03c-a3ff-20d977d0cd19\build\x64-Debug\Examples\Info_Query\Info_query.exe
C:\WINDOWS\SYSTEM32\ntdll.dll
C:\WINDOWS\System32\KERNEL32.DLL
C:\WINDOWS\System32\KERNELBASE.dll
C:\WINDOWS\System32\SHELL32.dll
C:\WINDOWS\System32\msvcrt.dll
C:\WINDOWS\System32\cfgmgr32.dll
C:\WINDOWS\System32\ucrtbase.dll
C:\WINDOWS\System32\shcore.dll
C:\WINDOWS\System32\RPCRT4.dll
C:\WINDOWS\System32\combase.dll
C:\WINDOWS\System32\bcryptPrimitives.dll
C:\WINDOWS\System32\windows.storage.dll
C:\WINDOWS\System32\msvcp_win.dll
C:\WINDOWS\System32\sechost.dll
C:\WINDOWS\System32\advapi32.dll
C:\WINDOWS\System32\profapi.dll
C:\WINDOWS\System32\powrprof.dll
C:\WINDOWS\System32\shlwapi.dll
C:\WINDOWS\System32\GDI32.dll
C:\WINDOWS\System32\gdi32full.dll
C:\WINDOWS\System32\USER32.dll
C:\WINDOWS\System32\win32u.dll
C:\WINDOWS\System32\kernel.appcore.dll
C:\WINDOWS\System32\cryptsp.dll
C:\WINDOWS\System32\ole32.dll
C:\WINDOWS\SYSTEM32\MSVCP140D.dll
C:\WINDOWS\SYSTEM32\VCRUNTIME140D.dll
C:\WINDOWS\SYSTEM32\VCRUNTIME140_1D.dll
C:\WINDOWS\SYSTEM32\ucrtbased.dll
C:\WINDOWS\System32\IMM32.DLL
Runtime Information:
User name: ryan
Computer name: KAY
App config dir: C:\Users\ryan\AppData\Local
Home directory: C:\Users\ryan
~~~~~

 **/

#include <iostream>
#include <string>
#include "../../include/BetterThrow/Error.hpp"
#include "../../include/BetterThrow/Info.hpp"

int main(int, char**)
{
	try {
		using std::cout;
		using std::endl;
		
		cout << "The OS is " << BT::getOSfamilyName() << endl;
		auto pi = BT::ProcessInfo<BT::native_path_string_t>::get<BT::native_path_string_t>(BT::getPID());

		BT::native_cout << BT_NATIVE("Process Information\n") << pi << endl;

		BT::native_cout << BT_NATIVE("Executable Path: ")
			<< BT::getModule(nullptr) << endl << endl;

		BT::native_cout << BT_NATIVE("Loaded Modules: \n");
		auto mods = BT::getLoadedModules();
		for (const auto &m : mods)
			BT::native_cout << m << endl;
		
		auto ui = BT::RuntimeInfo<BT::native_path_string_t>::get<BT::native_path_string_t>();
		BT::native_cout << BT_NATIVE("Runtime Information: \n") << ui << endl;
	}
	catch (std::exception & f) {
		BT::print_exception(f);
	}
	return 0;
}
