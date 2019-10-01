---
title: Download icedb sources
tags: [getting_started, troubleshooting, installation]
last_updated: September 26, 2018
keywords: dependencies, install
summary: "Instructions on how to download the icedb sources"
sidebar: mydoc_sidebar
permalink: download.html
folder: mydoc
---

## Download icedb

First download or clone icedb from the [Github repo](https://github.com/rhoneyager/libicedb). 
Development is ongoing, so cloning the repository probably makes the most sense. However, 
for anyone unexperienced with git, GitHub or version control, you might just want to 
download a ZIP file containing the latest copy of the source code. In this case, click the 
**Clone or download** button, and then click **Download ZIP**.

When cloning icedb, you need to get both the icedb source and it's git submodules!
If you do not get the submodules, then the CMake configuration step will fail!
To both clone icedb and get submodules, run:
```
git clone --recurse-submodules https://github.com/rhoneyager/libicedb.git
```

Once downloaded, go the the appropriate section of the "Specific build instructions" area
(on the sidebar) that matches your operating system.

NOTE: If you have a very old version of git, then the above command might not work. Upgrade
your version of git. If you can use Conda for providing package dependencies, then follow its
installation instructions [here](https://rhoneyager.github.io/libicedb/install_using_conda.html).
