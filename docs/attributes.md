Attributes
==============

The scattering database uses attributes to hold metadata about data and files.
The attributes in the API library are designed to match their HDF5/netCDF 
counterparts. Both files and datasets may be tagged with attributes.

## What is an attribute?

From the NetCDF manual:

"Attributes are used to store data about the data 
(ancillary data or metadata), similar in many ways 
to the information stored in data dictionaries and 
schema in conventional database systems. 
Most attributes provide information about a specific 
variable. These are identified by the name (or ID) 
of that variable, together with the name of the attribute.

An attribute has an associated
file / folder / group / table, a name, a data type, a length, 
and a value. The current version treats all attributes as 
matrices; scalar values are treated as single-element vectors.
"

## Underlying implementation

Internally, each attribute is stored using a C-style structure:

```c
struct ICEDB_attr {
	const ICEDB_attr_ftable *funcs; ///< Function table
	ICEDB_fs_hnd* parent; ///< The parent (container) of the attribute. May be NULL, but if non-NULL, then the parent must still EXIST, or else undefined behavior may occur upon write.
	ICEDB_attr_DATA data; ///< The attribute data. Expressed as a union.
	ICEDB_DATA_TYPES type; ///< The type of data.
	size_t numDims; ///< The number of dimensions of the data.
	size_t *dims; ///< The dimensions of the data.
	size_t sizeBytes; ///< The size of the data, in __bytes__. 
	size_t sizeElems; ///< The size of the data, in number of elements.
	char* name; ///< The name of the attribute. A NULL-terminated string.
};

/// Allows you to read / write the data according to different data types.
union ICEDB_attr_DATA {
	int8_t* i8t;
	int16_t* i16t;
	int32_t* i32t;
	int64_t* i64t;
	intmax_t* imaxt;
	intptr_t* iptrt;
	uint8_t* ui8t;
	uint16_t* ui16t;
	uint32_t* ui32t;
	uint64_t* ui64t;
	uintmax_t* uimaxt;
	uintptr_t* uiptrt;
	float* ft;
	double* dt;
	char* ct;
	void* vt;
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
enum ICEDB_attr_TYPES {
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

All attributes are allocated in blocks of 8 bytes. 
There should be relatively few attributes in each 
database, so any inefficiencies in implementation 
are ignored.

## Functions

The attributes have a uniform programming interface. 
This interface allows you to read attributes, add 
attributes, check for the existence of attributes, et
cetera. 

The basic functions are combined in the ICEDB\_funcs\_attr\_container struct.

### Creating attributes

```c
ICEDB_attr* ICEDB_attr_create(
	ICEDB_fs_hnd* parent,
	const char* name,
	ICEDB_DATA_TYPES type,
	size_t numDims,
	const size_t *dims);
```

This function creates an attribute that is attached to 
some parent object. This parent can be the file handle 
for a group (i.e. a folder) or a table. The attribute has
a name, a data type, and dimensions.

The attribute also needs for you to set the data before
writing. The ICEDB\_attr structure has a member variable, 
named data, that can express the attribute's values using 
different data types (e.g. as an unsigned 8-byte integer, 
as a float). The data array is pre-allocated to the correct
size. You just have to write into it.

Once the data is populated, call the ICEDB\_attr\_write function.

### Learning about attributes

- To get number of attributes: use the ICEDB\_attr\_getNumAttrs function.

```c
size_t ICEDB_attr_getNumAttrs(const ICEDB_fs_hnd* p, 
	ICEDB_OUT ICEDB_error_code* err);
```

This function takes, as arguments, a filesystem handle (basically,
a folder), and outputs the number of attributes and an optional 
error code. If err != 0, then chech the error system for the message.

- Get attribute name (by number)

```c
typedef const char*(*ICEDB_attr_getName_f)(
	const ICEDB_fs_hnd* p,
	size_t attrnum,
	ICEDB_OPTIONAL size_t inPathSize,
	ICEDB_OUT size_t* outPathSize,
	ICEDB_OUT char ** const bufPath,
	ICEDB_OPTIONAL ICEDB_OUT ICEDB_free_charIPP_f * const deallocator);
```

This function takes a file handle and an attribute number in the 
range [0, total number of attributes). The rest of the function
looks complicated because C-style strings are hard to allocate
and deallocate. The function can be passed either a pre-allocated 
array of memory in which to write the C-string, or it can return
a newly-allocated string, which after consumption must be freed
by calling the provided deallocator function pointer.

- Does a particular attribute exist?

```c
bool ICEDB_attr_exists(
	const ICEDB_fs_hnd* p,
	const char* name, 
	ICEDB_OUT ICEDB_error_code* err);
```

Does an attribute with a certain name exist? If an error occurs,
err is nonzero and the return value has no meaning.

- Get attribute type

The ICEDB\_attr structure has a type field that contains the type.
Open the attribute, and read the type.

- Get attribute size

The ICEDB\_attr structure has two size fields: the size in elements
of the attribute's type, and the size in bytes. Open the attribute,
and read the size.

- Get attribute dimensions

The ICEDB\_attr structure has two dimension variables: the number
of dimensions and an array of dimensions.

### Copying, deleting and renaming attributes

- Delete attribute
- Rename attribute
- Copy attribute

### Reading attributes

- Read entire attribute as type

Open the attribute, and read the 'data' field.

### Writing attributes

- Write entire attribute

Open the attribute, and write to the data field, according to 
whatever builtin type matches the data.

## Error codes

The attributes code can throw many errors. Functions that return a 
pointer will return NULL (0) if an error has occurred. Functions 
that return a true/false value to indicate success will return
false on error. Some functions also provide an error code, either by
the function return or through an output pointer. Consult the 
error system documentation regarding how to process errors.

Some common attribute errors are:
- Attribute does not exist (i.e. when opening)
- Attribute already exists (when making a new attribute)
- Filesystem handle is invalid (attributes are attached to an object, and no valid object was provided)
- The attribute is read-only (if the underlying file / folder / table / group is not writable)


