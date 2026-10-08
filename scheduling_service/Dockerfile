ARG UBUNTU_VERSION=jammy
ARG DOCKER_ORG=usdotfhwastoldev
ARG DOCKER_TAG=develop

FROM ${DOCKER_ORG}/streets_service_base:${DOCKER_TAG}-${UBUNTU_VERSION}
# Set bash as the default shell for subsequent RUN commands
SHELL ["/bin/bash", "-c"]

RUN apt install -y libssl-dev qtbase5-dev

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
RUN echo " ------> Install streets service configuration library from streets_utils..."
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



# Install intersection_client
RUN echo " ------> Install intersection client library from streets_utils..."
WORKDIR  /home/carma-streets/streets_utils/streets_api/intersection_client_api
RUN mkdir build
WORKDIR  /home/carma-streets/streets_utils/streets_api/intersection_client_api/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

# Install streets_vehicle_list
RUN echo " ------> Install streets_vehicle_list library from streets_utils..."
WORKDIR  /home/carma-streets/streets_utils/streets_vehicle_list/
RUN mkdir build
WORKDIR  /home/carma-streets/streets_utils/streets_vehicle_list/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

RUN echo " ------> Install streets_vehicle_scheduler library from streets_utils..."
WORKDIR  /home/carma-streets/streets_utils/streets_desired_phase_plan/
RUN mkdir build
WORKDIR  /home/carma-streets/streets_utils/streets_desired_phase_plan/build
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

# Install streets_signal_phase_and_timing
RUN echo " ------> Install streets_signal_phase_and_timing library from streets_utils..."
WORKDIR  /home/carma-streets/streets_utils/streets_signal_phase_and_timing/
RUN mkdir build
WORKDIR  /home/carma-streets/streets_utils/streets_signal_phase_and_timing/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

# Install streets_vehicle_scheduler
RUN echo " ------> Install streets_vehicle_scheduler library from streets_utils..."
WORKDIR  /home/carma-streets/streets_utils/streets_vehicle_scheduler/
RUN mkdir build
WORKDIR  /home/carma-streets/streets_utils/streets_vehicle_scheduler/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install


RUN echo " ------> compile scheduling-service..."
COPY ./scheduling_service/ /home/carma-streets/scheduling_service/
WORKDIR  /home/carma-streets/scheduling_service
RUN mkdir build
WORKDIR  /home/carma-streets/scheduling_service/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN ldconfig

WORKDIR  /home/carma-streets/scheduling_service/build/

# Build args for Open Containers Annotations (https://specs.opencontainers.org/image-spec/annotations/)
ARG VERSION
ARG VCS_REF
ARG BUILD_DATE

# Set metadata labels
LABEL org.opencontainers.image.title="scheduling_service"
LABEL org.opencontainers.image.description="Base image for scheduling_service"
LABEL org.opencontainers.image.vendor="Leidos"
LABEL org.opencontainers.image.version=${VERSION}
LABEL org.opencontainers.image.url="https://highways.dot.gov/research/research-programs/operations"
LABEL org.opencontainers.image.source="https://github.com/usdot-fhwa-stol/carma-streets"
LABEL org.opencontainers.image.revision=${VCS_REF}
LABEL org.opencontainers.image.created=${BUILD_DATE}
LABEL org.opencontainers.image.base.name="docker.io/usdotfhwastoldev/streets_service_base:${DOCKER_TAG}-${UBUNTU_VERSION}"

CMD ["/home/carma-streets/scheduling_service/build/scheduling_service"]
