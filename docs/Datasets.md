Datasets
==============

Most of the data in the scattering database is stored using tables. 
In HDF5, these are called 
[datasets](https://support.hdfgroup.org/HDF5/Tutor/crtdat.html#def), and
some dataset subsets are called [tables](https://support.hdfgroup.org/HDF5/doc/HL/RM_H5TB.html).
In netCDF, these objects are called 
[variables](https://www.unidata.ucar.edu/software/netcdf/docs/netcdf_data_set_components.html#variables).

All datasets may contain attributes.

## What is a datasets?

A datasets is a multidimensional array of data elements, together with supporting metadata. 
Tables may contain different types of data, such as integers, floating point numbers and character arrays.
In icedb, all of the elements of a datasets must have the same data type. This restriction is the same as in
netCDF, although HDF5 has a more relaxed implementation.

Datasets are usually much larger than attributes. Unlike attributes, datasets are not automatically
loaded into memory. Datasets support partial read and write operations.

## Usage

Creating a table is easy.

This command will make a table with two rows and three columns.
The data are inserted into the table in row-major form.
See Table.hpp to list the methods for adding larger data into tables.
 For these other methods, the sizing of the table may be de-coupled
from writing data to the tables.
```
auto tblTest = grpTest1->createTable<int64_t>("Test_table_1", { 2, 3 }, { -2, -1, 0, 1, 2, 3 });
```

Under their current implementation, table dimensions are entirely fixed and are
set at creation. In the future, support for extendible dimensions will be added.

Tables support the same data types as attributes (uint64_t, int64_t, float, double).
Strings are on the to-do list.
Unlike with attributes, tables can be entirely multi-dimensional.

Tables can have attributes.
```
tblTest->writeAttribute<int64_t>(icedb::Attributes::Attribute<int64_t>("TestInt7", 5));
tblTest->writeAttribute<int64_t>(icedb::Attributes::Attribute<int64_t>("TestInt8", { 4 }, { 1, -1, 2, -2 }));
```

For compatability with netCDF, each data table must be associated with one
or more "dimension scales". To create and associate these scales with a table,
commands like these are needed:
```
auto tblDims1 = grpTest1->createTable<int64_t>("X_axis", { 3 }, { 9, 10, 11 });
auto tblDims2 = grpTest1->createTable<int64_t>("Y_axis", { 2 }, { 4, 6 });
tblDims1->setDimensionScale("X_axis");
tblDims2->setDimensionScale("Y_axis");
tblTest->attachDimensionScale(0, tblDims2.get());
tblTest->attachDimensionScale(1, tblDims1.get());
```
