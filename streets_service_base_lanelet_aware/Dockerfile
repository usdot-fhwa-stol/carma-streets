FROM usdotfhwastoldev/streets_service_base:focal
ENV LANELET2_MAP="/home/carma-streets/MAP/Intersection.osm"
RUN cat install_lanelet2_dependencies.sh
RUN /opt/carma-streets/build_scripts/install_lanelet2_dependencies.sh
