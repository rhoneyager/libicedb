---
title: "Discussion Notes"
keywords: icedb libicedb
tags: [collaboration]
sidebar: mydoc_sidebar
permalink: notes.html
summary: Short summaries of the web meeting among the development team
---

## 17 April 2018
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
