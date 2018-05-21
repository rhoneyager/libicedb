#pragma once
#include <hdf5.h>
#include <memory>
#include <functional>
#include <gsl/gsl_assert>
#include "Handles_HDF.hpp"
#include "Handles_Valid.hpp"

namespace HH {
	namespace Handles {

		struct Handle_base_b {}; ///< For containerization
		template <typename HandleType, class InvalidValueClass>
		struct Handle_base : public Handle_base_b {
		public:
			typedef Handle_base<HandleType, InvalidValueClass> Handle_base_t;
			typedef std::function<bool(HandleType)> InvalidValueFunction_t;
			//InvalidValueFunction_t InvalidValueFunction_f = InvalidValueClass::isInvalid;
			HandleType h = NULL;
			bool valid() const {
				if (_isInvalid) return false;
				return !InvalidValueClass::isInvalid(h);
			}
			void swap(Handle_base_t& rhs) {
				std::swap(this->h, rhs.h);
				std::swap(this->_isInvalid, rhs._isInvalid);
			}

			Handle_base(HandleType newh, bool isInvalid = false) : h(newh), _isInvalid(isInvalid) {}
			bool isInvalid() const { return _isInvalid; }
		protected:
			bool _isInvalid = false;
			/// Only call from the containing class. Used when transfering the scoped handle to
			/// another container. Prevents the close routine from activating.
			void _invalidate() { h = NULL; _isInvalid = true; }
		};

		template <typename HandleType, class CloseMethod, class InvalidValueClass>
		struct ClosableHandle :
			public Handle_base<HandleType, InvalidValueClass>
		{
			~ClosableHandle() {
				bool res = close();
				if (!res) abort();
			}
			ClosableHandle(HandleType newh, bool isInvalid = false, bool noClose = false)
				: Handle_base(newh, isInvalid), _noClose(noClose) {}
			bool close() {
				if (!valid()) return true; // Lie
				if (_noClose) return true;
				if (CloseMethod::Close(h)) {
					_invalidate();
					return true;
				}
				return false; // Error on closing.
			}
			bool DoesNotClose() const { return _noClose; }
		private:
			bool _noClose;
		};

		template <typename HandleType, class CloseMethod, class InvalidValueClass>
		struct ScopedHandle;

		/// A class that encapsulates a handle in a shared-memory object.
		/// Handle is released immediately when the last object copy closes.
		/// \note Potentially unsafe across DLL boundaries.
		template <typename HandleType, class CloseMethod, class InvalidValueClass>
		struct SharedHandle :
			public ClosableHandle<HandleType, CloseMethod, InvalidValueClass>,
			public std::enable_shared_from_this<SharedHandle<HandleType, CloseMethod, InvalidValueClass> >
		{
		private:
			SharedHandle() {}
		public:
			typedef SharedHandle<HandleType, CloseMethod, InvalidValueClass> thisSharedHandle_t;
			~SharedHandle() {}
			SharedHandle(HandleType newh) : ClosableHandle(newh) {}
			SharedHandle(ScopedHandle<HandleType, CloseMethod, InvalidValueClass> &&old)
				: ClosableHandle(old.h, oldDoesNotClose().old.isInvalid()) {
			}
			static thisSharedHandle_t generate(HandleType newh) {
				return std::make_shared<thisSharedHandle_t>(newh);
			}
			static thisSharedHandle_t generate(ScopedHandle<HandleType, CloseMethod, InvalidValueClass> &&old) {
				return std::make_shared<thisSharedHandle_t>(old);
			}
		};

		/// A scoped handle container. The handle is released immediately when out of scope.
		template <typename HandleType, class CloseMethod, class InvalidValueClass>
		struct ScopedHandle :
			public ClosableHandle<HandleType, CloseMethod, InvalidValueClass>
		{
			typedef ScopedHandle<HandleType, CloseMethod, InvalidValueClass> thisScopedHandle_t;
			typedef std::shared_ptr<ScopedHandle<HandleType, CloseMethod, InvalidValueClass> > thisSharedHandle_t;

			~ScopedHandle() {}
			ScopedHandle(HandleType newh, bool isInvalid = false, bool noClose = false)
				: ClosableHandle(newh, isInvalid, noClose)
			{}
			ScopedHandle(const ScopedHandle<HandleType, CloseMethod, InvalidValueClass> &rhs)
				: ScopedHandle(rhs.h, rhs.valid(), true)
			{}
			ScopedHandle(ScopedHandle<HandleType, CloseMethod, InvalidValueClass> &&old)
				: ScopedHandle(old.h, old.valid(), old.DoesNotClose()) {
				if (!old.valid()) _invalidate();
				old._invalidate();
			}
			bool operator==(const thisScopedHandle_t&) = delete;
			bool operator!=(const thisScopedHandle_t&) = delete;
			bool operator<(const thisScopedHandle_t&) = delete;
			thisScopedHandle_t& operator=(const thisScopedHandle_t&) = delete; ///< Handles can be moved, not copied.
			thisSharedHandle_t make_shared() {
				return std::make_shared<thisSharedHandle_t>(std::move(*this));
			}
		};

		typedef ScopedHandle<hid_t, Closers::CloseHDF5File, InvalidHDF5Handle> H5F_ScopedHandle;
		typedef ScopedHandle<hid_t, Closers::CloseHDF5Dataset, InvalidHDF5Handle> H5D_ScopedHandle;
		typedef ScopedHandle<hid_t, Closers::CloseHDF5Dataspace, InvalidHDF5Handle> H5S_ScopedHandle;
		typedef ScopedHandle<hid_t, Closers::CloseHDF5Datatype, InvalidHDF5Handle> H5T_ScopedHandle;
		typedef ScopedHandle<hid_t, Closers::CloseHDF5Group, InvalidHDF5Handle> H5G_ScopedHandle;
		typedef ScopedHandle<hid_t, Closers::CloseHDF5Attribute, InvalidHDF5Handle> H5A_ScopedHandle;
		typedef ScopedHandle<hid_t, Closers::CloseHDF5PropertyList, InvalidHDF5Handle> H5P_ScopedHandle;
		typedef ScopedHandle<hid_t, Closers::DoNotClose, InvalidHDF5Handle> H5_fundamental_ScopedHandle;
		typedef Handle_base<hid_t, InvalidHDF5Handle> HH_hid_t;


		typedef HH_hid_t H5_Handle;

		inline H5_Handle getPlist_default() {
			return H5_Handle{ H5P_DEFAULT };
		}
	}
}
