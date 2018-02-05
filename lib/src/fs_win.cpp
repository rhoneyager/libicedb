#include "../icedb/fs.hpp"
#include <system_error>
#define _BIND_TO_CURRENT_VCLIBS_VERSION 1
#include <Windows.h>
#include <ShlObj.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <Shlwapi.h>
#pragma comment(lib, "Psapi")
#pragma comment(lib, "Ws2_32")
#pragma comment(lib, "Advapi32")
#pragma comment(lib, "Shell32")
#pragma comment(lib, "Shlwapi")


namespace icedb {

	namespace fs {

		bool exists(const path& p, std::error_code &ec) noexcept {
			bool res = PathFileExistsW(p.native().c_str());
			DWORD err = GetLastError();
			ec.assign(static_cast<int>(err), std::system_category());
			return res;
		}
		bool exists(const path &p) {
			std::error_code ec;
			bool res = exists(p, ec);
			if (ec.value()) throw std::runtime_error(ec.message().c_str());
			return res;
		}
		bool is_directory(const path& p, std::error_code &ec) noexcept {
			bool res = PathIsDirectoryW(p.native().c_str());
			DWORD err = GetLastError();
			ec.assign(static_cast<int>(err), std::system_category());
			return res;
		}
		bool is_directory(const path& p) {
			std::error_code ec;
			bool res = is_directory(p, ec);
			if (ec.value()) throw std::runtime_error(ec.message().c_str());
			return res;
		}
		bool is_symlink(const path& p, std::error_code &ec) noexcept {
			DWORD atts = GetFileAttributesW(p.native().c_str());
			if (atts == INVALID_FILE_ATTRIBUTES) {
				DWORD err = GetLastError();
				ec.assign(static_cast<int>(err), std::system_category());
			}
			bool isSymLink = (atts & FILE_ATTRIBUTE_REPARSE_POINT) ? true : false;
			return isSymLink;
		}
		bool is_symlink(const path& p) {
			std::error_code ec;
			bool res = is_symlink(p, ec);
			if (ec.value()) throw std::runtime_error(ec.message().c_str());
			return res;
		}
		path read_symlink(const path& p, std::error_code &ec) noexcept {
			HANDLE hFile = nullptr;
			// See example at https://msdn.microsoft.com/en-us/library/windows/desktop/aa364962(v=vs.85).aspx
			hFile = CreateFileW(p.native().c_str(), GENERIC_READ, FILE_SHARE_READ,
				NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			path res;
			if (!hFile) { // File could not be opened
				ec.assign(static_cast<int>(ENOENT), std::system_category());
			}
			else {
				const int MaxUnicodePath = 32767;
				WCHAR Path[MaxUnicodePath];
				DWORD dwRet = GetFinalPathNameByHandleW(hFile, Path, MaxUnicodePath, VOLUME_NAME_NT);
				
				if (dwRet < MAX_PATH) {
					res.assign(std::basic_string<wchar_t>(Path));
				}
				else {
					ec.assign(static_cast<int>(ERANGE), std::system_category());
				}
				CloseHandle(hFile);
			}
			return res;
		}
		path read_symlink(const path& p) {
			std::error_code ec;
			path res = read_symlink(p, ec);
			if (ec.value()) throw std::runtime_error(ec.message().c_str());
			return res;
		}
		bool create_directory(const path& p, std::error_code& ec) noexcept {
			bool res = CreateDirectoryW(p.native().c_str(), NULL);
			if (!res) {
				DWORD err = GetLastError();
				ec.assign(static_cast<int>(err), std::system_category());
			}
			return res;
		}
		bool create_directory(const path& p) {
			std::error_code ec;
			bool res = create_directory(p, ec);
			if (ec.value()) throw std::runtime_error(ec.message().c_str());
			return res;
		}
		//bool create_directories(const path& p, std::error_code& ec) noexcept;

	}
}
