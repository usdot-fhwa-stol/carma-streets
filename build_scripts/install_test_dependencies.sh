#!/bin/sh

# exit on errors
set -e

# add the STOL APT repository

apt-get update

# NOTE: libxml2 and libxslt1-dev are requirements for pip

DEPENDENCIES="\
    libxml2
    libxslt1-dev
    python3-pip
    
"
    

# install all things needed for deployment, always done
apt-get install -y $DEPENDENCIES 
python3 -m pip install gcovr
