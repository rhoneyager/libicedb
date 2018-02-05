#pragma once
#include "../icedb/defs.h"
#include <map>
#include <memory>
#include <string>
#include <vector>
//#include "gsl/span"
//#include "Data_Types.hpp"

namespace icedb {

	namespace fs {
		enum class ObjectTypes {
			ANY,
			SHAPE
		};

		enum class IOopenFlags {
			READ_ONLY,
			READ_WRITE,
			TRUNCATE,
			CREATE
		};

		enum class file_type {
			none,
			not_found,
			regular,
			directory,
			symlink,
			block,
			character,
			fifo,
			socket,
			unknown
		};

		class path {
		public:
#if defined(ICEDB_OS_WINDOWS)
			typedef wchar_t value_type;
			static constexpr value_type preferred_separator = L'\\';
#elif defined(ICEDB_OS_UNIX) || defined(ICEDB_OS_LINUX)
			typedef char value_type;
			static constexpr value_type preferred_separator = '/';
#else
			static_assert(0, "The icedb-internal filesystem library is not available for this type of system.");
#endif
			typedef std::basic_string<value_type> string_type;

		private:
			string_type _M_pathname;
			//file_type _M_type;
			void _M_split_cmpts();
			path& _M_append(const string_type&);
			enum class _Type : unsigned char {
				_Multi, _Root_name, _Root_dir, _Filename
			};
		public:
			//struct _Cmpt;
			typedef string_type _Cmpt;
			using _List = std::vector<_Cmpt>;
			_List _M_cmpts; // empty unless _M_type == _Type::_Multi
			_Type _M_type = _Type::_Multi;

			path() noexcept {}
			path(const path & __p) = default;
			path(path &&p) noexcept
				:_M_pathname(std::move(p._M_pathname)), _M_type(p._M_type)
			{
				_M_split_cmpts();
				p.clear();
			}
			path(string_type&& source) :
				_M_pathname(std::move(source)) {
				_M_split_cmpts();
			}
			path(const std::string &src);

			~path() = default;

			path& operator=(const path& __p) = default;
			path& operator=(path&& __p) noexcept;
			path& operator=(string_type&& __source);
			path& assign(string_type&& __source);

			path& operator/=(const path& __p) { return _M_append(__p._M_pathname); }
			inline path operator/(const path &p) { path res = *this; res /= p; return p; }
			bool operator==(const path& p) const
			{
				if (p._M_pathname != _M_pathname) return false;
				return true;
			}
			bool operator!=(const path& p) const { return !operator==(p); }

			void clear() noexcept { _M_pathname.clear(); _M_split_cmpts(); }

			void swap(path& __rhs) noexcept {
				__rhs._M_pathname.swap(_M_pathname);
				//std::swap(__rhs._M_type, _M_type);
			}

			// native format observers

			const string_type&  native() const noexcept { return _M_pathname; }
			const value_type*   c_str() const noexcept { return _M_pathname.c_str(); }
			operator string_type() const { return _M_pathname; }

			std::string    string() const;

			// query

			bool empty() const noexcept { return _M_pathname.empty(); }
			// iterators
			class iterator {
			public:
				using difference_type = std::ptrdiff_t;
				using value_type = path;
				using reference = const path&;
				using pointer = const path*;
				using iterator_category = std::bidirectional_iterator_tag;
				iterator() : _M_path(nullptr), _M_cur(), _M_at_end() { }

				iterator(const iterator&) = default;
				iterator& operator=(const iterator&) = default;

				reference operator*() const;
				pointer   operator->() const { return std::addressof(**this); }

				iterator& operator++();
				iterator  operator++(int) { auto __tmp = *this; ++_M_cur; return __tmp; }

				iterator& operator--();
				iterator  operator--(int) { auto __tmp = *this; --_M_cur; return __tmp; }

				friend bool operator==(const iterator& __lhs, const iterator& __rhs)
				{
					return __lhs._M_equals(__rhs);
				}

				friend bool operator!=(const iterator& __lhs, const iterator& __rhs)
				{
					return !__lhs._M_equals(__rhs);
				}
			private:
				friend class path;

				iterator(const path* __path, path::_List::const_iterator __iter)
					: _M_path(__path), _M_cur(__iter), _M_at_end()
				{ }

				iterator(const path* __path, bool __at_end)
					: _M_path(__path), _M_cur(), _M_at_end(__at_end)
				{ }

				bool _M_equals(iterator r) const {
					if (_M_cur != r._M_cur) return false;
					return true;
				}

				const path*                 _M_path;
				path::_List::const_iterator _M_cur;
				bool                        _M_at_end;  // only used when type != _Multi

			};
			typedef iterator const_iterator;

			iterator begin() const;
			iterator end() const;

		};

		bool exists(const path&);
		bool exists(const path&, std::error_code &ec) noexcept;
		bool is_symlink(const path&);
		bool is_symlink(const path&, std::error_code &ec) noexcept;
		path read_symlink(const path&);
		path read_symlink(const path&, std::error_code &ec) noexcept;
		bool is_regular_file(const path&);
		bool is_regular_file(const path&, std::error_code &ec) noexcept;
		bool is_directory(const path&);
		bool is_directory(const path&, std::error_code &ec) noexcept;
		bool create_directory(const path& p);
		bool create_directory(const path& p, std::error_code& ec) noexcept;
		bool create_directories(const path& p);
		bool create_directories(const path& p, std::error_code& ec) noexcept;

	}
}
