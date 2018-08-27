#!/bin/bash
source activate icedb
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/icedb -DBUILD_PLUGIN_DDSCAT=off -DBUILD_PLUGIN_GENERAL_TEXT_IO=off -DBUILD_PLUGIN_PSU=on -DBUILD_PLUGIN_SILO=off -DBUILD_TESTING=off ../

