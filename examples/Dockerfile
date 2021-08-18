FROM ubuntu:bionic-20210702

# Avoid interactive prompts during the building of this docker image
ARG DEBIAN_FRONTEND="noninteractive"

RUN apt-get update  && apt-get install -y sudo cmake gcc-7 g++-7 libboost1.65-dev libboost-thread1.65-dev libboost-regex1.65-dev libboost-log1.65-dev libboost-program-options1.65-dev libboost1.65-all-dev libxerces-c-dev libcurl4-openssl-dev libsnmp-dev libmysqlclient-dev libjsoncpp-dev uuid-dev libusb-dev libusb-1.0-0-dev libftdi-dev swig liboctave-dev gpsd libgps-dev portaudio19-dev libsndfile1-dev libglib2.0-dev libglibmm-2.4-dev libpcre3-dev libsigc++-2.0-dev libxml++2.6-dev libxml2-dev liblzma-dev dpkg-dev libmysqlcppconn-dev libev-dev libuv-dev git vim zip build-essential libssl-dev qtbase5-dev qtbase5-dev-tools curl libqhttpengine-dev libgtest-dev libcpprest-dev libpugixml-dev libgeographic-dev automake autotools-dev sqlite3 libsqlite3-dev

RUN mkdir -p /home/carma-streets-ws/


# Install google test
RUN echo " ------> Install googletest..."
WORKDIR /home/carma-streets-ws/carma-streets/
RUN mkdir -p /home/carma-streets-ws/carma-streets/ext
WORKDIR /home/carma-streets-ws/carma-streets/ext
RUN git clone https://github.com/google/googletest/
WORKDIR /home/carma-streets-ws/carma-streets/ext/googletest/
RUN cmake .
RUN make
RUN sudo make install

# Install librdkafka
RUN echo " ------> Install librdkafka..."
WORKDIR /home/carma-streets-ws/carma-streets/ext
RUN git clone https://github.com/edenhill/librdkafka
WORKDIR /home/carma-streets-ws/carma-streets/ext/librdkafka/
RUN ./configure --prefix=/usr
RUN make
RUN sudo make install


# Install spdlog
RUN echo " ------> Install spdlog... "
WORKDIR /home/carma-streets-ws/carma-streets/ext
RUN git clone https://github.com/gabime/spdlog.git
WORKDIR /home/carma-streets-ws/carma-streets/ext/spdlog/
RUN mkdir build 
WORKDIR /home/carma-streets-ws/carma-streets/ext/spdlog/build
RUN cmake .. && make -j
RUN sudo make install


# Install rapidjson
RUN echo " ------> Install rapidjson..."
WORKDIR /home/carma-streets-ws/carma-streets/ext
RUN git clone  https://github.com/Tencent/rapidjson
WORKDIR /home/carma-streets-ws/carma-streets/ext/rapidjson/
RUN mkdir build
WORKDIR /home/carma-streets-ws/carma-streets/ext/rapidjson/build
RUN cmake .. && make -j
RUN sudo make install

# Install kafka-clients
RUN echo " ------> Install kafka-clients..."
RUN ls -lrt
COPY ./ /home/carma-streets-ws/carma-streets
RUN ls -lrt
WORKDIR  /home/carma-streets-ws/carma-streets/kafka_clients
RUN  rm -rf build && mkdir build
WORKDIR  /home/carma-streets-ws/carma-streets/kafka_clients/build
RUN cmake ..
RUN make
RUN sudo make install

# Install PROJ, a package for coordinate transformations
RUN sudo git clone https://github.com/OSGeo/PROJ.git /home/carma-streets-ws/PROJ --branch 6.2.1 && \
        cd /home/carma-streets-ws/PROJ && \
        sudo ./autogen.sh && \
        sudo ./configure && \
        sudo make && \
        sudo make install
        
# Download a cmake module for PROJ
RUN cd /usr/share/cmake-3.10/Modules && sudo curl -O https://raw.githubusercontent.com/mloskot/cmake-modules/master/modules/FindPROJ4.cmake

ENV DEBIAN_FRONTEND=noninteractive

# Install CARMA Ready Lanelet2
RUN echo " ------> Install Lanelet2..."
WORKDIR  /home/carma-streets-ws/
RUN mkdir carma_lanelet2 && \
    cd carma_lanelet2 && \
    mkdir src && \
    cd src && \
    git init && \
    git remote add origin -f https://github.com/usdot-fhwa-stol/autoware.ai.git && \
    git config core.sparsecheckout true	&& \
    echo "common/hardcoded_params/*" >> .git/info/sparse-checkout && \
    echo "common/lanelet2_extension/*" >> .git/info/sparse-checkout && \
    echo "lanelet2/*" >> .git/info/sparse-checkout && \
    git pull --depth 1 origin refactor_lanelet2_extension && \
    git checkout refactor_lanelet2_extension

WORKDIR  /home/carma-streets-ws/carma_lanelet2

RUN sudo apt-get install lsb-release && \
    sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list' && \
    curl -s https://raw.githubusercontent.com/ros/rosdistro/master/ros.asc | sudo apt-key add -

RUN sudo apt-get update && \
    sudo apt-get install -y catkin && \
    LANELET2_EXTENSION_LOGGER_TYPE=1 catkin_make install && \
    echo "source /home/carma-streets-ws/install/setup.bash" >> ~/.bashrc

# Install carma-streets-examples
RUN echo " ------> compile examples..."
WORKDIR  /home/carma-streets-ws/carma-streets/examples
RUN  rm -rf build && mkdir build
WORKDIR  /home/carma-streets-ws/carma-streets/examples/build
RUN cmake ..
RUN make

WORKDIR  /home/carma-streets-ws/carma-streets/examples/build/

# Set metadata labels
RUN echo "------> Setting metadata labels..."
LABEL org.label-schema.schema-version="1.0"
LABEL org.label-schema.name="carma-streets-SonarCloud"
LABEL org.label-schema.description="Base image for CARMA CI testing using SonarCloud"
LABEL org.label-schema.vendor="Leidos"
LABEL org.label-schema.version="${VERSION}"
LABEL org.label-schema.url="https://highways.dot.gov/research/research-programs/operations"
LABEL org.label-schema.vcs-url="https://github.com/usdot-fhwa-stol/carma-streets"
LABEL org.label-schema.vcs-ref=${VCS_REF}
LABEL org.label-schema.build-date=${BUILD_DATE}

CMD ["/home/carma-streets-ws/carma-streets/examples/build/examples"]
