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

## Functions

The attributes have a uniform programming interface. This interface allows you
to read attributes, add attributes, check for the existence of attributes, et
cetera. 

### Learning about attributes

### Deleting and renaming attributes

### Reading attributes

### Writing attributes

