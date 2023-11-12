#!/bin/bash
set -e
# Get ubuntu distribution code name. All STOL APT debian packages are pushed to S3 bucket based on distribution codename.
# shellcheck source=/dev/null
source /etc/lsb-release
# add the STOL APT repository
echo "deb [trusted=yes] http://s3.amazonaws.com/stol-apt-repository ${DISTRIB_CODENAME} main" > /etc/apt/sources.list.d/stol-apt-repository.list
apt-get update
DEPENDENCIES=(
    libboost-all-dev 
    carma-clock-1
)

DEBIAN_FRONTEND=noninteractive apt install --no-install-recommends --yes --quiet "${DEPENDENCIES[@]}"

# Install librdkafka
echo " ------> Install librdkafka..."
cd /tmp
git clone https://github.com/confluentinc/librdkafka.git -b v2.2.0
cd librdkafka/
cmake -H. -B_cmake_build
cmake --build _cmake_build
cmake --build _cmake_build --target install
cd ../
rm -r librdkafka

# Install rapidjson
echo " ------> Install rapidjson..."
cd /tmp
git clone https://github.com/Tencent/rapidjson
cd rapidjson/
git checkout a95e013b97ca6523f32da23f5095fcc9dd6067e5
cmake -Bbuild 
cmake --build build
cmake --install build
cd .. 
rm -r rapidjson

# Install spdlog
echo " ------> Install spdlog... "
cd /tmp
git clone https://github.com/gabime/spdlog.git -b v1.12.0
cd spdlog 
cmake -Bbuild 
cmake --build build
cmake --install build
cd .. 
rm -r spdlog
