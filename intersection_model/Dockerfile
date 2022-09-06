FROM ubuntu:bionic-20210702

# Avoid interactive prompts during the building of this docker image
ARG DEBIAN_FRONTEND="noninteractive"

ARG DEPS="automake \
    autotools-dev \
    cmake \
    curl \
    git \
    libboost1.65-all-dev \
    libgeographic-dev \
    libpugixml-dev \
    libqhttpengine-dev \
    libsqlite3-dev \
    libssl-dev \
    lsb-release \
    qtbase5-dev \
    qtbase5-dev-tools \
    sqlite3"

SHELL ["/bin/bash", "-o", "pipefail", "-c"]

COPY ./intersection_model /home/carma-streets/intersection_model
COPY ./kafka_clients /home/carma-streets/kafka_clients
COPY ./streets_utils /home/carma-streets/streets_utils

RUN apt-get update && \
    apt-get install -y ${DEPS} && \
    # Install google test
    echo " ------> Install googletest..." && \
    mkdir -p /home/carma-streets/ext && \
    git clone --depth 1 https://github.com/google/googletest/ /home/carma-streets/ext/googletest && \
    cd /home/carma-streets/ext/googletest && \
    cmake . && \
    make -j install && \
    # Install spdlog
    echo " ------> Install spdlog... " && \
    git clone --depth 1 https://github.com/gabime/spdlog /home/carma-streets/ext/spdlog && \
    mkdir -p /home/carma-streets/ext/spdlog/build && \
    cd /home/carma-streets/ext/spdlog/build && \
    cmake .. && \
    make -j install && \
    # Install librdkafka
    echo " ------> Install librdkafka..." && \
    git clone --depth 1 https://github.com/edenhill/librdkafka /home/carma-streets/ext/librdkafka && \
    cd /home/carma-streets/ext/librdkafka && \
    ./configure --prefix=/usr && \
    make -j && \
    make -j install && \
    # Install rapidjson
    echo " ------> Install rapidjson..." && \
    git clone --depth 1 https://github.com/Tencent/rapidjson /home/carma-streets/ext/rapidjson/ && \
    mkdir -p /home/carma-streets/ext/rapidjson/build && \
    cd /home/carma-streets/ext/rapidjson/build && \
    cmake .. && \
    make -j install && \
    # Install streets_service_base
    echo " ------> Install streets service base library from streets_utils..." && \
    mkdir -p /home/carma-streets/streets_utils/streets_service_base/build && \
    cd /home/carma-streets/streets_utils/streets_service_base/build && \
    cmake -DCMAKE_BUILD_TYPE="Debug" .. && \
    make -j install && \
    # Install qhttpengine
    echo " ------> Install qhttpengine..." && \
    cd /home/carma-streets/ext && \
    git clone --depth 1 https://github.com/etherealjoy/qhttpengine.git /home/carma-streets/ext/qhttpengine && \
    mkdir /home/carma-streets/ext/qhttpengine/build && \
    cd /home/carma-streets/ext/qhttpengine/build && \
    cmake .. && \
    make -j install && \
    # Install intersection_server_api
    echo " ------> Install streets intersection api server from streets_utils..." && \
    mkdir -p /home/carma-streets/streets_utils/streets_api/intersection_server_api/build && \
    cd /home/carma-streets/streets_utils/streets_api/intersection_server_api/build && \
    cmake -DCMAKE_BUILD_TYPE="Debug" .. && \
    make -j install && \
    # Install kafka-clients
    echo " ------> Install kafka-clients..." && \
    mkdir -p /home/carma-streets/kafka_clients/build && \
    cd /home/carma-streets/kafka_clients/build && \
    cmake -DCMAKE_BUILD_TYPE="Debug" .. && \
    make -j install && \
    # Install PROJ, a package for coordinate transformations
    git clone --depth 1 https://github.com/OSGeo/PROJ.git /home/carma-streets/PROJ --branch 6.2.1 && \
    cd /home/carma-streets/PROJ && \
    ./autogen.sh && \
    ./configure && \
    make -j install && \
    # Download a cmake module for PROJ
    cd /usr/share/cmake-3.10/Modules && curl -O https://raw.githubusercontent.com/mloskot/cmake-modules/master/modules/FindPROJ4.cmake && \
    sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list' && \
    apt-key adv --fetch-keys https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc && \
    # Install CARMA Ready Lanelet2
    # Lanelet2 is written on the assumption ROS exists. Therefore some special steps are needed for compilation
    # First catkin_make must be installed. The version needed is only available with the ros-melodic-catkin build
    # This means the ros source list must be added to to allow this installation
    # Additionally, the python-rospkg package managment tool is required for catkin to work properly 
    apt-get update && \
    apt-get install -y ros-melodic-catkin && \
    # Once catkin is installed only the required lanelet2 packages will be pulled in from carma
    # NOTE: The lanelet2_python package requires additional dependencies that have not yet been installed so it is removed for now
    echo " ------> Install Lanelet2..." && \
    mkdir -p /home/carma-streets/carma_lanelet2/src && \
    cd /home/carma-streets/carma_lanelet2/src && \
    git init && \
    echo "temp" && \
    git remote add origin -f https://github.com/usdot-fhwa-stol/autoware.ai.git && \
    git config core.sparsecheckout true	&& \
    echo "common/hardcoded_params/*" >> .git/info/sparse-checkout && \
    echo "common/lanelet2_extension/*" >> .git/info/sparse-checkout && \
    echo "lanelet2/*" >> .git/info/sparse-checkout && \
    echo "mrt_cmake_modules/*" >> .git/info/sparse-checkout && \
    git pull --depth 1 origin refactor_lanelet2_extension && \
    git checkout refactor_lanelet2_extension && \
    rm -r lanelet2/lanelet2_python lanelet2/lanelet2_examples && \
    cd /home/carma-streets/carma_lanelet2 && \
    # In order to trick lanelet2 into building the ROS_VERSION environment variable must be set
    # In order to fully decouple lanelet2_extension from ros the LANELET2_EXTENSION_LOGGER_TYPE environment variable must be set
    source /opt/ros/melodic/setup.bash && \
    apt-get install -y python-rospkg libeigen3-dev && \
    ROS_VERSION=1 LANELET2_EXTENSION_LOGGER_TYPE=1 catkin_make install && \
    # Install intersection_model
    echo " ------> compile examples..." && \
    mkdir -p /home/carma-streets/intersection_model/build && \
    cd /home/carma-streets/intersection_model/build && \
    source /opt/ros/melodic/setup.bash && \
    source /home/carma-streets/carma_lanelet2/install/setup.bash && \
    cmake -DCMAKE_BUILD_TYPE="Debug" .. && \
    # FIXME: find out if this is actually supposed to make install because before was only make
    make -j && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR  /home/carma-streets/intersection_model/build/
# Set metadata labels
RUN echo "------> Setting metadata labels..."
LABEL org.label-schema.schema-version="1.0"
LABEL org.label-schema.name="intersection_model"
LABEL org.label-schema.description="Base image for intersection_model"
LABEL org.label-schema.vendor="Leidos"
LABEL org.label-schema.version="${VERSION}"
LABEL org.label-schema.url="https://highways.dot.gov/research/research-programs/operations"
LABEL org.label-schema.vcs-url="https://github.com/usdot-fhwa-stol/carma-streets"
LABEL org.label-schema.vcs-ref=${VCS_REF}
LABEL org.label-schema.build-date=${BUILD_DATE}


CMD ["/home/carma-streets/intersection_model/build/src/server/intersection-server"]