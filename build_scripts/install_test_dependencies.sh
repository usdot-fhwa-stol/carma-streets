#!/bin/sh

# exit on errors
set -e

# add the STOL APT repository

apt-get update

# NOTE: lxml python library dependes on libxml2 and libxslt1-dev. lxml is a dependency of gcovr

DEPENDENCIES="\
    python3-pip 
    python3-lxml 
"
    

# install all things needed for deployment, always done
apt-get install -y $DEPENDENCIES 
python3 -m pip install gcovr
