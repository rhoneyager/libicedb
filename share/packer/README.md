Automated build testing scripts
--------------------------------

This folder contains scripts that use [Packer](https://www.packer.io/)
to generate virtual machine images that can build icedb.

The images contain both icedb dependencies and Microsoft Azure
DevOps build agents. Images are set to auto-update.

To activate and register the build agents, see the README files
in /root on each image.

Supported operating systems:
- CentOS 7 with conda-provided g++ and libraries
- Debian Stretch
- Fedora 30
- Ubuntu 16.04
- Ubuntu 18.04
- Ubuntu 19.04

Supported image types:
- CharlieCloud *
- Docker *
- LXD
- Singularity *
- Vagrant/HyperV *
- Vagrant/VirtualBox *
- Vagrant/VMware *

