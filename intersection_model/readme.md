## intersection_model
# A Library to read lanelet2 osm map 
This library will read and lanelet2 osm map and take in a configurable bounding box to mark the intersection being managed. The library will store the appropriate lanelet2 objects inside this bound box including entry lanes into the intersection, linking lanes through the intersection and any regulatory elements (stop bars, stop signs, traffic lights, speed limits) inside the intersection. The intersection_model had methods to translate GPS location data into lane specific information like lanelet id and distance to the end of the lane for services like the message_service to provide lane. The intersection_model will also return the intersection lanelet2 objects to services that need them to make sense of the lane specific updates the message_service provides.


## Steps to Build intersection_model locally

```
apt-get update  && apt-get install -y cmake libboost1.65-all-dev git autotools-dev automake sqlite3 libsqlite3-dev curl libpugixml-dev libgeographic-dev qtbase5-dev qtbase5-dev-tools libqhttpengine-dev libssl-dev

# Clone and install googletest, librdkafka, spdlog and rapidjson in the ext/ directory
cd carma-streets root directory
mkdir ext/
cd ext/
git clone https://github.com/google/googletest/
cd googletest/
cmake .
make
sudo make install

cd /ext
git clone https://github.com/edenhill/librdkafka
cd  librdkafka/
./configure --prefix=/usr
make
sudo make install

cd ext/
git clone https://github.com/gabime/spdlog.git
cd spdlog/
mkdir build 
cd  build
cmake .. && make -j
sudo make install

cd ext
git clone  https://github.com/Tencent/rapidjson
cd  rapidjson/
mkdir build
cd build
cmake .. && make -j
sudo make install

# Install kafka-clients library
cd carma-streets/kafka_clients
mkdir build/
cd build
make ..
make
sudo make install

# Install PROJ, a package for coordinate transformations
sudo git clone https://github.com/OSGeo/PROJ.git   carma-streets/ext/PROJ --branch 6.2.1 && \
        cd /home/carma-streets/PROJ && \
        sudo ./autogen.sh && \
        sudo ./configure && \
        sudo make && \
        sudo make install
        
# Download a cmake module for PROJ
cd /usr/share/cmake-3.10/Modules && sudo curl -O https://raw.githubusercontent.com/mloskot/cmake-modules/master/modules/FindPROJ4.cmake

ENV DEBIAN_FRONTEND=noninteractive

sudo apt update && \
    sudo apt install -y lsb-release && \
    sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list' && \
    curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add -


# Install CARMA Ready Lanelet2
# Lanelet2 is written on the assumption ROS exists. Therefore some special steps are needed for compilation
# First catkin_make must be installed. The version needed is only available with the ros-melodic-catkin build
# This means the ros source list must be added to sudo to allow this installation
# Additionally, the python-rospkg package managment tool is required for catkin to work properly 
RUN sudo DEBIAN_FRONTEND=noninteractive apt update && \
    sudo DEBIAN_FRONTEND=noninteractive apt install -y ros-melodic-catkin && \
    echo "source /opt/ros/melodic/setup.bash" >> /root/.bashrc && \
    echo "source /home/carma-streets/install/setup.bash" >> /root/.bashrc

# Once catkin is installed only the required lanelet2 packages will be pulled in from carma
# NOTE: The lanelet2_python package requires additional dependencies that have not yet been installed so it is removed for now
cd carma-streets/
mkdir carma_lanelet2 && \
    cd carma_lanelet2 && \
    mkdir src && \
    cd src && \
    git init && \
    echo "temp" && \
    git remote add origin -f https://github.com/usdot-fhwa-stol/autoware.ai.git && \
    git config core.sparsecheckout true && \
    echo "common/hardcoded_params/*" >> .git/info/sparse-checkout && \
    echo "common/lanelet2_extension/*" >> .git/info/sparse-checkout && \
    echo "lanelet2/*" >> .git/info/sparse-checkout && \
    echo "mrt_cmake_modules/*" >> .git/info/sparse-checkout && \
    git pull --depth 1 origin refactor_lanelet2_extension && \
    git checkout refactor_lanelet2_extension && \
    rm -r lanelet2/lanelet2_python && \
    rm -r lanelet2/lanelet2_examples

cd carma_lanelet2/

# In order to trick lanelet2 into building the ROS_VERSION environment variable must be set
# In order to fully decouple lanelet2_extension from ros the LANELET2_EXTENSION_LOGGER_TYPE environment variable must be set
source /opt/ros/melodic/setup.bash && \
    sudo DEBIAN_FRONTEND=noninteractive apt-get install -y python-rospkg && \
    sudo DEBIAN_FRONTEND=noninteractive apt-get install -y libeigen3-dev && \
    ROS_VERSION=1 LANELET2_EXTENSION_LOGGER_TYPE=1 catkin_make install


# Install intersection_model
cd carma-streets/intersection_model
mkdir build
cd build
RUN cmake ..
RUN make


```
## Steps to Build intersection_model using Docker

```
Navigate to /carma-streets/ directory
docker build -t intersection_model -f intersection_model/Dockerfile .
```
## API definition
1. intersection model is a service used the expose intersection geometry information from MAP and lanelet2 osm map to carma-streets services that require this information via REST server. 
2. Intersection Geometry consists of list of entry, link, and departure lanelets which have the following fields:
```
intersection_info:
      type: object
      properties:
        id:
          type: integer
        name:
          type: string
        entry_lanelets:
          $ref: "#/components/schemas/lanelet_array"
        link_lanelets:
          $ref: "#/components/schemas/lanelet_array"
        departure_lanelets:
          $ref: "#/components/schemas/lanelet_array"
        
lanelet_array:
    description: The array of lanelets
    type: array
    items:
        $ref: "#/components/schemas/lanelet_info"

lanelet_info:
    description: The information of lanelet 
    type: object
    properties:
      id:
        type: integer
      speed_limit:
        type: number
        description: Unit of measure is m/s
      conflict_lanelet_ids:
        type: array
        items:
          type: integer
        description: List of unique identifers for lanelets that have conflicts
      length:
        type: number
        description: The length of lanelet. Unit of measure is meter
      turn_direction:
        type: string
        description: Turn direction of intersection lane
      signal_group_id: 
        type: integer
        description: The matching signal group send by the SPAT message for this lanelet
      connecting_lanelet_ids:
        description: List of unique identifers for following lanelets that are connecting the depature and entry lanelets
        type: array
        items:
          type: integer
```
3. Intesection model handler and router classes inherit from the default handler and router classes from the generated library in streets_utils/streets_api folder. 
## Notes
When running unit test for the intersection model, making sure the correct osm file is upload under the intersection_model directory.