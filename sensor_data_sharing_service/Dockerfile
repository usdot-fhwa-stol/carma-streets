FROM usdotfhwastoldev/streets_service_base_lanelet_aware:bionic
COPY ./sensor_data_sharing_service/ /home/carma-streets/sensor_data_sharing_service
COPY ./streets_utils/ /home/carma-streets/streets_utils
COPY ./kafka_clients/ /home/carma-streets/kafka_clients
RUN /home/carma-streets/sensor_data_sharing_service/build.sh
RUN chmod u+x /opt/carma_lanelet2/setup.bash

LABEL org.label-schema.schema-version="1.0"
LABEL org.label-schema.name="sensor_data_sharing_service"
LABEL org.label-schema.description="Image for Sensor Data Sharing Service"
LABEL org.label-schema.vendor="Leidos"
LABEL org.label-schema.version="${VERSION}"
LABEL org.label-schema.url="https://highways.dot.gov/research/research-programs/operations"
LABEL org.label-schema.vcs-url="https://github.com/usdot-fhwa-stol/carma-streets"
LABEL org.label-schema.vcs-ref=${VCS_REF}
LABEL org.label-schema.build-date=${BUILD_DATE}


ENTRYPOINT ["/bin/bash", "-c", "source /opt/carma_lanelet2/setup.bash && /home/carma-streets/sensor_data_sharing_service/build/sensor_data_sharing_service"]