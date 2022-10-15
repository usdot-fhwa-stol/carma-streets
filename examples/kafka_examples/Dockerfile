FROM ubuntu:bionic-20210702

RUN apt-get update  && apt-get install -y cmake git build-essential libboost1.65-all-dev

RUN mkdir -p /home/carma-streets/


# Install google test
RUN echo " ------> Install googletest..."
WORKDIR /home//carma-streets/
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
RUN git clone https://github.com/Tencent/rapidjson --depth 1 --branch 27c3a8dc0e2c9218fe94986d249a12b5ed838f1d
WORKDIR /home/carma-streets/ext/rapidjson/
RUN mkdir build
WORKDIR /home/carma-streets/ext/rapidjson/build
RUN cmake .. && make -j
RUN make install

# Install kafka-clients
RUN echo " ------> Install kafka-clients..."
RUN ls -lrt
COPY kafka_clients/ /home/carma-streets/kafka_clients
RUN ls -lrt
WORKDIR  /home/carma-streets/kafka_clients
RUN  rm -rf build && mkdir build
WORKDIR  /home/carma-streets/kafka_clients/build
RUN cmake ..
RUN make
RUN make install

RUN echo " ------> compile examples (Make sure to comment out examples from dockerignore!) ..."
COPY examples/ /home/carma-streets/examples/
WORKDIR  /home/carma-streets/examples
RUN  rm -rf build && mkdir build
WORKDIR  /home/carma-streets/examples/build
RUN cmake ..
RUN make

WORKDIR  /home/carma-streets/examples/build/

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

CMD ["/home/carma-streets/examples/build/examples"]
