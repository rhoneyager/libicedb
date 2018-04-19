---
title: "Discussion Notes"
keywords: icedb libicedb discussion notes
tags: [collaboration]
sidebar: mydoc_sidebar
permalink: notes.html
summary: Short summaries of the web meeting among the development team
---

Since the last IPWG/IWSSM (Bologna 2016) meeting a couple of people (Stefan, Jussi, Davide, Eugene, Ben) worked on an excel-sheet with an initial file structure and variable names (following as closely as possible the CF convention) for storing scattering datasets. The Excel-file can be downloaded [here]

At the recent snowfall workshop in Cologne, we discussed to better store the data in different level files:

Level0: Lowest level of processing

    Level0a: Contains the 3D structure
    Level0b: Contains amplitude scattering matrix (following Mischenko convention)

Level 1: Phase Matrix (to be discussed)

Level 2: Essential scattering properties (to be discussed)

    Details need to be further discussed
    General idea: provide similar content as Liu database (cross sections, phase function, angle averaged quantities)

Level 3: Scattering properties integrated over PSD (to be discussed)

It might not be possible to generate all data levels from existing databases (some information might be missing) but it should be mandatory for all dataset developers to provide at least level0 and level2 files. For leve0a, an agreement was already found on how to name and define the 3D structure variables (all information can be found in the Excel file). Ideally, the level0 files should be enough to provide it to an API which generates the higher level files from it. Also interpolations of variables in the database requested by several users (included e.g. by Liu-DB) should be provided by an API which is separated from any specific database but can be used with any dataset.

We will keep the discussions ongoing via web-meetings to work out the details of the different file types.

**ENTRIES ARE IN REVERSED CRONOLOGICAL ORDER**


## Notes from the 4th Scattering WebEx Meeting (17 April 2018)
Webex seem to not work with pc audio but just with phones. Screen sharing is working. Webex is notoriously bad on linux platforms, it generally requires 32 bit implementations of java packages which are critically badly mantained on most distributions. It could work easier by installing a 32 bit linux distribution on a virtual machine, but still quite time consuming. 

PARTICIPANTS: Eugene - Stefan - Ian - Patrick - Ryan - Annakaisa - Davide

Ryan shows the new website created specifically for the libicedb code description and documentation

First point of the discussion is to have people involved and report things to be discussed/solved in the next weeks
We agreed that even if github does not provide a proper discussion tool we should try to use the buil-in issue tracker and project manager in order to establish a constructive feedback loop between core developers and test users. If those instruments become insufficient then we will thinck about something different

Some techinical issues with the implementation:
We use hdf5 instead of netcdf because the netcdf library is not as much cross platform as the hdf5. However libicedb is currently producing hdf5 files that are fully compatible with netcdf specifications and thus it should be possible to access them using any netcdf tool/library/language people is currently using
HDF5 c++ interface is not stable, but rather continuously changing and the available documentation does not reflect immediatly the changes the hdf5-c++ developer team is making. As a workaround Ryan will use standard C interface and build a c++ wrapper around it. However there is also a parallel project he is developing named hdf5forhumans which would make it easier for everybody to interact with hdf5 files using c++

The current implementation allows for great flexibility in how to structure the database (one file per shape or multiple shapes in one file and so on). It will be up to the user how to structure the data

Looking forward to how to share the databases we should take a look into git-annex functionality, then we can think about how to implement it. It would allow people to store their databases on different storage services and be linked to the library automatically.
However the priority is to have people using the library and report

Ryan coded additional program to draw a line of research and some possible application of the library. People can contribute sharing codes in other languages and more experienced people can contribute to c++. Eugene, Davide and Patrick are gonna try to help Ryan's work. The most important message should be that everybody is invited to contribute to the code with what he/she has. Not be frightned by the complexity of the coding part.

There is also room to contribute with non-coding stuff. Looking at the structure of the data highligting improvements possible and bugs, contributing to documentation and tutorial stuff. Ryan on December shared an working package which people can use to have a feeling of the library functionality without troubles arising from lack of specific format support and details on the implementation.

Ian will talk to Matt Lammers to have him involved in order to be able to use Kwo's group webpage for database aceess

From now on we shall try to put the notes on the code-webpage for a easier navigation and link to the development.

