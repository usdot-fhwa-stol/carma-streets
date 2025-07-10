#!/bin/bash

# This script is used to initialize the CARMA Streets deployment environment including:
# - Setting up environment variables for docker compose deployment
# - Installing V2X Hub and required runtime dependencies
# - Setting up V2X Hub environment variables for docker compose deployment


# Function to validate latitude and longitude
validate_coordinate() {
  local coord="$1"
  local type="$2"
  # Regular expression to validate a floating point number
  if [[ ! "$coord" =~ ^[+-]?[0-9]+(\.[0-9]+)?$ ]]; then
    echo "Invalid $type format. Please enter a valid number (e.g., 40.7128 or -74.0060)."
    return 1
  fi

  # Validate ranges for latitude (-90 to 90) and longitude (-180 to 180)
  if [[ "$type" == "latitude" ]]; then
    if (( $(echo "$coord < -90" | bc -l) )) || (( $(echo "$coord > 90" | bc -l) )); then
      echo "Latitude must be between -90 and 90 degrees."
      return 1
    fi
  elif [[ "$type" == "longitude" ]]; then
    if (( $(echo "$coord < -180" | bc -l) )) || (( $(echo "$coord > 180" | bc -l) )); then
      echo "Longitude must be between -180 and 180 degrees."
      return 1
    fi
  fi
  return 0
}
# Default values
INSTALL_V2XHUB="FALSE"
CARMA_STREETS_COMPOSE_PROFILES=""
CARMA_STREETS_COMPOSE_PROFILES_AVAILABLE="vehicle_scheduling,signal_optimization,cooperative_perception,debug"
CARMA_STREETS_COMPOSE_PROFILES_DEFAULT="cooperative_perception,debug"
INFRASTRUCTURE_ID_DEFAULT="rsu_1234"
INFRASTRUCTURE_NAME_DEFAULT="East Intersection"
INFRASTRUCTURE_IP_DEFAULT="127.0.0.1"
SENSOR_JSON_DIR_DEFAULT="./sensor_configurations"
SIMULATION_MODE_DEFAULT="FALSE"
CARMA_STREETS_VERSION="develop"
CARMA_STREETS_VERSION_TYPE="develop"
CARMA_STREETS_ORG="usdotfhwastoldev"
reconfigure_choice="N"


## Check for existing .env file
if [ -f .env ]; then
    echo "CARMA Streets Docker Environment configuration file (.env) Already exists. Would you like to reconfigure? (Y/N)"
    read -r reconfigure_choice
    if [[ "$reconfigure_choice" =~ [yY](es)* ]]; then
        echo "Reconfiguring CARMA Streets Docker Environment..."
    fi
fi
# If reconfigure choice is Y or no .env file exists, proceed with the setup
if [[ "$reconfigure_choice" =~ [yY](es)* ]] || [ ! -f .env ]; then


    echo "Setting up the environment..."

    # Prompt user whether they want to deploy a release,release candidate or development version of CARMA Streets
    read -r -p "Do you want to deploy a release, release candidate or development version of CARMA Streets? (release/release_candidate/development, or press Enter to use default as release): " CARMA_STREETS_VERSION_TYPE
    CARMA_STREETS_VERSION_TYPE=${CARMA_STREETS_VERSION_TYPE:-release}
    echo "Selected CARMA Streets Version Type: $CARMA_STREETS_VERSION_TYPE"
    if [[ "$CARMA_STREETS_VERSION_TYPE" == "release" ]]; then
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
        # Displaying all available versions
        echo "Available versions:"
        echo "$updated_tags"

        # select a version or accept the latest version as default
        read -r -p "Enter CARMA Streets Version (choose from the above, or press Enter to use the latest version $latest_version): " chosen_version
        CARMA_STREETS_VERSION=${chosen_version:-$latest_version}
        CARMA_STREETS_ORG="usdotfhwastol"
    elif [[ "$CARMA_STREETS_VERSION_TYPE" == "release_candidate" ]]; then
        echo "Retrieving available Release Candidates':"
        RELEASE_CANDIDATES= git branch -r | grep 'origin/release/' | sed 's|origin/release/||'
        echo "Available Release Candidates:"
        echo "$RELEASE_CANDIDATES"
        # select a version or accept the latest version as default
        read -r -p "Enter CARMA Streets Release Candidate Version (choose from the above): " chosen_version
        CARMA_STREETS_VERSION=${chosen_version}
        CARMA_STREETS_ORG="usdotfhwastolcandidate"
    else
        echo "Seeting CARMA Streets Version to develop ..."
        CARMA_STREETS_VERSION="develop"
        CARMA_STREETS_ORG="usdotfhwastoldev"
    fi
    read -r -p "Install V2X Hub (Y/N, or press Enter to use default as Y): " INSTALL_V2XHUB
    INSTALL_V2XHUB=${INSTALL_V2XHUB:-Y}
    echo "Install V2X Hub: $INSTALL_V2XHUB"

    if [[ "$INSTALL_V2XHUB" =~ [yY](es)*  ]]; then
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
    read -r -p "Enter Sensor Configuration Directory Path (or press Enter to use default as $SENSOR_JSON_DIR_DEFAULT): " SENSOR_JSON_DIR
    SENSOR_JSON_DIR=${SENSOR_JSON_DIR:-$SENSOR_JSON_DIR_DEFAULT}

    # Available CARMA Streets Profiles
    echo "Avaible CARMA Streets Profiles:"
    # Convert the string to an array by splitting on commas
    IFS=',' read -r -a profiles_array <<< "$CARMA_STREETS_COMPOSE_PROFILES_AVAILABLE"
    # Loop through the string, which will now be split by commas
    for item in "${profiles_array[@]}"; do
        echo "$item"
    done

    read -r -p "Enter comma separated list of Profiles to activate: (or press Enter to use default as $CARMA_STREETS_COMPOSE_PROFILES_DEFAULT): " CARMA_STREETS_COMPOSE_PROFILES
    CARMA_STREETS_COMPOSE_PROFILES=${CARMA_STREETS_COMPOSE_PROFILES:-$CARMA_STREETS_COMPOSE_PROFILES_DEFAULT}

    # Write to .env file
    cat <<EOF > .env
    INFRASTRUCTURE_ID="$INFRASTRUCTURE_ID"
    INFRASTRUCTURE_NAME="$INFRASTRUCTURE_NAME"
    INFRASTRUCTURE_IP="$INFRASTRUCTURE_IP"
    SENSOR_JSON_DIR="$SENSOR_JSON_DIR"
    COMPOSE_PROFILES="$CARMA_STREETS_COMPOSE_PROFILES"
    SIMULATION_MODE=$SIMULATION_MODE
    STOL_ORG="$CARMA_STREETS_ORG"
    STOL_TAG="$CARMA_STREETS_VERSION"
