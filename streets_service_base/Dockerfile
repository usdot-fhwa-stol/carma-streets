ARG UBUNTU_CODENAME
FROM ghcr.io/usdot-fhwa-stol/carma-builds-x64:carma-system-4.5.0-${UBUNTU_CODENAME}
COPY ./build_scripts /opt/carma-streets/build_scripts
RUN /opt/carma-streets/build_scripts/install_streets_service_base_dependencies.sh