Everybody works with it and share thoughts about what they can contribute on so than we can split up the effort:
Eugene uses other shapefile and keep in touch for problems
Stefan try Jussi's shapefiles as well. For the storage we can use some ftp still. Also melted particles would be an interesting test

If anybody wants to try something, just write Ryan and he will share instruction on how to use the library to handle special properties of the particles

Next meeting? 17 May works best for everybody 

## Notes from the 3rd Scattering Skype Meeting (15th November 2017)

The BAMS report article on the first International Summer Scattering Workshop has been accepted and it is waiting for editing.

The scheduling of the meeting originally kept the UTC reference for a universal coordination, but since our last daylight saving time change the 15:00 UTC can be a problem in certain parts of the world. After KSK suggestion we decided to keep the schedule at 17:00 German time and share the meeting updates hoping for our calendar application to be smart enough to adjust the correct timezone

Kwo and Ian will explore the possibility to use their institutional webex account to organize the meeting since skype is revealing to not be the best solution

Kwo also annouce a new iteration of the openSSP database that will aloow the user to specify PSD and m-D relation. Also plans to include polarimetry.

1. Ryan send to everybody an example database encoded with the developing icedb library.
  * It is hdf5, but still fully netcdf compatible so everybody should be able to access it using their favourite standard netcdf tool
  * The program automatically index every file in a specific folder (it expects to find shapefiles)
  * It is still missing the unique identifier functionality, but any additional property can be rather easily integrated thanks to the set of tool Ryan implemented in the library
  * The software gives function to create tables and attributes and can write tables with any dimension
  * **task for everybody**: try to use the example program and report for missing functionalities and bugs

## Notes from the 2nd Scattering Skype Meeting (13th September 2017)

1. Level0 files and routines
  * Need to establish a separate working group dedicated to the implementation of level 0 routines
  * In particular Ryan is asking for some help in gathering adda and gmm sample geometry files 
  * Eugene offered to get in touch with Ryan and to provide access to GMM level0 sample files
  * Kuo and Ian suggest a general geometry reference (common frame of reference) and will prepare some examples
  * There was a general agreement that more detailed discussions will be most effective once we have an example level0 file

2. Discussion on physical particle properties
  * Jussi compiled several suggestions and questions related to the physical particle properties (see earlier entry in this forum!). He provided a short summary of his main points which was discussed afterwards:
  * Projected area along each incidence direction should be provided
  * We should decide for **a** standard definition of dmax (multiple alternative definitions will be allowed to be included in the file but as optional variables)
  * There was some agreement that dmax as maximum 3D distance between two dipoles inside the particle would be a better definition than the diameter of the enclosing sphere
  * Closely related to dmax is the definitions of density; most participants agreed with Jussi's suggestion hwo to define density
  * Code some API that compute physical properties in a standardized way based on the shape-files provided 

3. Discussion on variable naming and structuring
  * In the current variable structure we suggest to group them by **essential** and **optional** variables for each data file type
  * Jussi suggested to group them into
    + **mandatory**: everybody has to provide them following our definitions
    + **standardized**: if somebody wants to provide them, he/she has to use our standard/definitions
    + **optional**: variables that can be added by every database producer according to their definitions; these definitions **have to** be provided in the meta-data.
  * Jussi also suggested to not use the suffix _standard for variables but rather include the method or paper in the suffix, e.g. particle_max_dimension_3Ddistance
  * The variable particle_scattering_element_composition should be optional if the particle is composed by ice only in order to reduce file size
  * Make naming self explainatory. Ryan will provide suggestions

