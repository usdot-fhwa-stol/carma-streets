#!/bin/sh

# exit on errors
set -e

# add the STOL APT repository

apt-get update

# NOTE: libwebsockets-dev from Ubuntu 20 on is sufficient
DEPENDENCIES="python3-pip "
    

# install all things needed for deployment, always done
apt-get install -y $DEPENDENCIES 
pip3 install gcovr
