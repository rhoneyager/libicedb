3-D Structure Import Program
=============================

This program reads ADDA and DDSCAT-formatted shapes, and then stores them
in HDF5 files. It provides an example of how to create and manipulate
shapes.

## Example Usages:

To import a file or folder (shape.dat) into a database (shape.hdf5):
```
3d_structures_example --from shape.dat --to shape.hdf5
```

## Program options

| Option | Input type | Description |
| -----  | ---------- | ----------- |
| from   | string     | Path where shapes are read from. For this simple example, this must be a file containing a single shape. |
| to     | string     | Path where shapes are written to. |
| resolution | float  | The lattice spacing (in um). |

