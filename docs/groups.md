Groups
----------

\brief This page describes how to manipulate Groups in HDF5 files. This includes accessing datasets and attributes.

HDF5 and NetCDF 4 files have a directory-like structure. Files can contain groups, datasets and attributes.
Groups can also contain more groups, datasets and attributes.

All of these functions are defined in lib/deps/HH/Groups.hpp.


## Manipulating groups

Does a group exist as a child of the current object?
```
htri_t HH::Group::exists(
	gsl::not_null<const char*> name,
	HH::HH_hid_t LinkAccessPlist = H5P_DEFAULT);
```

To create a group:
```
HH::Group HH::Group::create(
	gsl::not_null<const char*> name,
	HH::HH_hid_t LinkCreationPlist = H5P_DEFAULT,
	HH::HH_hid_t GroupCreationPlist = H5P_DEFAULT,
	HH::HH_hid_t GroupAccessPlist = H5P_DEFAULT)
```

To open a group:
```
HH::Group HH::Group::open(
	gsl::not_null<const char*> name,
	HH::HH_hid_t GroupAccessPlist = H5P_DEFAULT);
```

## Accessing objects inside of Groups

```
Has_Attributes atts;
Has_Datasets dsets;
```

