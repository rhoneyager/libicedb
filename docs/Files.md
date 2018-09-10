Files
----------
\brief This page describes how to open and close HDF5 files.

In HH, all Files are Groups. So, all of the functions that work for Groups work for Files, too.

See [Groups.md](Groups.md) for the general discussion.

All of these functions are defined in lib/deps/HH/Files.hpp.

There are some special functions that pertain only to files.

## Opening and creating a file:

To open or create a file, call either:
```
static HH::File HH::File::openFile(
	gsl::not_null<const char*> filename,
	unsigned int FileOpenFlags,
	HH::HH_hid_t FileAccessPlist = H5P_DEFAULT);
```
or
```
static HH::File HH::File::createFile(
	gsl::not_null<const char*> filename,
	unsigned int FileCreateFlags,
	HH::HH_hid_t FileCreationPlist = H5P_DEFAULT,
	HH::HH_hid_t FileAccessPlist = H5P_DEFAULT);
```

- filename is, of course, the file's name. It should be a character array, and cannot be NULL.
- FileCreateFlags and FileOpenFlags are the flags used in accessing the file. These are used by the backend
  HDF5 library. Valid options are listed at https://support.hdfgroup.org/HDF5/doc/RM/RM_H5F.html#File-Create.
  Valid options are H5F_ACC_TRUNC, H5F_ACC_EXCL, H5F_ACC_RDWR, H5F_ACC_RDONLY.



## Other functions:

Get information about a file:
```
HH_NODISCARD herr_t get_info(H5F_info_t &info) const;
```

Load an image of an already-opened HDF5 file into system memory:
```
HH_NODISCARD static ssize_t get_file_image(
	HH_hid_t file_id,
	void* buf_ptr,
	size_t buf_len);
```

Open a file image that is loaded into system memory as a regular HDF5 file:
```
HH_NODISCARD static File open_file_image(
	not_null<void*> buf_ptr,
	size_t buf_size,
	unsigned int flags);
```

Create a new file image (i.e. a file that exists purely in memory):
- filename is the file to write, if backing_store_in_file == true
- block_allocation_len is the size of each new allocation as the file grows
- backing_store_in_file determines whether a physical file is written upon close.
```
HH_NODISCARD static File create_file_image(
	not_null<const char*> filename,
	size_t block_allocation_len = 10000000, // 10 MB
	bool backing_store_in_file = false,
	HH_hid_t ImageCreationPlist = HH_hid_t(H5Pcreate(H5P_FILE_ACCESS), Closers::CloseHDF5PropertyList::CloseP));
```
