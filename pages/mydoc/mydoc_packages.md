---
title: Packages
tags: [getting_started, troubleshooting, installation, packages]
last_updated: January 27, 2018
keywords: dependencies, packages
summary: "Pre-compiled packages are available for certain operating systems"
sidebar: mydoc_sidebar
permalink: install_packages.html
folder: mydoc
---

## Get icedb pre-packaged 

On certain operating systems, pre-packaged versions of icedb are available. 
This is the easiest way to get started, but it is also the least customizable. 
If you only want to run the example binaries, then this option is best. 
If you want to link your own code against icedb, then this option is okay. 
If you want to develop icedb or greatly extend its functionality, then skip 
to the next section.

icedb packages are released monthly on 
[the GitHub repository](https://github.com/rhoneyager/libicedb/releases).

For rolling package updates, you can find packages for your platform using the links below.
The "master" branch is the stable branch, and the "development" branch is the branch that
will eventually become the next master.

## Build status


| Platform                   | Master Branch                                                                                                                                                                        | Development Branch                                                                                                                                                                  |
|----------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| CentOS 7 / Conda g++ 7     | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/CentOS7-conda?branchName=master)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=9)        | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/CentOS7-conda?branchName=devel)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=9)        |
| CentOS 7 / SCL g++ 7.3     | Work in progress                                                                                                                                                                     | Work in progress                                                                                                                                                                    |
| Debian Stretch             | Work in progress                                                                                                                                                                     | Work in progress                                                                                                                                                                    |
| Fedora 28 / g++ 8.1        | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Fedora-28?branchName=master)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=4)            | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Fedora-28?branchName=devel)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=4)            |
| macOS                      | Work in progress                                                                                                                                                                     | Work in progress                                                                                                                                                                    |
| Ubuntu 16.04 / g++ 5.4     | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-16.04?branchName=master)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=2)         | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-16.04?branchName=devel)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=2)         |
| Ubuntu 16.04 / Conda g++ 7 | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-16.04-conda?branchName=master)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=8)   | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-16.04-conda?branchName=devel)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=8)   |
| Ubuntu 18.04 / clang 6     | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-18.04-clang-6?branchName=master)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=7) | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-18.04-clang-6?branchName=devel)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=7) |
| Ubuntu 18.04 / g++ 6       | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-18.04-gcc-6?branchName=master)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=6)   | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-18.04-gcc-6?branchName=devel)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=6)   |
| Ubuntu 18.04 / g++ 7       | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-18.04?branchName=master)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=1)         | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-18.04?branchName=devel)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=1)         |
| Ubuntu 18.04 / g++ 8       | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-18.04-gcc-8?branchName=master)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=5)   | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Ubuntu-18.04-gcc-8?branchName=devel)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=5)   |
| Windows 10 / VS 2017       | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Windows?branchName=master)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=3)              | [![Build status](https://icedb.visualstudio.com/icedb/_apis/build/status/Windows?branchName=devel)](https://icedb.visualstudio.com/icedb/_build/latest?definitionId=3)              |

### Notes
- "Partially succeeded" indicates that an error occurred during the "make test" step of the build. You should not use packages produced by such a build in production.
- The SCL toolset conflicts with the build testing system. We need to redo the build steps.
- macOS needs a testing system.
- Azure Pipelines does not provide a build agent for most BSDs. A read-only instance of CDash might be provided for those platforms.

