FROM usdotfhwastol/streets_service_base:carma-system-4.5.0-focal
ENV LANELET2_MAP="/home/carma-streets/MAP/Intersection.osm"
COPY ./build_scripts /opt/carma-streets/build_scripts
RUN cat install_lanelet2_dependencies.sh
RUN /opt/carma-streets/build_scripts/install_lanelet2_dependencies.sh
