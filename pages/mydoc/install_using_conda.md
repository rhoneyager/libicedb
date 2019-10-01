---
title: Configure, Build and Install using Conda
tags: [getting_started, troubleshooting, installation, Conda]
keywords: dependencies Linux Conda install
last_updated: September 26, 2018
summary: "Install guide using Conda"
sidebar: mydoc_sidebar
permalink: install_using_conda.html
folder: mydoc
---

The [Conda package management system](https://conda.io) allows you to quickly get 
up-to-date libraries and compilers for your system. This is particularly useful for 
conservative distributions, like CentOS and Red Hat Enterprise Linux. Older versions 
of other distributions, like Ubuntu 14.04, may also benefit.

From the Conda Project's website:
```
Conda is an open source package management system and environment management system 
that runs on Windows, macOS and Linux. Conda quickly installs, runs and updates packages 
and their dependencies. Conda easily creates, saves, loads and switches between 
environments on your local computer. It was created for Python programs, but it can 
package and distribute software for any language.

Conda as a package manager helps you find and install packages. If you need a package 
that requires a different version of Python, you do not need to switch to a different 
environment manager, because conda is also an environment manager. With just a few 
commands, you can set up a totally separate environment to run that different version 
of Python, while continuing to run your usual version of Python in your normal environment.
```

Conda can install more than just Python packages. It has support for installing the 
GNU compilers, and icedb's backend libraries, too!

First-time installation of Conda:
---------------------------------

1. Download the version of [Anaconda](https://www.anaconda.com/download/) or [Miniconda](https://conda.io/miniconda.html) that is appropriate for your operating system and processor family.
2. Follow the installation instructions. This usually involves using bash to execute the downloaded package and adding Conda to your path.

First-time creation of the "icedb" environment:
-----------------------------------------------

After installing Conda, you will want to create a special "environment" for icedb. An environment is an isolated installation of packages. This command creates the "icedb" environment and gathers all of the packages that you need to build icedb:
```
conda create --name icedb gcc_linux-64 gxx_linux-64 boost hdf5 cmake make git
```

Using the "icedb" environment, and building the code:
-----------------------------------------------------

1. Activate the environment. This puts the package directories into your PATH. You should do this every time you spawn a new shell and want to work with icedb.
```
conda activate icedb
```
2. Download (and perhaps extract) the source code package. 
3. Create a new build directory. It can be anywhere. Switch into this directory.
4. Run cmake to generate the build scripts. e.g.:
```
cmake {Path to source directory}
```
This command defaults to using the Conda-installed packages for the build.

If you eventually want to install icedb somewhere, then call cmake like this:
```
cmake -DCMAKE_INSTALL_PREFIX={install path} {Path to source directory}
```

5. Build the code, using:
```
make
```
6. If the build is successful, binaries and libraries should be in the ./Release directory.
If you changed the CMAKE\_BUILD\_TYPE option to another setting, like "Debug" or 
"RelWithDebInfo", the binaries and libraries will be in the ./Debug or ./RelWithDebInfo 
directory, respectively. These options trigger different compiler switches, such as turning 
on/off debug symbols and different levels of compiler optimizations. There is a marked 
difference in the size and speed of the compiled programs.

7. To copy the binaries to the install directory:
```
sudo make install
```
{% include note.html content="If you already have write privileges on your install directory, such as when it is in your home directory, then you can just run make install without sudo." %}

8. To create archives of the source code and compiled binaries, try
```
make package
```
CMake can make many different types of packages. Consult the CMake / CPack documentation for details.

*Note:* We do not yet distribute Conda-specific packages of icedb.

