---
title: "Getting started with icedb"
keywords: homepage icedb libicedb
tags: [getting_started]
sidebar: mydoc_sidebar
permalink: index.html
summary: icedb is a toolkit for manipulating particle scattering databases. It is under heavy development, so this site doesn't have much content yet. The instructions here will help you quickly get started. The other topics listed in the sidebar will provide additional information and detail.
---

There are now many published datasets on the scattering properties of realistically shaped snow
and aerosol particles. These datasets represent increasingly sophisticated attempts to match the
variability and detail of particles found in nature. Applications range from ground-based, 
airborne, and space-based retrievals to active and passive forward simulators and the assimilation 
of all-sky microwave observations into numerical weather prediction models.

The icedb toolkit provides the ability to manipulate both particle structural information 
and scattering information. It can be used to convert diverse sets of scattering databases into
a common HDF5 / NetCDF file format. This format is an outgrowth of discussions at the [2017 
International Summer Snowfall Workshop](https://journals.ametsoc.org/doi/pdf/10.1175/BAMS-D-17-0208.1).
Having a common format for storing scattering data is quite desirable, as it allows researchers to
rapidly assess features of different particle models and use existing results in their own research.
Different papers describe their particle models using different metrics, so icedb also provides 
functions to establish consistent definitions of quantities like particle size, aspect ratio, 
fractal dimension and projected area. 

The library and the associated applications will build on Windows, OS X, many Linuxes and BSDs. It
is written in C++, and will also eventually provide a C-style interface. This will allow end users to
use the library in diverse languages, such as Fortran, IDL, Julia, Matlab and Python. It has a natural 
interface that conceals file I/O details from end users, allowing them to focus on science and 
not programming arcana. 

{% include links.html %}
