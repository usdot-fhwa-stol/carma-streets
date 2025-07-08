#!/bin/bash

# This script is used to initialize the CARMA Streets deployment environment including:
# - Installing docker and other required runtime dependencies
# - Setting up environment variables for docker compose deployment
# - Installing V2X Hub and required runtime dependencies
# - Setting up V2X Hub environment variables for docker compose deployment
# - Installing Data Collection tools and required runtime dependencies

INSTALL_DOCKER="FALSE"
INSTALL_V2X_HUB="FALSE"
INSTALL_DATA_COLLECTION="FALSE"
CARMA_STREETS_COMPOSE_PROFILES=""
CARMA_STREETS_COMPOSE_PROFILES_AVAILABLE="vehicle_scheduling,signal_optimization,cooperative_perception,debug"
CARMA_STREETS_COMPOSE_PROFILES_DEFAULT="cooperative_perception,debug"
INFRASTRUCTURE_ID_DEFAULT="rsu_1234"
INFRASTRUCTURE_NAME_DEFAULT="East Intersection"
INFRASTRUCTURE_IP_DEFAULT="127.0.0.1"

echo "Setting up the environment..."

# Repository URL
repo_url_latest="https://api.github.com/repos/usdot-fhwa-stol/carma-streets/releases/latest"

# Getting the latest release information using curl
release_info=$(curl -sSL $repo_url_latest)

# Parsing the JSON response to get the tag_name (version) of the latest release
latest_version=$(echo "$release_info" | grep -o '"tag_name": *"[^"]*"' | cut -d '"' -f 4)

# Fetching all tags from Git repository
tags=$(git ls-remote --tags https://github.com/usdot-fhwa-stol/carma-streets.git | awk -F/ '{print $3}' | sort -V)

# Remove curly braces, Properties found, duplicate entries, and show only versions above 7.0
updated_tags=$(echo "$tags" | sed 's/\^{}//;s/^v//' | grep -v '^Properties_Found$' | awk '!seen[$0]++ && $1 >= "7.0"')
# Add develop
updated_tags+=$'\n'"develop"
# Displaying all available versions
echo "Available versions:"
echo "$updated_tags"

# select a version or accept the latest version as default
read -r -p "Enter CARMA Streets Version (choose from the above, or press Enter to use the latest version $latest_version): " chosen_version
CARMA_STREETS_VERSION=${chosen_version:-$latest_version}
echo "Selected CARMA Streets Version: $CARMA_STREETS_VERSION"

read -r -p "Install V2X Hub (TRUE/FALSE, or press Enter to use default as $INSTALL_V2XHUB): " INSTALL_V2XHUB
echo "Install V2X Hub: $INSTALL_V2XHUB"

if [[ "$INSTALL_V2XHUB" == "TRUE" ]]; then
    # clone the V2X Hub repository
    cd ..
    if [ -d "V2X-Hub" ]; then
        echo "V2X Hub directory already exists. Skipping cloning."
        cd V2X-Hub/configuration/
    else
        echo "Cloning V2X Hub repository on path $(pwd)..."
        git clone https://github.com/usdot-fhwa-OPS/V2X-Hub.git
    fi
    
    cd V2X-Hub/configuration/
    # Initialize V2X Hub Docker environment
    echo "Initializing V2X Hub Docker environment..."
    ./initialize_docker_environment.sh
    ./initialize_secrets.sh
    echo "V2X Hub Docker environment initialized successfully."
    echo "Pulling V2X Hub Docker images..."
    docker compose pull
    echo "V2X Hub Docker images pulled successfully."
    cd ../../carma-streets/
    echo "V2X Hub installed and deployed successfully."
else
    echo "Skipping V2X Hub installation. NOTE: To interface with an RSU or CDASim, V2X Hub is required!"
fi
# Infrastructure id
read -r -p "Enter Infrastructure id (or press Enter to use default as $INFRASTRUCTURE_ID_DEFAULT): " INFRASTRUCTURE_ID
INFRASTRUCTURE_ID=${INFRASTRUCTURE_ID:-$INFRASTRUCTURE_ID_DEFAULT}

# Infrastructure name
read -r -p "Enter Infrastructure name (or press Enter to use default as $INFRASTRUCTURE_NAME_DEFAULT): " INFRASTRUCTURE_NAME
INFRASTRUCTURE_NAME=${INFRASTRUCTURE_NAME:-$INFRASTRUCTURE_NAME_DEFAULT}

# INFRASTRUCTURE IP
read -r -p "Enter INFRASTRUCTURE IP (or press Enter to use default as $INFRASTRUCTURE_IP_DEFAULT): " V2XHUB_IP
INFRASTRUCTURE_IP=${INFRASTRUCTURE_IP:-$INFRASTRUCTURE_IP_DEFAULT}

read -r -p "Simulation Mode (TRUE/FALSE, or press Enter to use default as $SIMULATION_MODE_DEFAULT): " SIMULATION_MODE
SIMULATION_MODE=${SIMULATION_MODE:-$SIMULATION_MODE_DEFAULT}

# Sensor Configuration File Path
read -r -p "Enter Sensor Configuration File Path (or press Enter to use default as $SENSOR_JSON_FILE_PATH_DEFAULT): " SENSOR_JSON_FILE_PATH
SENSOR_JSON_FILE_PATH=${SENSOR_JSON_FILE_PATH:-$SENSOR_JSON_FILE_PATH_DEFAULT}

# Available CARMA Streets Profiles
echo "Avaible CARMA Streets Profiles:"
# Loop through the string, which will now be split by commas
for item in $CARMA_STREETS_COMPOSE_PROFILES_AVAILABLE; do
  echo "Profile: $item"
done

read -r -p "Enter comma separated list of Profiles to activate IP (or press Enter to use default as $CARMA_STREETS_COMPOSE_PROFILES_DEFAULT): " CARMA_STREETS_COMPOSE_PROFILES
CARMA_STREETS_COMPOSE_PROFILES=${CARMA_STREETS_COMPOSE_PROFILES:-$CARMA_STREETS_COMPOSE_PROFILES_DEFAULT}
echo "WARNING: This will overwrite the existing .env file if it exists."
read -r -p "Are you sure you want to continue? (Y/N): " overwrite_confirm
if [[ "$overwrite_confirm" =~ [yY](es)* ]]; then
    # Write to .env file
    cat <<EOF > .env
INFRASTRUCTURE_ID=$INFRASTRUCTURE_ID
INFRASTRUCTURE_NAME=$INFRASTRUCTURE_NAME
INFRASTRUCTURE_IP=$INFRASTRUCTURE_IP
SENSOR_JSON_FILE_PATH=$SENSOR_JSON_FILE_PATH
COMPOSE_PROFILES=$CARMA_STREETS_COMPOSE_PROFILES
SIMULATION_MODE=$SIMULATION_MODE
STOL_ORG=usdotfhwastoldev
STOL_TAG=$CARMA_STREETS_VERSION
EOF
    echo ".env file created successfully."
else
    echo "Skipping .env file creation."
fi

docker compose pull
docker compose up -d

if [[ "$INSTALL_V2XHUB" == "TRUE" ]]; then
    cd V2X-Hub/configuration/
    echo "Adding V2X Hub user ..."
    ./add_v2x_hub_user.sh
    echo "V2X Hub user added successfully."
fi