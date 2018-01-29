---
title: "Getting started with icedb"
keywords: homepage icedb libicedb
tags: [getting_started]
sidebar: mydoc_sidebar
permalink: index.html
summary: icedb is a toolkit for manipulating particle scattering databases. It is under heavy development, so this site doesn't have much content yet. The instructions here will help you quickly get started. The other topics listed in the sidebar will provide additional information and detail.
---

{% include note.html content="If you're using this software, then you are probably interested in storing and manipulating snow and aerosol particles. This toolkit provides the ability to manipulate both particle structural information and scattering information." %}

## Get icedb pre-packaged 

On certain operating systems, pre-packaged versions of icedb are available. This is the easiest way to get started, but it is also the least customizable. If you only want to run the example binaries, then this option is best. If you want to link your own code against icedb, then this option is okay. If you want to develop icedb or greatly extend its functionality, then skip to the next section.

{% include note.html content="Packages are on the TO-DO list for now." %}

## Build icedb


Follow these instructions to build icedb.

### 1. Download icedb

First download or clone icedb from the [Github repo](https://github.com/rhoneyager/libicedb). Development is ongoing, so cloning the repository probably makes the most sense. However, for anyone unexperienced with git, GitHub or version control, you might just want to download a ZIP file containing the latest copy of the source code. In this case, click the **Clone or download** button, and then click **Download ZIP**.

{% include note.html content="Currently, this repository is **PRIVATE**, so only members of the ibedb development team can access it. If you want access, [send me an email](mailto:ryan@honeyager.info)." %}

### 2. Install the prerequisites

If you've never installed or run icedb locally on your computer, follow these instructions to install the backend libraries that icedb needs to function:

[Sidebar navigation][mydoc_sidebar_navigation].

* [Install dependencies on CentOS / RHEL / Fedora][install_deps_on_fed]
* [Install dependencies on Debian / Ubuntu][install_deps_on_deb]
* [Install dependencies on FreeBSD][install_deps_on_bsd]
* [Install dependencies on Mac][install_deps_on_mac]
* [Install dependencies on Windows][install_deps_on_windows]

### 3. Configure, build and install icedb using CMake

CMake is a tool for generating build scripts. Every development environment is set up in a different way. Files may be located in different places. Packages may have different versions. Different compilers may be used. Different operating systems provide different features to end-user programs. CMake accounts for all of these differences, and produces sets of building instructions that will work for your particular platform.

In case you have never used CMake before, you might want to take a look at [it's website](http://cmake.org) and these tutorials for end-users using the [console](https://www.youtube.com/watch?v=CLvZTyji_Uw) and a [GUI](https://www.youtube.com/watch?v=lAiuLHy4dCk).

If you've never installed or run icedb locally on your computer, follow these instructions to install icedb:


* [Build icedb using Makefiles (most of you should go here)][mydoc_build_icedb_on_makefiles]
* [Build icedb using Visual Studio 2017][mydoc_build_icedb_on_vs17]
* [Build icedb using XCode][mydoc_build_icedb_on_xcode]

### 4. Learn where the installation has put the different components of the icedb installation

### 5. Try out the example applications

### 6. Try linking a small, sample application to the icedb libraries


{% include links.html %}