EOF
    echo ".env file created successfully."

fi
# Load environment variables from .env file whether it was created or already exists
source .env

# If COMPOSE_PROFILES includes cooperative_perception, check if sensor_configurations/sensors.json exists
echo "Active Compose Profiles: $COMPOSE_PROFILES"
if [[ "$COMPOSE_PROFILES" == *"cooperative_perception"* ]]; then
    if [ ! -f "sensor_configurations/sensors.json" ]; then
        echo "sensor_configurations/sensors.json file does not exist. Cooperative Perception requires a sensor configuration file. Creating Sensor Configuration file..."
        # Check if jq is installed
        if ! command -v jq &> /dev/null; then
            echo "jq is not installed. Installing jq to create sensor configuration json ..."
            sudo apt-get update && sudo apt-get install -y jq
        fi        
        mkdir -p sensor_configurations

        read -p "Enter Latitude (WGS84, e.g., 40.7128): " LATITUDE
        while ! validate_coordinate "$LATITUDE" "latitude"; do
            read -p "Enter Latitude (WGS84, e.g., 40.7128): " LATITUDE
        done

        read -p "Enter Longitude (WGS84, e.g., -74.0060): " LONGITUDE
        while ! validate_coordinate "$LONGITUDE" "longitude"; do
            read -p "Enter Longitude (WGS84, e.g., -74.0060): " LONGITUDE
        done

        # Read Sensor ID
        read -p "Enter Sensor ID (e.g., sensor_1): " SENSOR_ID
        if [ -z "$SENSOR_ID" ]; then
            echo "Sensor ID cannot be empty. Using default sensor_1."
            SENSOR_ID="sensor_1"
        fi
        # Create JSON using jq
        JSON_OUTPUT=$(jq -n \
              --arg lat "$LATITUDE" \
              --arg lon "$LONGITUDE" \
              --arg sensorId "$SENSOR_ID" \
              '[
                {
                    "sensorId": ($sensorId | tostring),
                    "type": "SemanticLidar",
                    "ref": {
                    "type": "WGS84",
                    "location": {
                        "latitude": ($lat | tonumber),
                        "longitude": ($lon | tonumber), 
                        "altitude": 0.0
                    }
                    }
                } 
                ]')
        # Write JSON to sensors.json
        echo "$JSON_OUTPUT" > sensor_configurations/sensors.json
        echo "Sensor configuration file created successfully at sensor_configurations/sensor.json"
    else
        echo "sensor_configurations/sensors.json file exists."
    fi
fi
docker compose pull
# Ask user if they want to deploy CARMA Streets with active profiles displayed
echo "Active Compose Profiles: $COMPOSE_PROFILES"
read -r -p "Do you want to deploy CARMA Streets with the active profiles? (Y/N, or press Enter to use default as Y): " deploy_choice
deploy_choice=${deploy_choice:-Y}
if [[ "$deploy_choice" =~ [yY](es)* ]]; then
    echo "Deploying CARMA Streets with active profiles..."
    docker compose up -d
    echo "CARMA Streets deployed successfully with active profiles: $COMPOSE_PROFILES"
    # Prompt user to ask if they want to add V2X Hub user (yes/no)
    read -r -p "Do you want to add a V2X Hub user? (Y/N, or press Enter to use default as Y): " add_v2x_hub_user
    add_v2x_hub_user=${add_v2x_hub_user:-Y}
    if [[ "$add_v2x_hub_user" =~ [yY](es)* ]]; then
        cd ../V2X-Hub/configuration/
        echo "Adding V2X Hub user ..."
        ./add_v2xhub_user.sh
        echo "V2X Hub user added successfully."
    fi

else
    echo "Skipping CARMA Streets deployment."
    exit 0
fi


