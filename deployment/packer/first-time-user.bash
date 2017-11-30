#!/bin/bash
# These commands will clone the github repository. Before you run these,
# make sure that you set up your git ssh key.
#
# You can also manually download the package as a .zip file from the 
# GitHub website (https://github.com/rhoneyager/libicedb/archive/master.zip).
ssh-keyscan github.com >> ~/.ssh/known_hosts
git clone git@github.com:rhoneyager/libicedb.git

# After this, follow the instructions in the README.md file.

cd libicedb
mkdir build
cd build
#cmake ../
