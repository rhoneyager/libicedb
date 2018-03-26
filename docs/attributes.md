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

## Usage

Each attribute is stored in an HDF5 object.
When read or written, an attribute is encapsulated in an icedb::Attributes::Attribute<T> template class.

```
namespace icedb {
	namespace Attributes {
		template <class DataType> class Attribute {
		public:
			typedef DataType type;
			std::vector<DataType> data;
			std::string name;
			std::vector<size_t> dimensionality;
			bool isArray() const;

			Attribute(const std::string &name);
			Attribute(const std::string &name, DataType val);
			Attribute(const std::string &name, std::initializer_list<size_t> dims, std::initializer_list<DataType> data);
		};
	}
}
```

Attributes have a name, dimensions, and a datatype.
Here is how you can define an attribute:
```
		icedb::Attributes::Attribute<double> attrDouble1(
			"TestDouble1", { 1 }, { 2.0 });
```
This creates an attribute, called "TestDouble1", with a double type,
expressed as a one-dimensional array with a single value (1) in this dimension,
with a value of 2.0.
We can write this attribute to a group using:
```
		grpTest1->writeAttribute(attrDouble1);
```

In HDF5, attributes can be multi-dimensional, but in netCDF, they may
have only a single dimension. Here is how to define a single-dimensional
vector of four floating-point values:
```
		icedb::Attributes::Attribute<float> attrFloat2(
			"TestFloat2",
			{ 4 },
			{ 1.0f, 2.0f, 2.5f, 3.0f });
		grpTest1->writeAttribute(attrFloat2);
```

HDF5 supports multi-dimensional attributes, but NetCDF does not. Since icedb files are
supposed to be accessible to either NetCDF or HDF5, we likewise do not support multi-dimensional
attributes. However, if you want to use these attributes in your own project, then
a multi-dimensional attribute may be defined this way:
```
		icedb::Attributes::Attribute<float> attrFloat3(
			"TestFloat3",
			{ 2, 3 },
			{ 1.0f, 2.0f, 3.1f, 4.2f, 5.3f, 6.0f});
```

Also note, there is a difference between a vector (a one-dimensional array)
and a two-dimensional array that has one of the dimension lengths set to one.
For example, here is how to define a 1x3 matrix. It will not work with netCDF.
```
		icedb::Attributes::Attribute<float> attrFloat4(
			"TestFloat4",
			{ 1, 3 },
			{ 4.2f, 5.3f, 6.0f });
```

For ease of typing, we can write attributes more easily using lines like these:
```
		grpTest1->writeAttribute(icedb::Attributes::Attribute<uint64_t>("TestInt5", { 1 }, { 65536 }));
```
or
```
		grpTest1->writeAttribute<uint64_t>("TestInt5", {1}, {65536});
```

Attributes can be floats, doubles, signed and unsigned integers (int64_t, uint64_t, int32_t, uint32_t, int16_t, uint16_t, int8_t, uint8_t),
and strings of characters (std::string). Any other types are invalid. I might extend the interface to support these later on, but there isn't
much need to do so.
```
		grpTest1->writeAttribute(icedb::Attributes::Attribute<std::string>("TestString6", "Test string 6"));
```

Currently, there is one restriction for writing strings: only a single string can be written to
an attribute. So, this statement will not work:
```
grpTest1->writeAttribute(icedb::Attributes::Attribute<std::string>("TestStringSet7", { 2 }, { "Test string 7", "TS 8" }));
```
This will eventually be fixed.

Of course, you will also want to be able to read attributes. The syntax is similar.
```
		icedb::Attributes::Attribute<std::string> readTS6 = grpTest1->readAttribute<std::string>("TestString6");
		std::cout << "We just read TestString6 = " << readTS6.data[0] << std::endl;
```

## Functions

The attributes have a uniform programming interface. 
This interface allows you to read attributes, add 
attributes, check for the existence of attributes, et
cetera. 

All objects that can contain attributes inherit from the CanHaveAttributes class.
This class provides functions for listing attributes, getting their stored types,
reading, writing and deleting attributes.

