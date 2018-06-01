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
				if (_AlwaysValid) return true;
				if (_isInvalid) return false;
				return !InvalidValueClass::isInvalid(h);
			}
			void swap(Handle_base_t& rhs) {
				std::swap(this->h, rhs.h);
				std::swap(this->_isInvalid, rhs._isInvalid);
				std::swap(this->_AlwaysValid, rhs._AlwaysValid);
			}
			HandleType release() { HandleType i = h; _invalidate(); return i; }
			Handle_base(HandleType newh, bool DoNotInvalidate = true, bool AlwaysValid = false) : h(newh), _isInvalid(!DoNotInvalidate), _AlwaysValid(AlwaysValid) {}
			bool isInvalid() const { return _isInvalid; }
		protected:
			bool _isInvalid = false;
			bool _AlwaysValid = false;
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
			ClosableHandle(HandleType newh, bool DoNotInvalidate = true, bool noClose = false, bool AlwaysValid = false)
				: Handle_base(newh, DoNotInvalidate, AlwaysValid), _noClose(noClose) {}
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
		//template <typename HandleType, class CloseMethod, class InvalidValueClass>
		//struct WeakHandle;
		template <typename HandleType, class InvalidValueClass>
		using WeakHandle = Handle_base<HandleType, InvalidValueClass>;

		/// A class that encapsulates a handle in a shared-memory object.
		/// Handle is released immediately when the last object copy closes.
		/// \note Potentially unsafe across DLL boundaries.
		/// \todo Finish
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
		///
		/// The ScopedHandle HAS OWNERSHIP of its object.
		/// \see WeakHandle for a convertible class that does NOT get ownership, and which can be transferred.
		template <typename HandleType, class CloseMethod, class InvalidValueClass>
		struct ScopedHandle :
			public ClosableHandle<HandleType, CloseMethod, InvalidValueClass>
		{
			typedef ScopedHandle<HandleType, CloseMethod, InvalidValueClass> thisScopedHandle_t;
			typedef std::shared_ptr<ScopedHandle<HandleType, CloseMethod, InvalidValueClass> > thisSharedHandle_t;
			typedef WeakHandle<HandleType, InvalidValueClass> thisWeakHandle_t;

			virtual ~ScopedHandle() {}
			ScopedHandle(HandleType newh, bool DoNotInvalidate = true, bool noClose = false, bool AlwaysValid = false)
				: ClosableHandle(newh, DoNotInvalidate, noClose, AlwaysValid)
			{}
			/// \todo Document the cases when this constructor is valid.
			/// This is used to construct temporary objects (temporarily construct a weak
			/// handle)
			///ScopedHandle(const ScopedHandle<HandleType, CloseMethod, InvalidValueClass> &rhs)
			///	: ScopedHandle(rhs.h, rhs.valid(), true)
			///{}
			ScopedHandle(ScopedHandle<HandleType, CloseMethod, InvalidValueClass> &&old)
				: ScopedHandle(old.h, old.valid(), old.DoesNotClose())
			{
				old._invalidate(); // Odd infinite loop here.
				//if (!old.valid()) {
				//	_invalidate();
				//}
				//else old._invalidate(); // Need the "else", otherwise beware of infinite freeing loops.
			}
			bool operator==(const thisScopedHandle_t&) = delete;
			bool operator!=(const thisScopedHandle_t&) = delete;
			bool operator<(const thisScopedHandle_t&) = delete;
			thisScopedHandle_t& operator=(const thisScopedHandle_t&) = delete; ///< Handles can be moved, not copied.
			//thisSharedHandle_t make_shared() {
			//	return thisSharedHandle_t(std::move(*this));
			//}

			thisWeakHandle_t getWeakHandle() const {
				bool isValid = valid();
				auto wh = thisWeakHandle_t(h, isValid);
				return wh;
			}
		};

		
		/*
		//typedef Handle_base<HandleType, InvalidValueClass> WeakHandle;

		/// A handle that does not retain ownership. Really just an alias
		template <typename HandleType, class CloseMethod, class InvalidValueClass>
		struct WeakHandle
			: public Handle_base<HandleType, InvalidValueClass>
		{
			virtual ~WeakHandle() {}
			WeakHandle(HandleType newh, bool isInvalid = false)
				: Handle_base(newh, isInvalid)
			{}
		};
		*/

		/// Scoped handle for files
		typedef ScopedHandle<hid_t, Closers::CloseHDF5File, InvalidHDF5Handle> H5F_ScopedHandle;
		/// Scoped handle for datasets
		typedef ScopedHandle<hid_t, Closers::CloseHDF5Dataset, InvalidHDF5Handle> H5D_ScopedHandle;
		/// Scoped handle for dataspaces
		typedef ScopedHandle<hid_t, Closers::CloseHDF5Dataspace, InvalidHDF5Handle> H5S_ScopedHandle;
		/// Scoped handle for datatypes
		typedef ScopedHandle<hid_t, Closers::CloseHDF5Datatype, InvalidHDF5Handle> H5T_ScopedHandle;
		/// Scoped handle for groups
		typedef ScopedHandle<hid_t, Closers::CloseHDF5Group, InvalidHDF5Handle> H5G_ScopedHandle;
		/// Scoped handle for attributes
		typedef ScopedHandle<hid_t, Closers::CloseHDF5Attribute, InvalidHDF5Handle> H5A_ScopedHandle;
		/// Scoped handle for property lists
		typedef ScopedHandle<hid_t, Closers::CloseHDF5PropertyList, InvalidHDF5Handle> H5P_ScopedHandle;
		/// Scoped handle for HDF5 intrinsic objects (constant objects built-in to the HDF5 headers)
		typedef ScopedHandle<hid_t, Closers::DoNotClose, InvalidHDF5Handle> H5_fundamental_ScopedHandle;
		/// General handle for encapsulated HDF5 objects.
		typedef Handle_base<hid_t, InvalidHDF5Handle> HH_hid_t;

	}
}
