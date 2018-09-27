---
title: Sample applications
tags: [getting_started, installation]
last_updated: September 26, 2018
keywords: sample
summary: "A few example applications accompany icedb. This page describes them."
sidebar: mydoc_sidebar
permalink: sample_apps.html
folder: mydoc
---


## 3-D Structure Import Program

This program reads shapes, and then stores them
in HDF5 files. It provides an example of how to create and manipulate
shapes.

### Example Usages:

To import a file or folder (shape.dat, shape.nc) into a new, empty database (shape.hdf5):
```
icedb-3d_structures_importer --in shape.dat --out shape.hdf5 --truncate
```

### Program options

| Group | Option | Input type | Description |
| ----- | -----  | ---------- | ----------- |
| General | config-file | string | Use this option to read a file containing program options, such as metadata. Options are specified, once per line, as OPTION=VALUE pairs. |
| I/O   | in   | strings     | Path where shapes are read from. This may be a list of folders and files. If a directory is encountered, then it is recursively searched for shapes. The recursive search behavior is controlled by the in-matching-extensions option. |
| I/O   | in-format | string | The format of the input files. The format of the input files. This needs to be specified when the inputs are NetCDF or HDF5 files, as there are multiple conventions for these files that cannot be easily autodetected. Options: text, psu. |
| I/O   | in-nosearch | boolean | Set this option if you want to read in a set of files whose paths are exactly specified on the command line. This option allows for the finest control of input file selection. |
| I/O   | in-matching-extensions | strings | Use this to override the extensions of files to match (e.g. .adda .shp) when searching for valid shapes in directories specified in --in. |
| I/O   | out     | string     | Path where shapes are written to. |
| I/O   | db-path | string    | The group within the HDF5 database where the new data is written to. |
| I/O   | create | None | Create the output database if it does not exist. If it does, then exit without writing. |
| I/O   | truncate | None | Overwrite the output file. If not specified, then an already-existing output file is appended to. |
| Metadata | author | string | Name(s) of the person/group who generated the shape. |
| Metadata | contact-information | string | Affiliation, Contact information including email of the person/group who generated the scattering data. |
| Metadata | scattering-method | string | Method applied to the shape to calculate the scattering properties. | 
| Metadata | dataset-id | string  | Unique identifier for the dataset. |
| Metadata | dataset-version-major | unsigned integer  | Major version number of the dataset |
| Metadata | dataset-version-minor | unsigned integer  | Minor version number of the dataset |
| Metadata | dataset-version-revision | unsigned integer  | Revision version number of the dataset |
| Metadata | scattering_element_coordinates_scaling_factor | float | Prefactor applied to the scattering element coordinates. Defaults to 1.  | 
| Metadata | scattering_element_coordinates_units | string | Units of the scattering element coordinates. Defaults to "m" for meters. | 
| Metadata | constituent-names | string  | Specify the constituents. Pass in the format of "NUM1-NAME1,NUM2-NAME2,NUM3-NAME3" (i.e. 1-ice,2-water) |


## 3-D Structure Program

This program reads icedb-format shapes and passes them to a plugin for processing. 
Many of the plugins will write out the shape in a different format.

### Example Usages:

To import a file or folder (shape.dat, shape.nc) into a database (shape.hdf5):
```
icedb-3d_structures --in shape.hdf5 --to shape.silo
```

### Program options

| Group | Option | Input type | Description |
| ----- | -----  | ---------- | ----------- |
| General | config-file | string | Use this option to read a file containing program options, such as metadata. Options are specified, once per line, as OPTION=VALUE pairs. |
| I/O   | in   | string      | Database where shapes are read from. |
| I/O   | db-path | string    | The path _within_ the input database where the shape is read from. |
| I/O   | out   | string     | Path where shapes are written to. |
| I/O   | out-format   | string     | The format of the output object. |
| I/O   | create | None | Create the output database if it does not exist. If it does, then exit without writing. |
| I/O   | truncate | None | Overwrite the output file. If not specified, then an already-existing output file is appended to. |

## Scattering Phase Functions Import Program

**TODO: Document this!**

## icedb-units

This is a program designed to perform basic unit interconversions.
For now, only basic unit conversions between common mass, length, density, temperature
and frequency units is supported. Eventually, a udunits-based interface is envisioned.

### How to run

Execution is relatively simple. Just run the program. If no arguments are passed,
then follow the prompts specifying the input quantity, input units and output units.

The code has support for performing spectral conversions (i.e. frequency to wavelength).
You will be prompted if you wish to do this.

If the units are convertible, then a value is output. If the units cannot be converted,
then an ICEDB_LOG_ERROR message is presented.

###Command-line arguments:

| Option | Required type | Description |
| ------ | ------------- | ----------- |
| --input (-i) | float | This is the input quantity (sans units) |
| --input-units (-u) | string | These are the input units |
| --output-units (-o) | string | The output units |
| --spec | \<none\> | Pass this option if a spectral conversion is desired. |

The input quantity, input units and output units options are also positional.
So, the command `icedb-units 10 m um` will convert 10 meters to micrometers.
**Note: Negative quantities will mess up the positional parsing. In these cases, specify the input value with -i.**

## icedb-version

**TODO: Document this!**

## icedb-refract

This program calculates the refractive index of a bulk medium, such as solid ice or liquid water.
Mixtures (and the effective medium approximation) will be handled in a separate application.

### How to run

###Command-line arguments:

| Option | Required type | Description |
| ------ | ------------- | ----------- |
| --list-all | none | List all possible refractive index providers |
| --list-subst | string | List all refractive index providers for a given substance (viz. water, ice) |
| --subst | string | The substance of interest |
| --freq | double | Frequency |
| --freq-units | string | Units for the frequency (defaults to GHz) |
| --temp | double | Temperature |
| --temp-units | string | Units for the temperature (defaults to K) |

The arguments are also positional, with ordering of substance, frequency, frequency units, temperature,
temperature units. The temperature and associated units are optional.

**Note: when entering temperatures in degrees Celsius, negative temperatures cannot be specified using positional arguments.
In this case, you must specify temperature using ```--temp {temp}```**

This works:

```icedb-refract ice 30 GHz 263 K```

This also works:

```icedb-refract -f 18.7 GHz --subst ice```

So does this:

```icedb-refract --subst water -T 10 --temp-units C -f 20 --freq-units um```
