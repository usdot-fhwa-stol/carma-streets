#!/bin/bash
# Script assumes base image of carma-builds-64x
set -e
# Get ubuntu distribution code name. All STOL APT debian packages are pushed to S3 bucket based on distribution codename.
# shellcheck source=/dev/null
source /etc/lsb-release
# add the STOL APT repository
echo "deb [trusted=yes] http://s3.amazonaws.com/stol-apt-repository ${DISTRIB_CODENAME} main" > /etc/apt/sources.list.d/stol-apt-repository.list
echo "deb http://packages.ros.org/ros/ubuntu ${DISTRIB_CODENAME} main" > /etc/apt/sources.list.d/ros-latest.list
DEBIAN_FRONTEND=noninteractive apt install --no-install-recommends --yes --quiet gnupg
wget -qO- https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | apt-key add -
apt-get update
DEPENDENCIES=(
    autotools-dev 
    automake 
    sqlite3 
    libsqlite3-dev 
    libpugixml-dev 
    libgeographic-dev
    ros-melodic-catkin
    python-rospkg
    libeigen3-dev
    libtool
    libboost-all-dev
)
DEBIAN_FRONTEND=noninteractive apt install --no-install-recommends --yes --quiet "${DEPENDENCIES[@]}"
# Install PROJ, a package for coordinate transformations
git clone https://github.com/OSGeo/PROJ.git /tmp/PROJ --branch 6.2.1 
cd /tmp/PROJ  
./autogen.sh 
./configure 
make 
make install
cd ..
rm -r PROJ
        
# Download a cmake module for PROJ
wget -P /usr/local/share/cmake-3.27/Modules https://raw.githubusercontent.com/mloskot/cmake-modules/master/modules/FindPROJ4.cmake

cd /tmp
# Once catkin is installed only the required lanelet2 packages will be pulled in from carma
# NOTE: The lanelet2_python package requires additional dependencies that have not yet been installed so it is removed for now
mkdir carma_lanelet2
cd carma_lanelet2 
mkdir src 
cd src 
git init 
echo "temp" 
git remote add origin -f https://github.com/usdot-fhwa-stol/autoware.ai.git 
git config core.sparsecheckout true
# See https://www.shellcheck.net/wiki/SC2129 for reference on syntax
{
echo "common/hardcoded_params/*" 
echo "common/lanelet2_extension/*"  
echo "lanelet2/*"  
echo "mrt_cmake_modules/*"
 } >> .git/info/sparse-checkout 
git pull --depth 1 origin refactor_lanelet2_extension 
git checkout refactor_lanelet2_extension 
rm -r lanelet2/lanelet2_python
rm -r lanelet2/lanelet2_examples
cd /tmp/carma_lanelet2/
source /opt/ros/melodic/setup.bash
ROS_VERSION=1 LANELET2_EXTENSION_LOGGER_TYPE=1 catkin_make install -DCMAKE_INSTALL_PREFIX=/opt/carma_lanelet2 -DCATKIN_DEVEL_PREFIX=/tmp/carma_lanelet2/src
ls
cd install
cat setup.bash
cd tmp/
rm -r carma_lanelet2
echo 'source /opt/ros/melodic/setup.bash' >> ~/.bashrc 
echo 'source /opt/carma_lanelet2/setup.bash' >> ~/.bashrc 

