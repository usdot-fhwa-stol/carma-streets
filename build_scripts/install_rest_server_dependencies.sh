#!/bin/sh

# exit on errors
set -e

# add the STOL APT repository
apt-get update

# NOTE: libwebsockets-dev from Ubuntu 20 on is sufficient
DEPENDENCIES="libssl-dev \
    qtbase5-dev \
    qtbase5-dev-tools"
    
# install all things needed for deployment, always done
apt-get install -y $DEPENDENCIES 

cd ../ext
git clone https://github.com/etherealjoy/qhttpengine.git
cd qhttpengine/
mkdir build
cd build
cmake .. 
make -j
make install