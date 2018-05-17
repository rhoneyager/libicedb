3-D Structure Import Program
=============================

This program reads shapes, and then stores them
in HDF5 files. It provides an example of how to create and manipulate
shapes.

## Example Usages:

To import a file or folder (shape.dat, shape.nc) into a database (shape.hdf5):
```
3d_structures_singlethreaded --from shape.dat --to shape.hdf5
```

## Program options

| Group | Option | Input type | Description |
| ----- | -----  | ---------- | ----------- |
| General | config-file | string | Use this option to read a file containing program options, such as metadata. Options are specified, once per line, as OPTION=VALUE pairs. |
| I/O   | from   | strings     | Path where shapes are read from. This may be a list of folders and files. If a directory is encountered, then it is recursively searched for shapes. The recursive search behavior is controlled by the from-matching-extensions option. |
| I/O   | from-format | string | The format of the input files. The format of the input files. This needs to be specified when the inputs are NetCDF or HDF5 files, as there are multiple conventions for these files that cannot be easily autodetected. Options: text, psu. |
| I/O   | from-nosearch | boolean | Set this option if you want to read in a set of files whose paths are exactly specified on the command line. This option allows for the finest control of input file selection. |
| I/O   | from-matching-extensions | strings | Use this to override the extensions of files to match (e.g. .adda .shp) when searching for valid shapes in directories specified in --from. |
| I/O   | to     | string     | Path where shapes are written to. |
| I/O   | db-path | string    | The group within the HDF5 database where the new data is written to. |
| I/O   | create | None | Create the output database if it does not exist. If it does, then exit without writing. |
| I/O   | truncate | None | Overwrite the output file. If not specified, then an already-existing output file is appended to. |
| Metadata | resolution | float  | The lattice spacing (in um). |
| Metadata | author | string | Name(s) of the person/group who generated the shape. |
| Metadata | contact-information | string | Affiliation, Contact information including email of the person/group who generated the scattering data. |
| Metadata | scattering-method | string | Method applied to the shape to calculate the scattering properties. | 
