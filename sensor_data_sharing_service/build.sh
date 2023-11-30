#!/bin/bash
#  Copyright (C) 2018-2020 LEIDOS.
# 
#  Licensed under the Apache License, Version 2.0 (the "License"); you may not
#  use this file except in compliance with the License. You may obtain a copy of
#  the License at
# 
#  http://www.apache.org/licenses/LICENSE-2.0
# 
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
#  License for the specific language governing permissions and limitations under
#  the License.

# Build script to build Sensor Data Sharing Service
set -e
# Source ros and lanelet2 for lanelet aware services
source /opt/ros/melodic/setup.bash
source /opt/carma_lanelet2/setup.bash

COVERAGE_FLAGS=""

# make install for these subdirectories
MAKE_INSTALL_DIRS=(
    "streets_utils/json_utils"
    "streets_utils/streets_messages"
    "streets_utils/streets_service_configuration"
    "kafka_clients"
    "streets_utils/streets_service_base"
)

# only make for these subdirectories
MAKE_ONLY_DIRS=(
    "sensor_data_sharing_service"
)

for DIR in "${MAKE_INSTALL_DIRS[@]}" "${MAKE_ONLY_DIRS[@]}"; do
    cd /home/carma-streets/"$DIR"
    cmake -Bbuild -DCMAKE_CXX_FLAGS="${COVERAGE_FLAGS}" -DCMAKE_C_FLAGS="${COVERAGE_FLAGS}" -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_PREFIX_PATH="/opt/carma/cmake/;/opt/carma_lanelet2/" 
    cmake --build build
    for MAKE_INSTALL_DIR in "${MAKE_INSTALL_DIRS[@]}"; do
        if [ "$DIR" == "$MAKE_INSTALL_DIR" ]; then
            cmake --install build
        fi
    done
done