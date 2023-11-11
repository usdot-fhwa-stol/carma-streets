FROM ghcr.io/usdot-fhwa-stol/carma-builds-x64:bionic
COPY ./build_scripts /home/carma-streets/build_scripts
ENV LANELET2_MAP="/home/carma-streets/MAP/Intersection.osm"
RUN /home/carma-streets/build_scripts/install_lanelet2_dependencies.sh