3-D Structure Import Program
=============================

This program reads ADDA and DDSCAT-formatted shapes, and then stores them
in HDF5 files. It provides an example of how to create and manipulate
shapes.

## Example Usages:

To import a file or folder (shape.dat) into a database (db-shapes.hdf5):
```
3d_structures_beta --from shape.dat --to db-shapes.hdf5
```

To import a folder containing shape files into a specific path within a database:
```
3d_structures_beta --from shapes/fsu/oblate --to db-shapes.hdf5 --db-folder=fsu/oblate
```

To import, without hdf5 file compression and assuming an interlattice spacing of 40 um:
```
3d_structures_beta --from test.dat --to db-shapes.hdf5 --compression-level=0 --resolution=40
```

To forcibly overwrite an output file:
```
3d_structures_beta --from test.dat --to db-shapes.hdf5 --truncate
```

## Program options

| Option | Input type | Description |
| -----  | ---------- | ----------- |
| from   | string     | Path where shapes are read from. Can be a file, or a folder. If a folder, then every .dat, .shp, .txt and .shape file is read. |
| to     | string     | Path where shapes are written to. |
| db-folder | string  | The path within the database to write to. |
| resolution | float  | The lattice spacing (in um). |
| compression-level | number (0-9) | The level of compression used by zlib. 0 = no compression, 9 = max compression. |
| nc4-compat | bool | Set to 0 to disable NetCDF4 compatability. Produces marginally smaller output files. |
| truncate | none | Set to overwrite any output files that exist. |

