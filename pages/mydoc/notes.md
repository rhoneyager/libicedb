---
title: "Discussion Notes"
keywords: icedb libicedb discussion notes
tags: [collaboration]
sidebar: mydoc_sidebar
permalink: notes.html
summary: Short summaries of the web meeting among the development team
---
Notes uploaded by Davide Ori <dori@uni-koeln.de>

Since the last IPWG/IWSSM (Bologna 2016) meeting, a couple of people (Stefan, Jussi, Davide, Eugene, Ben) worked on an Excel workbook to describe an initial file structure and variable names (following as closely as possible the CF convention) for storing scattering datasets. The Excel file can be downloaded [here]( https://rhoneyager.github.io/libicedb/structs_metadata.html).

At the recent [snowfall workshop in Cologne](http://www.geomet.uni-koeln.de/forschung/en-optimice/issw/), we discussed how to better store the data in different level files:

- Level 0: Lowest level of processing

  1. Level 0a: Contains the 3D structure
  2. Level 0b: Contains amplitude scattering matrix (following Mischenko convention)

- Level 1: Phase Matrix (to be discussed)

- Level 2: Essential scattering properties (to be discussed)

  1. Details need to be further discussed
  2. General idea: provide similar content as Liu database (cross sections, phase function, angle averaged quantities)

- Level 3: Scattering properties integrated over PSD (to be discussed)

It might not be possible to generate all data levels from existing databases (some information might be missing),
but it should be mandatory for all dataset developers to provide at least level 0 and level 2 files.
For level 0a, an agreement was already found on how to name and define the 3D structure variables
(all information can be found in the Excel file). Ideally, the level 0 files should be enough to provide it
to an API which generates the higher level files from it. Also interpolations of variables in the 
database requested by several users (included e.g. by the [Liu DB](http://cirrus.met.fsu.edu/research/scatdb.html))
should be provided by an API which is
separated from any specific database but can be used with any dataset.

We will keep the discussions ongoing via web meetings to work out the details of the different file types.

**ENTRIES ARE IN REVERSE CHRONOLOGICAL ORDER**

## Notes from the 5th Scattering WebEx Meeting (17 May 2018)

PARTICIPANTS: Ryan - Eugene - Stefan - Davide - Ian - Jussi

Ryan attempted to share some slides beforehand, but we had problems with the mailing list see issue [#29](https://github.com/rhoneyager/libicedb/issues/29).

[MEETING SLIDES](https://github.com/rhoneyager/libicedb/files/1929718/2018-05-17.Scattering.Meeting.Slides.pdf-MISSING)

We had a lot of traffic on the libicedb github site recently (20 different IPs and more than 1000 page views)

1. Particle shape metadata
  * The import routines are running well for ordinary DDA and GMM shapefiles, but we are still missing a lot of metadata and variable description like popular netcdf attributes like *long_name* and *units*
  * Particle orientation should be moved to scattering quantities files since they are related to those properties.
  * The *command-line* argument parser is now taking a substantial number of inputs. We will move to inputfiles (text files with ARGUMENT:value pairs)
  * The Universal Unique IDentifier was a main point of discussion and still missing. Our original idea to generate it by parsing the shapefile is not efficient in case of GMM particles which have floating-point scattering element coordinates. We will rather make a unique identifier by combining *DATASET_ID* + *SHAPE_ID* + 4integers_version. The database provider will be responsible for non overlapping shape IDs within the datase (we will provide an optional hashing function for DDA particles)
  * Kwo's database files import might be trickier and will require some time
  * *scaling_factor* The concept of interdipole spacing is DDA based. We rather move to scaling factor: something that translates integer coordinates into physical units. In order to be consistent with netcdf standard (units should be an attribute) we will have a scaling_factor (value) and scaling_factor_units (meters for SI convention). This is not required for GMM floating point coordinates but we will add it in any case for consistency.
  * Chalmer's University asked for passing effective radius instead of dipole spacing. We agreed that it is important to have one (scaling_factor) as a required attribute and effective radius might be added as an optional attribute. Moreover it might be possible (for easier shapes) to implement an automatic conversion in the future
2. Next development steps
  * The natural step forward is to include some physical properties (level 0b)
  * Ryan will code the library interface and a template so that anybody can take the chance to contribute to the development of the library by adding some algorithm to compute physical properties of the scatterer
3. Miscellaneous
  * We really need to review the spreadsheets with the description of the standard
  * Ryan asks for people looking at the additional programs built ontop of the library
  * Whoever works on something on the library he/she should mention that in the project tab on the github repository. A step-by-step guide on how to use git and contribute to the repository will be published on the documentation website soon
  * **NEXT MEETING** 14 June 2018 17:00 CET (1500 UTC)

## Notes from the 4th Scattering WebEx Meeting (17 April 2018)
NASA Goddard WebEx seems to not work with PC audio but does with phones. Screen sharing is working. WebEx is
notoriously bad on Linux platforms, as it generally requires 32-bit implementations of Java packages which
are badly maintained on most distributions. It could potentially work by installing a 32-bit Linux
distribution on a virtual machine, but this is still quite time consuming. 

PARTICIPANTS: Eugene - Stefan - Ian - Patrick - Ryan - Annakaisa - Davide - Robin

[MEETING SLIDES](https://github.com/rhoneyager/libicedb/files/1929718/2018-04-17.Scattering.Meeting.Slides.pdf)

Ryan showed the new website created specifically for the libicedb code description and documentation.

First point of the discussion is to have people involved and report things to be discussed/solved in the next month.
We agreed that, even if GitHub does not provide a proper discussion tool, we should try to use the 
built-in issue tracker and project manager in order to establish a constructive feedback loop between core 
developers and test users. If those instruments become insufficient then we will think about something different.

Some techinical issues with the implementation:

We use HDF5 instead of NetCDF because the NetCDF library lacks as much cross-platform support as HDF5. However, 
libicedb is currently producing HDF5 files that are fully compatible with NetCDF specifications and thus it
should be possible to access them using any NetCDF tool/library/language. Unfortunately, Ryan discovered that the
HDF5 C++ interface is unstable; the API changes between even minor versions, and the available documentation 
does not reflect the changes that the HDF5-C++ developer team have made. As a workaround, 
Ryan will use standard HDF5 C interface and build a C++ wrapper around it. There is a parallel project he is 
developing, named HDFforHumans which would make it easier for everybody to interact with HDF5 files using C++.

The current implementation allows for great flexibility in how to structure the database 
(one file per shape or multiple shapes in one file and so on). It will be up to the user how to structure the data.

Looking forward to how to share the databases: we should take a look into git-annex functionality, and 
we can think about how to implement it. It would allow people to store their databases on different 
storage services and be linked to the library automatically. However, the immediate priority is to have people use
the library and report their experiences.

Additional programs were presented to explore possible applications of the library. People can contribute 
by sharing codes in other languages, and more experienced people can contribute to the core C++ development.
Eugene, Davide and Patrick are gonna try to help Ryan's work. The most important message should be that 
everybody is invited to contribute to the code with what he/she has, and not be frightned by the complexity of the coding part.

There is also room to contribute with non-coding stuff. Looking at the structure of the data 
highligting improvements possible and bugs, contributing to documentation and tutorials. In December, Ryan shared
a working package which people can use to try the library's functionality without troubles arising from 
lack of specific format support and details on the implementation.

Ian will talk to Matt Lammers to have him involved in order to be able to use Kwo's group webpage for database aceess.

From now on we shall try to put the notes on the code's webpage for easier navigation and better links to the development branch.

For the upcoming month:

Everybody can work with the library and share thoughts about what they can contribute so than we can split up the effort:

- Eugene: will be importing the remainder of the Penn State shape files and will keep in touch for problems.
- Stefan: will try Jussi's shapefiles as well. For the storage we can still use the U. Cologne FTP servers. Also, melted particles would be an interesting test.
- If anybody wants to try something, just e-mail Ryan and he will share instructions on how to use the
library to handle special properties of the particles.

Next meeting? 17 May works best for everybody.

## Notes from the 3rd Scattering Skype Meeting (15th November 2017)

The BAMS report article on the first International Summer Scattering Workshop has been accepted and it is waiting for editing.

The scheduling of the meeting originally kept the UTC reference for a universal coordination, but since our last daylight saving time change the 15:00 UTC can be a problem in certain parts of the world. Per Kwo-Sen Kuo's suggestion we decided to keep the schedule at 17:00 German time and share the meeting updates, while hoping that our calendar application is smart enough to account for the correct timezone.

Kwo and Ian will explore the possibility of using their institutional WebEx account to organize the meeting since Skype is not scaling well.

Kwo also annouced a new iteration of the openSSP database that will allow the user to specify PSD and m-D relations. He also plans to include polarimetry.

1. Ryan sent to everybody an example database encoded with the developing icedb library.
  * It is written in HDF5, but still is fully NetCDF compatible so everybody should be able to access it using their favourite standard NetCDF tool
  * The program automatically indexes every file in a specific folder (it expects to find shapefiles).
  * It is still missing the unique identifier functionality, but any additional property can be rather easily integrated thanks to the set of tools that are implemented in the library.
  * The example provides functions to create tables and attributes and can write tables with any dimension.
  * **task for everybody**: Try to use the example program and report for missing functionalities and bugs.

## Notes from the 2nd Scattering Skype Meeting (13th September 2017)

1. Level 0 files and routines
  * Need to establish a separate working group dedicated to the implementation of Level 0 routines
  * In particular, Ryan is asking for some help in gathering ADDA and GMM sample geometry files 
  * Eugene offered to get in touch with Ryan and to provide access to GMM sample files
  * Kuo and Ian suggest a general geometry reference (common frame of reference) and will prepare some examples
  * There was a general agreement that more detailed discussions will be most effective once we have an example Level 0 file

2. Discussion on physical particle properties
  * Jussi compiled several suggestions and questions related to the physical particle properties (see earlier entry in this forum!). He provided a short summary of his main points which was discussed afterwards:
  * Projected area along each incidence direction should be provided
  * We should decide for **a** standard definition of d\_max (multiple alternative definitions will be allowed to be included in the file but as optional variables)
  * There was some agreement that d\_max as maximum 3D distance between two dipoles inside the particle would be a better definition than the diameter of the enclosing sphere
  * Closely related to d\_max is the definition of density; most participants agreed with Jussi's suggestion for how to define density
  * Code some API that compute physical properties in a standardized way based on the shape files provided 

3. Discussion on variable naming and structuring  
  In the current variable structure we suggest to group variables into **essential** and **optional** variables for each data file type, Jussi suggested to group them into
     1. **mandatory**: everybody has to provide these following our definitions
     2. **standardized**: if somebody wants to provide these variables, he must correspond to our standard/definitions
     3. **optional**: variables that can be added by every database producer according to their definitions; these definitions **must** be provided in the meta-data.

    Ryan also suggested to not use the suffix \_standard for variables but rather include the method or paper in the suffix, e.g. particle\_max\_dimension\_3D\_distance  
    The variable particle\_scattering\_element\_composition should be optional if the particle is composed only of ice in order to reduce file size.
    Variable naming should be self explainatory. Ryan will provide suggestions.

4. Data Exchange, FTP server, Mailing List  
  In order to share/exchange data and code, an open ftp server (current space limit 100 GB) is provided by Uni. Cologne: 
    1. ftp gop.meteo.uni-koeln.de
    2. user: libscat
    3. pwd: ask Stefan or Davide

    In order to simplify email communication, we also set up a mailing list for all people involved in the regular Skype meetings:
      1. **<meteo-scattdiscuss@uni-koeln.de>**
      2. You can easily subscribe/unsubscribe and change other setting [here](https://lists.uni-koeln.de/mailman/listinfo/meteo-scattdiscuss)
      3. For more specific code discussions we set up a second mailing list called <meteo-scattcode@uni-koeln.de> change your settings for this list by following this [link](https://lists.uni-koeln.de/mailman/listinfo/meteo-scattcode)

    Users also requested an SSL encoding for snowport; this is currently work in progress


## Notes and thoughts on particle physical properties

Jussi Leinonen, August 2017

**Background**

I am currently about to create a large set of snowflake models for scattering simulations. I thought it would be good to make this dataset compatible with our ideas for the database, but I noticed that many of the particle physical properties in the spreadsheet so far lack exact definitions. I am writing this to document to express my suggestions for how those properties should be defined. Additions and comments are welcome.

**Particle physical properties**

Below, I’ll visit each particle physical property listed as so-far undefined in the spreadsheet tabs “particle\_physical\_properties” and “3d\_structure”. I give suggestions as to how they should be implemented.

* particle\_maximum\_dimension\_standard:  
 I believe this should be independent of particle orientation, so we’re left with two reasonable choices: (1) The maximum distance between any two elements in the particle or (2) The diameter of the smallest enclosing sphere of the set of elements. These are not equal in general.
  1. Can be computed by first getting the convex hull of points and then finding the maximum distance between the vertices. This can be done with this algorithm in O(n log h): [link](https://en.wikipedia.org/wiki/Chan%27s_algorithm) but I haven’t implemented it yet.
  2. Can be implemented in O(n) time using the algorithm of [Hopp and Reeve, 1996:](http://www.mel.nist.gov/msidlibrary/doc/hopp95.pdf). I have an implementation of this in my aggregation package.  
 Both take some work to code. This is probably the trickiest of the properties.

* particle\_density\_standard:  
 Density is a tricky definition. For the standard way of defining it I would just go with a simple definition: The the particle mass divided by the volume of a sphere defined by the standard maximum dimension (however we decide to calculate that), i.e. m/(𝜋/6 D^3).

* particle\_projected\_area\_standard:  
 This can be calculated by projecting the element coordinates on a plane, giving each element a "footprint" of approximately the element spacing, and neglecting overlap. After this the projected area can be calculated from the total covered area on the plane.  This is quite easy to implement for the x, y or z directions; for an arbitrary axis it takes a bit more work. I have a Python implementation of this. Does this need really to be one of the mandatory variables?

* particle\_projected\_axis\_ratio\_standard:  
 One way to calculate this is to take the maximum extents of the particle projection (calculated as in particle\_projected\_area\_standard) in each direction and take the ratio of these. As above: Is it necessary for this to be mandatory? Also, it’s not clear how the ratio should be defined (i.e. is it the extent in the x direction divided by that in the y direction, or the other way around?).

* particle\_principal\_axes:  
 This can be calculated like this: If X is a (n,3) matrix with the element coordinates, the 3x3 covariance matrix is C= (1/n) (X-µ)T(X-µ) where µ is the mean of X (these definitions need to be adjusted if the masses of the elements are variable). Then the eigenvectors of C give the directions of the principal axes and the eigenvalues give the variance of distance along the corresponding eigenvector. Thus a good definition for the ith principal axis is (√λi)vi where λi is the ith largest eigenvalue of C and vi is the corresponding unit eigenvector. This is quite easy to implement if you have a linear algebra package (even if you don’t, C is just 3x3 so the eigensystem is quite easy to calculate). The main caveat is that with some simplistic particle shapes C may be singular, so some consideration needs to be given to robustness.

* particle\_scattering\_element\_composition:  
 This seems fairly clear except I'm not sure if this is necessary if there is only one constituent (e.g. ice). Maybe this can be made optional, to be used only if there is more the dimension particle\_constituent\_id is larger than 1?

* particle\_constituent\_name:  
 Just a note: due to the way NetCDF4 handles strings this can be a bit complicated to implement as given in the spec. Might be easier to make it an attribute with constituent names separated by commas.

**General notes**

There are some inconsistencies in variable naming regarding the use of plural or singular. For example we may have multiple element coordinates and the variable name is particle\_scattering\_element\_coordinates (plural). On the other hand we may also have multiple constituents but there the variable name is particle\_constituent\_name (singular). I have no strong preference as to which way is correct but I recommend we adopt a consistent convention.

## Notes from the 1st Scattering Skype Meeting (2nd August 2017)

1. ) General scattering database file structure

    Store data into netCDF4 and XML files
    Distinction into multiple levels of data processing:
 
    **LEVEL0:**
      * 3D shape structure **(level0a)**
      * physical particle properties **(level0b)**
      *  ampl. Scattering matrices (per orientation) **(level0c)**

    **LEVEL1:**
      * can be generated by API if level0c is provided
      * phase matrices (per orientation); suited e.g. for polarimetric RT

    **LEVEL2:**
      * Essential scattering properties

     **LEVEL3**
      * Scattering properties integrated over PSDs

2. ) Naming conventions

    What is needed for individual users, what is relevant for automatic interfaces?

    **Filenames**
      * Hash filename with shape file to produce unique particle identifier
      * Dataset-id (identifying producer, time of generation,...)
      * Release version (?)

    **Folder structure, naming of folders**
      * Get inspired by satellite data product naming for naming of folders
      * Folders and sub-folders per database
      * Sub-folders according to levels (level0a, level0b,...)
      * E.g. OpenSSP could offer selection boxes for which databases to include for a query

    **All: Generate examples of (dummy) files and folder structure/naming as you think it would be best using the google-drive or github**

3. ) Meta Data
   * Need a meta-data record for search functionality
   * Store all meta information in netcdf attributes
   * Provide a function that takes netcdf file and writes an .xml or .json file containing all meta data of the dataset suited for any specific interface tool

