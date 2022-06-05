FROM ubuntu:bionic-20210702

RUN apt-get update  && apt-get install -y cmake libboost1.65-all-dev build-essential git qtbase5-dev qtbase5-dev-tools libqhttpengine-dev libssl-dev

RUN mkdir -p /home/carma-streets/

# Install google test
RUN echo " ------> Install googletest..."
WORKDIR /home/carma-streets/
RUN mkdir -p /home/carma-streets/ext
WORKDIR /home/carma-streets/ext
RUN git clone https://github.com/google/googletest/
WORKDIR /home/carma-streets/ext/googletest/
RUN cmake .
RUN make
RUN make install

# Install spdlog
RUN echo " ------> Install spdlog... "
WORKDIR /home/carma-streets/ext
RUN git clone https://github.com/gabime/spdlog.git
WORKDIR /home/carma-streets/ext/spdlog/
RUN mkdir build 
WORKDIR /home/carma-streets/ext/spdlog/build
RUN cmake .. && make -j
RUN make install

# Install rapidjson
RUN echo " ------> Install rapidjson..."
WORKDIR /home/carma-streets/ext
RUN git clone  https://github.com/Tencent/rapidjson
WORKDIR /home/carma-streets/ext/rapidjson/
RUN mkdir build
WORKDIR /home/carma-streets/ext/rapidjson/build
RUN cmake .. && make -j
RUN make install

COPY ./streets_utils/ /home/carma-streets/streets_utils
# Install streets_service_base
RUN echo " ------> Install streets service base library from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_service_base
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_service_base/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

# Install net-snmp
WORKDIR /home/
RUN apt-get update  && apt-get install -y wget libperl-dev
RUN wget http://sourceforge.net/projects/net-snmp/files/net-snmp/5.9.1/net-snmp-5.9.1.tar.gz 
RUN tar -xvzf /home/net-snmp-5.9.1.tar.gz
WORKDIR /home/net-snmp-5.9.1/
RUN ./configure
RUN make
RUN make install


COPY ./snmp_client_service/ /home/carma-streets/snmp_client_service
# Install streets_service_base
# RUN echo " ------> Install streets service base library from streets_utils..."
# WORKDIR /home/carma-streets/streets_utils/snmp_client_service
# RUN mkdir build
# WORKDIR /home/carma-streets/streets_utils/snmp_client_service/build
# RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
# RUN make
# RUN make install

# Set metadata labels
RUN echo "------> Setting metadata labels..."
LABEL org.label-schema.schema-version="1.0"
LABEL org.label-schema.name="snmp_client"
LABEL org.label-schema.description="Base image for snmp_client"
LABEL org.label-schema.vendor="Leidos"
LABEL org.label-schema.version="${VERSION}"
LABEL org.label-schema.url="https://highways.dot.gov/research/research-programs/operations"
LABEL org.label-schema.vcs-url="https://github.com/usdot-fhwa-stol/carma-streets"
LABEL org.label-schema.vcs-ref=${VCS_REF}
LABEL org.label-schema.build-date=${BUILD_DATE}