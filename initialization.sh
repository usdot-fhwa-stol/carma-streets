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
DEBUG="FALSE"

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

# Displaying all available versions
echo "Available versions:"
echo "$updated_tags"

# select a version or accept the latest version as default
read -r -p "Enter CARMA Streets Version (choose from the above, or press Enter to use the latest version $latest_version): " chosen_version
CARMA_STREETS_VERSION=${chosen_version:-$latest_version}
