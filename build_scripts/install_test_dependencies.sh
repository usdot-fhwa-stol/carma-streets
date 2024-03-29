#!/bin/bash

# exit on errors
set -e

# add the STOL APT repository

apt-get update

# NOTE: lxml python library dependes on libxml2 and libxslt1-dev. lxml is a dependency of gcovr

DEPENDENCIES=(
    python3-pip 
    python3-lxml 
)
    

# install all things needed for deployment, always done
DEBIAN_FRONTEND=noninteractive apt-get install --yes --quiet --no-install-recommends "${DEPENDENCIES[@]}"
python3 -m pip install gcovr
