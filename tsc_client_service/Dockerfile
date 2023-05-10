ARG UBUNTU_VERSION=jammy-20230126

FROM ubuntu:$UBUNTU_VERSION

COPY ./build_scripts /home/carma-streets/build_scripts
WORKDIR /home/carma-streets/build_scripts
RUN ./install_dependencies.sh

RUN mkdir -p /home/carma-streets/

# Install librdkafka
RUN echo " ------> Install librdkafka..."
WORKDIR /home/carma-streets/ext
RUN git clone https://github.com/confluentinc/librdkafka.git 
WORKDIR /home/carma-streets/ext/librdkafka/
RUN cmake -H. -B_cmake_build
RUN cmake --build _cmake_build
RUN cmake --build _cmake_build --target install

# Install rapidjson
RUN echo " ------> Install rapidjson..."
WORKDIR /home/carma-streets/ext
RUN git clone  https://github.com/Tencent/rapidjson
WORKDIR /home/carma-streets/ext/rapidjson/
RUN mkdir build
WORKDIR /home/carma-streets/ext/rapidjson/build
RUN cmake .. && make -j
RUN make install

# Install kafka-clients
RUN echo " ------> Install kafka-clients..."
COPY ./kafka_clients/ /home/carma-streets/kafka_clients
WORKDIR  /home/carma-streets/kafka_clients
RUN mkdir build
WORKDIR  /home/carma-streets/kafka_clients/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

COPY ./streets_utils/ /home/carma-streets/streets_utils

# Install streets_service_configuration
RUN echo " ------> Install streets service base library from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_service_configuration
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_service_configuration/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

# Install streets_service_base
RUN echo " ------> Install streets service base library from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_service_base
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_service_base/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

# Install intersection_client_api 
RUN echo " ------> Install intersection client library from streets_utils..."
WORKDIR  /home/carma-streets/streets_utils/streets_api/intersection_client_api
RUN mkdir build
WORKDIR  /home/carma-streets/streets_utils/streets_api/intersection_client_api/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

# Install streets_tsc_configuration
RUN echo " ------> Install streets_tsc_configuration library from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_tsc_configuration
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_tsc_configuration/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

# Install streets_desired_phase_plan
RUN echo " ------> Install streets desired phase plan from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_desired_phase_plan
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_desired_phase_plan/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

# Install streets_signal_phase_and_timing
RUN echo " ------> Install streets service base library from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_signal_phase_and_timing
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_signal_phase_and_timing/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install


# Install net-snmp
WORKDIR /home/carma-streets/ext/
RUN apt-get update  && apt-get install -y wget libperl-dev
RUN wget http://sourceforge.net/projects/net-snmp/files/net-snmp/5.9.1/net-snmp-5.9.1.tar.gz 
RUN tar -xvzf /home/carma-streets/ext/net-snmp-5.9.1.tar.gz
WORKDIR /home/carma-streets/ext/net-snmp-5.9.1/
RUN ./configure --with-default-snmp-version="1" --with-sys-contact="@@no.where" --with-sys-location="Unknown" --with-logfile="/var/log/snmpd.log" --with-persistent-directory="/var/net-snmp"
RUN make
RUN make install


COPY ./tsc_client_service/ /home/carma-streets/tsc_client_service
# Install streets_service_base
RUN echo " ------> Install streets service base library from streets_utils..."
WORKDIR /home/carma-streets/tsc_client_service
RUN mkdir build
WORKDIR /home/carma-streets/tsc_client_service/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN ldconfig

# Set metadata labels
RUN echo "------> Setting metadata labels..."
LABEL org.label-schema.schema-version="1.0"
LABEL org.label-schema.name="tsc_service"
LABEL org.label-schema.description="Base image for tsc_service"
LABEL org.label-schema.vendor="Leidos"
LABEL org.label-schema.version="${VERSION}"
LABEL org.label-schema.url="https://highways.dot.gov/research/research-programs/operations"
LABEL org.label-schema.vcs-url="https://github.com/usdot-fhwa-stol/carma-streets"
LABEL org.label-schema.vcs-ref=${VCS_REF}
LABEL org.label-schema.build-date=${BUILD_DATE}

# Add docker-compose-wait tool -------------------
ENV WAIT_VERSION 2.7.2
ADD https://github.com/ufoscout/docker-compose-wait/releases/download/$WAIT_VERSION/wait /wait
RUN chmod +x /wait

CMD ["/home/carma-streets/tsc_client_service/build/traffic_signal_controller_service"]