4. Data Exchange, FTP server, Mailing List
  * In order to share/exchange data and code, an open ftp server (current space limit 100 GB) is provided by Uni. Cologne: 
     + ftp **gop.meteo.uni-koeln.de**
     + user: libscat
     + pwd: ask Stefan or Davide

  * In order to simplify email communication, we also set up a mailing list for all people involved in the regular skype meetings:
    + **meteo-scattdiscuss@uni-koeln.de**
    + You can easily subscribe/unsubscribe and change other setting here: https://lists.uni-koeln.de/mailman/listinfo/meteo-scattdiscuss
    + For more specific code discussions we set up a second mailing list called meteo-scattcode@uni-koeln.de (change your settings for this list by following this link:  https://lists.uni-koeln.de/mailman/listinfo/meteo-scattcode )

  * Users also requested an SSL encoding for snowport; this is currently work in progress


## Notes and thoughts on particle physical properties

Jussi Leinonen, August 2017

* Background

I am currently about to create a large set of snowflake models for scattering simulations. I thought it would be good to make this dataset compatible with our ideas for the database, but I noticed that many of the particle physical properties in the spreadsheet so far lack exact definitions. I am writing this to document to express my suggestions for how those properties should be defined. Additions and comments are welcome.

* Particle physical properties

Below, I’ll visit each particle physical property listed as so-far undefined in the spreadsheet tabs “particle_physical_properties” and “3d_structure”. I give suggestions as to how they should be implemented.

  1. particle_maximum_dimension_standard:

  I believe this should be independent of particle orientation, so we’re left with two reasonable choices: (1) The maximum distance between any two elements in the particle or (2) The diameter of the smallest enclosing sphere of the set of elements. These are not equal in general.

    * Can be computed by first getting the convex hull of points and then finding the maximum distance between the vertices. This can be done with this algorithm in O(n log h): [link](https://en.wikipedia.org/wiki/Chan%27s_algorithm) but I haven’t implemented it yet.
    * Can be implemented in O(n) time using the algorithm of [Hopp and Reeve, 1996:](http://www.mel.nist.gov/msidlibrary/doc/hopp95.pdf). I have an implementation of this in my aggregation package.

  Both take some work to code. This is probably the trickiest of the properties.

  2. particle_density_standard:

  Density is a tricky definition. For the standard way of defining it I would just go with a simple definition: The the particle mass divided by the volume of a sphere defined by the standard maximum dimension (however we decide to calculate that), i.e. m/(𝜋/6 D^3).

  3. particle_projected_area_standard:

  This can be calculated by projecting the element coordinates on a plane, giving each element a “footprint” of approximately the element spacing, and neglecting overlap. After this the projected area can be calculated from the total covered area on the plane.  This is quite easy to implement for the x, y or z directions; for an arbitrary axis it takes a bit more work. I have a Python implementation of this. Does this need really to be one of the mandatory variables?

  4. particle_projected_axis_ratio_standard:

  One way to calculate this is to take the maximum extents of the particle projection (calculated as in particle_projected_area_standard) in each direction and take the ratio of these. As above: Is it necessary for this to be mandatory? Also, it’s not clear how the ratio should be defined (i.e. is it the extent in the x direction divided by that in the y direction, or the other way around?).

  5. particle_principal_axes:

  This can be calculated like this: If X is a (n,3) matrix with the element coordinates, the 3x3 covariance matrix is C= (1/n) (X-µ)T(X-µ) where µ is the mean of X (these definitions need to be adjusted if the masses of the elements are variable). Then the eigenvectors of C give the directions of the principal axes and the eigenvalues give the variance of distance along the corresponding eigenvector. Thus a good definition for the ith principal axis is (√λi)vi where λi is the ith largest eigenvalue of C and vi is the corresponding unit eigenvector. This is quite easy to implement if you have a linear algebra package (even if you don’t, C is just 3x3 so the eigensystem is quite easy to calculate). The main caveat is that with some simplistic particle shapes C may be singular, so some consideration needs to be given to robustness.

  6. particle_scattering_element_composition:

  This seems fairly clear except I’m not sure if this is necessary if there is only one constituent (e.g. ice). Maybe this can be made optional, to be used only if there is more the dimension particle_constituent_id is larger than 1?

  7. particle_constituent_name:

  Just a note: due to the way NetCDF4 handles strings this can be a bit complicated to implement as given in the spec. Might be easier to make it an attribute with constituent names separated by commas.

* General notes

There are some inconsistencies in variable naming regarding the use of plural or singular. For example we may have multiple element coordinates and the variable name is particle_scattering_element_coordinates (plural). On the other hand we may also have multiple constituents but there the variable name is particle_constituent_name (singular). I have no strong preference as to which way is correct but I recommend we adopt a consistent convention.

## NOTES from the 1st Scattering Skype Meeting (2nd August 2017)

1. ) General scattering database file structure

  Store data into netCDF4 and xml files
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

