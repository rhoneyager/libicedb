#pragma once
#include "defs.h"
#include <HH/Groups.hpp>

namespace icedb {
	/// The virtual mount namespace
	namespace vmount {
		class Virtual_Mount : public HH::Group {
		public:
			virtual ~Virtual_Mount() {}
			/// This entirely overrides the normal group mounting mechanism.
			/// It makes symbolic or external links in the virtual file space.
			/// If conflicts exist with the links, then the links get masked.
			HH_NODISCARD herr_t mount(
				gsl::not_null<const char*> destination_groupname,
				HH::HH_hid_t source,
				HH::HH_hid_t MountPlist = H5P_DEFAULT)
			{
				return HH::Group::mount(destination_groupname, source, MountPlist);
			}
			/// This entirely overrides the normal unmounting mechanism.
			/// It removes any broken symbolic links. If masking has occurred, then
			/// it restores the next avilable copy of the linked data.
			HH_NODISCARD herr_t unmount(
				const char* mountpoint)
			{
				return HH::Group::unmount(mountpoint);
			}
			/// When a backend's data changes, call this function to update the virtual mount.
			HH_NODISCARD herr_t update(const char* mountpoint)
			{
				return -1;
			}
			/// Convenience function to get a group that can be written to
			HH::Group getWritable() const {
				return HH::HH_hid_t::dummy();
			}
		};
	}

	/// The Particle Physical Properties namespace
	namespace PPP {

	}
}