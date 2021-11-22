FROM ubuntu:bionic-20210702

<<<<<<< HEAD
<<<<<<< HEAD
RUN apt-get update  && apt-get install -y cmake libboost1.65-all-dev git autotools-dev qtbase5-dev qtbase5-dev-tools libqhttpengine-dev libssl-dev
=======
RUN apt-get update  && apt-get install -y cmake libboost1.65-all-dev git autotools-dev automake sqlite3 libsqlite3-dev curl libpugixml-dev libgeographic-dev qtbase5-dev qtbase5-dev-tools libqhttpengine-dev libssl-dev
>>>>>>> the intersection_client class has been added. the osm class must be removed!
=======
RUN apt-get update  && apt-get install -y cmake libboost1.65-all-dev git autotools-dev qtbase5-dev qtbase5-dev-tools libqhttpengine-dev libssl-dev
>>>>>>> circleci update

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

# Install kafka-clients
RUN echo " ------> Install kafka-clients..."
COPY ./kafka_clients/ /home/carma-streets/kafka_clients
WORKDIR  /home/carma-streets/kafka_clients
RUN mkdir build
WORKDIR  /home/carma-streets/kafka_clients/build
RUN cmake ..
RUN make
RUN make install

RUN echo " ------> Install intersection client library from carma-utils..."
COPY ./streets_utils/ /home/carma-streets/streets_utils
WORKDIR  /home/carma-streets/streets_utils/streets_api/intersection_client_api
RUN mkdir build
WORKDIR  /home/carma-streets/streets_utils/streets_api/intersection_client_api/build
RUN cmake ..
RUN make
RUN make install


RUN echo " ------> compile scheduling-service..."
COPY ./scheduling_service/ /home/carma-streets/scheduling_service/
WORKDIR  /home/carma-streets/scheduling_service
RUN mkdir build
WORKDIR  /home/carma-streets/scheduling_service/build
RUN cmake ..
RUN make

WORKDIR  /home/carma-streets/scheduling_service/build/

# Set metadata labels
# Set metadata labels
RUN echo "------> Setting metadata labels..."
LABEL org.label-schema.schema-version="1.0"
LABEL org.label-schema.name="scheduling_service"
LABEL org.label-schema.description="Base image for scheduling_service"
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

CMD ["/home/carma-streets/scheduling_service/build/scheduling_service"]
