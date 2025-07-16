ARG DOCKER_ORG=usdotfhwastoldev
ARG UBUNTU_CODENAME=focal
ARG DOCKER_TAG=develop
FROM ${DOCKER_ORG}/streets_service_base:${DOCKER_TAG}-${UBUNTU_CODENAME}
ENV LANELET2_MAP="/home/carma-streets/MAP/Intersection.osm"
RUN /opt/carma-streets/build_scripts/install_lanelet2_dependencies.sh
