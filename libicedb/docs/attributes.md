Attributes
==============

The scattering database uses attributes to hold metadata about data and files.
The attributes in the API library are designed to match their netCDF 
counterparts. Both files and datasets may be tagged with attributes.

## Underlying implementation

Internally, each attribute is stored using a C-style structure:

```c
struct ICEDB_ATTR {
	ICEDB_ATTR_DATA data;
	ICEDB_ATTR_TYPES type;
	size_t size;
	bool hasSize;
};
```

The actual data in each attribute is stored using a C union, which represents 
the same region of memory as different types. To avoid confusion about the 
actual type of the data, the 'type' variable of the structure lists the actual
type. A pointer to the data is stored in the 'data' variable. The data variable
is allocated dynamically on the heap. The data can contain more than a single 
value; fixed and variable-length arrays are allowed. When the array size is 
fixed, 'hasSize' is true and the 'size' variable represents the number of 
objects in the array. That is, if the stored data is a 32-bit integer, then 
'size' refers to the number of 32-bit integers stored in 'data'. For 
variable-length arrays, hasSize is false and the data must be null-terminated.
Variable length arrays are most useful for storing character data.

### Attribute types

```c
enum ICEDB_ATTR_TYPES {
	ICEDB_TYPE_CHAR, // NC_CHAR, a single 8-bit character, char*
	ICEDB_TYPE_INT8, // NC_BYTE, 8-bit integer, int8_t*
	ICEDB_TYPE_UINT8, // NC_UBYTE, unsigned 8-bit integer, uint8_t*
	ICEDB_TYPE_UINT16, // NC_USHORT, unsigned 16-bit integer, uint16_t*
	ICEDB_TYPE_INT16, // NC_SHORT, signed 16-bit integer, int16_t*
	ICEDB_TYPE_UINT32, // NC_UINT, unsigned 32-bit integer, uint32_t*
	ICEDB_TYPE_INT32, // NC_INT (or NC_LONG), signed 32-bit integer, int32_t*
	ICEDB_TYPE_UINT64, // NC_UINT64, unsigned 64-bit integer, uint64_t*
	ICEDB_TYPE_INT64, // NC_INT64, signed 64-bit integer, int64_t*
	ICEDB_TYPE_FLOAT, // NC_FLOAT, 32-bit floating point, float*
	ICEDB_TYPE_DOUBLE, // NC_DOUBLE, 64-bit floating point, double*
	// These have no corresponding NetCDF type. They never get saved by themselves, but contain pointers to things like string arrays, which are NetCDF objects.
	ICEDB_TYPE_INTMAX,
	ICEDB_TYPE_INTPTR, // used for pointers to pointers of data
	ICEDB_TYPE_UINTMAX,
	ICEDB_TYPE_UINTPTR // used for pointers to pointers of data
};
```

All attributes are allocated in blocks of 8 bytes. There should be relatively few attributes in each database, so any inefficiencies in implementation are ignored.

## Functions

The attributes have a uniform programming interface. This interface allows you
to read attributes, add attributes, check for the existence of attributes, et
cetera. 

### Learning about attributes

- Get number of attributes
- Does attribute exist?
- Get attribute type
- Get attribute size
- Is attribute size fixed?
- Get attribute name (by number)

### Copying, deleting and renaming attributes

- Delete attribute
- Rename attribute
- Copy attribute

### Reading attributes

- Read entire attribute as type

### Writing attributes

- Write entire attribute as type

## Error codes

- Wrong attribute type
- Attribute does not exist
