# icedb packer instructions

The files in this directory are used to produce pre-provisioned
virtual machines for Linux development with icedb!

This deployment depends on the [Packer](https://www.packer.io/)
package. Download it directly from this website. It is under heavy
development and the OS-provided packages are typically outdated.

Currently, we only will generate [LXD](https://linuxcontainers.org/lxd/)
images, targeting an Ubuntu 17.10 guest. Packer can handle many
more virtualization targets (e.g. Docker, VirtualBox, Vagrant, Hyper-V)
and guest operating systems (e.g. Windows, FreeBSD, various Linuxes), but
this is sufficient for now.

## Build instructions for Ubuntu

1. Download / install Packer.

2. Install and configure lxd (see [here](https://help.ubuntu.com/lts/serverguide/lxd.html))
```
sudo apt update
sudo apt install lxd
sudo lxd init
```

3. Build the LXD image
```
packer build packer-icedb.json
```

The image is created and inserted into the local LXD server's image cache. You can view this using:
```
lxc image list
```

4. (Optional) Launch a new instance of this image, and follow the instructions in /root/README.
```
lxc launch icedb-ubuntu-artful test-image
lxc exec test-image -- /bin/bash
cat README
```


