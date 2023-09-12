#!/bin/sh

# exit on errors
set -e

# add the STOL APT repository
. /etc/lsb-release
echo "deb [trusted=yes] http://s3.amazonaws.com/stol-apt-repository ${DISTRIB_CODENAME} main" > /etc/apt/sources.list.d/stol-apt-repository.list

apt-get update

# NOTE: libwebsockets-dev from Ubuntu 20 on is sufficient
DEPENDENCIES="build-essential \
    cmake \
    git \
    libboost-all-dev \
    libgtest-dev \
    libssl-dev \
    qtbase5-dev \
    wget \
    libperl-dev \
    wget \
    libspdlog-dev"
    
# STOL library dependencies

LIBRARY_DEPENDENCIES=" \
    carma-clock-1"

# install all things needed for deployment, always done
apt-get install -y $DEPENDENCIES ${LIBRARY_DEPENDENCIES}

# install gtest
cd /usr/src/googletest/
mkdir -p build/
cd build
cmake ..
make install
