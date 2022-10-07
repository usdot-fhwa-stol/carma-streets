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

# Install librdkafka
RUN echo " ------> Install librdkafka..."
WORKDIR /home/carma-streets/ext
RUN git clone https://github.com/edenhill/librdkafka
WORKDIR /home/carma-streets/ext/librdkafka/
RUN ./configure --prefix=/usr
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

RUN echo " ------> Install intersection client library from streets_utils..."
WORKDIR  /home/carma-streets/streets_utils/streets_api/intersection_client_api
RUN mkdir build
WORKDIR  /home/carma-streets/streets_utils/streets_api/intersection_client_api/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

RUN echo " ------> Install streets_vehicle_list library from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_vehicle_list
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_vehicle_list/build
RUN cmake -DCMAKE_CXX_FLAGS="${COVERAGE_FLAGS}" -DCMAKE_C_FLAGS="${COVERAGE_FLAGS}" -DCMAKE_BUILD_TYPE="Debug" ..
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

RUN echo " ------> Install streets_signal_phase_and_timing library from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_signal_phase_and_timing
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_signal_phase_and_timing/build
RUN cmake -DCMAKE_CXX_FLAGS="${COVERAGE_FLAGS}" -DCMAKE_C_FLAGS="${COVERAGE_FLAGS}" -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

RUN echo " ------> Install streets_tsc_configuration library from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_tsc_configuration
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_tsc_configuration/build
RUN cmake -DCMAKE_CXX_FLAGS="${COVERAGE_FLAGS}" -DCMAKE_C_FLAGS="${COVERAGE_FLAGS}" -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
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

# Install streets_desired_phase_plan_arbitrator
RUN echo " ------> Install streets desired phase plan from streets_utils..."
WORKDIR /home/carma-streets/streets_utils/streets_desired_phase_plan_arbitrator
RUN mkdir build
WORKDIR /home/carma-streets/streets_utils/streets_desired_phase_plan_arbitrator/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make
RUN make install

RUN echo " ------> compile siganl-optimization-service..."
COPY ./signal_opt_service/ /home/carma-streets/signal_opt_service/
WORKDIR  /home/carma-streets/signal_opt_service
RUN mkdir build
WORKDIR  /home/carma-streets/signal_opt_service/build
RUN cmake -DCMAKE_BUILD_TYPE="Debug" ..
RUN make

# Set metadata labels
RUN echo "------> Setting metadata labels..."
LABEL org.label-schema.schema-version="1.0"
LABEL org.label-schema.name="signal_opt_service"
LABEL org.label-schema.description="Base image for signal_opt_service"
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

CMD ["/home/carma-streets/signal_opt_service/build/signal_opt_service"]
