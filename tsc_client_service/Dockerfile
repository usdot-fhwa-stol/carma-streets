ARG UBUNTU_VERSION=jammy
ARG DOCKER_ORG=usdotfhwastoldev
ARG DOCKER_TAG=develop
FROM ${DOCKER_ORG}/streets_service_base:${DOCKER_TAG}-${UBUNTU_VERSION}
# Set bash as the default shell for subsequent RUN commands
SHELL ["/bin/bash", "-c"]

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

# Install streets_phase_control_schedule
RUN echo " ------> Install streets phase control schedule from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_phase_control_schedule
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_phase_control_schedule/build
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

# Install streets_snmp_cmd
RUN echo " ------> Install streets snmp command from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_snmp_cmd
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_snmp_cmd/build
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
RUN echo " ------> compile tsc_client_service..."
WORKDIR /home/carma-streets/tsc_client_service
RUN mkdir build
WORKDIR /home/carma-streets/tsc_client_service/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN ldconfig

# Build args for Open Containers Annotations (https://specs.opencontainers.org/image-spec/annotations/)
ARG VERSION
ARG VCS_REF
ARG BUILD_DATE

# Set metadata labels
LABEL org.opencontainers.image.title="tsc_service"
LABEL org.opencontainers.image.description="Base image for tsc_service"
LABEL org.opencontainers.image.vendor="Leidos"
LABEL org.opencontainers.image.version=${VERSION}
LABEL org.opencontainers.image.url="https://highways.dot.gov/research/research-programs/operations"
LABEL org.opencontainers.image.source="https://github.com/usdot-fhwa-stol/carma-streets"
LABEL org.opencontainers.image.revision=${VCS_REF}
LABEL org.opencontainers.image.created=${BUILD_DATE}
LABEL org.opencontainers.image.base.name="docker.io/usdotfhwastoldev/streets_service_base:${DOCKER_TAG}-${UBUNTU_VERSION}"

# Add docker-compose-wait tool -------------------
ENV WAIT_VERSION 2.7.2
ADD https://github.com/ufoscout/docker-compose-wait/releases/download/$WAIT_VERSION/wait /wait
RUN chmod +x /wait

CMD ["/home/carma-streets/tsc_client_service/build/traffic_signal_controller_service"]
