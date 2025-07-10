ARG DOCKER_ORG=usdotfhwastoldev
ARG UBUNTU_CODENAME=focal
FROM ${DOCKER_ORG}/streets_service_base:${UBUNTU_CODENAME}
ENV LANELET2_MAP="/home/carma-streets/MAP/Intersection.osm"
RUN /opt/carma-streets/build_scripts/install_lanelet2_dependencies.sh
