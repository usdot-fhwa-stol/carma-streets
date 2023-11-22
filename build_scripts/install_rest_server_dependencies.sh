#!/bin/bash

# exit on errors
set -e

# add the STOL APT repository
apt-get update

# NOTE: libwebsockets-dev from Ubuntu 20 on is sufficient
DEPENDENCIES=(
    libssl-dev
    qtbase5-dev 
    qtbase5-dev-tools
)
    
# install all things needed for deployment, always done
apt-get install -y "${DEPENDENCIES[@]}" 

cd /home/carma-streets/ext
git clone https://github.com/etherealjoy/qhttpengine.git
cd /home/carma-streets/ext/qhttpengine/
mkdir /home/carma-streets/ext/qhttpengine/build
cd /home/carma-streets/ext/qhttpengine/build
cmake .. -DMAKE_PREFIX_PATH=/usr/include/x86_64-linux-gnu/qt5
make -j
make install