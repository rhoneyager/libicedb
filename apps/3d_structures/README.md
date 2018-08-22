3-D Structure Program
=============================

This program reads icedb-format shapes and passes them to a plugin for processing. 
Many of the plugins will write out the shape in a different format.

## Example Usages:

To import a file or folder (shape.dat, shape.nc) into a database (shape.hdf5):
```
icedb-3d_structures_importer --in shape.hdf5 --to shape.silo
```

## Program options

| Group | Option | Input type | Description |
| ----- | -----  | ---------- | ----------- |
| General | config-file | string | Use this option to read a file containing program options, such as metadata. Options are specified, once per line, as OPTION=VALUE pairs. |
| I/O   | in   | string      | Database where shapes are read from. |
| I/O   | db-path | string    | The path _within_ the input database where the shape is read from. |
| I/O   | out   | string     | Path where shapes are written to. |
| I/O   | out-format   | string     | The format of the output object. |
| I/O   | create | None | Create the output database if it does not exist. If it does, then exit without writing. |
| I/O   | truncate | None | Overwrite the output file. If not specified, then an already-existing output file is appended to. |
