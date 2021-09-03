FROM ubuntu:bionic-20210702

RUN apt-get update  && apt-get install -y sudo cmake gcc-7 g++-7 libboost1.65-dev libboost-thread1.65-dev libboost-regex1.65-dev libboost-log1.65-dev libboost-program-options1.65-dev libboost1.65-all-dev libxerces-c-dev libcurl4-openssl-dev libsnmp-dev libmysqlclient-dev libjsoncpp-dev uuid-dev libusb-dev libusb-1.0-0-dev libftdi-dev swig liboctave-dev gpsd libgps-dev portaudio19-dev libsndfile1-dev libglib2.0-dev libglibmm-2.4-dev libpcre3-dev libsigc++-2.0-dev libxml++2.6-dev libxml2-dev liblzma-dev dpkg-dev libmysqlcppconn-dev libev-dev libuv-dev git vim zip build-essential libssl-dev qtbase5-dev qtbase5-dev-tools curl libqhttpengine-dev libgtest-dev libcpprest-dev 

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
COPY ./ /home/carma-streets-ws/carma-streets
WORKDIR  /home/carma-streets-ws/carma-streets/kafka_clients
RUN  rm -rf build && mkdir build
WORKDIR  /home/carma-streets-ws/carma-streets/kafka_clients/build
RUN cmake ..
RUN make
RUN sudo make install

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