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

# script executes all kafka_clients and scheduling service build and coverage steps so that they can be singularly
# wrapped by the sonarcloud build-wrapper
set -e
# For lanelet aware streets services like message_services and intersection_model
source /opt/ros/noetic/setup.bash
source /opt/carma_lanelet2/setup.bash

COVERAGE_FLAGS="-g --coverage -fprofile-arcs -ftest-coverage"

# make install for these subdirectories
MAKE_INSTALL_DIRS=(
    "streets_utils/json_utils"
    "streets_utils/streets_messages"
    "streets_utils/streets_service_configuration"
    "kafka_clients"
    "streets_utils/streets_service_base"
    "streets_utils/streets_vehicle_list"
    "streets_utils/streets_tsc_configuration"
    "streets_utils/streets_desired_phase_plan"
    "streets_utils/streets_phase_control_schedule"
    "streets_utils/streets_timing_plan"
    "streets_utils/streets_signal_phase_and_timing"
    "streets_utils/streets_api/intersection_client_api"
    "streets_utils/streets_vehicle_scheduler"
    "streets_utils/streets_api/intersection_server_api"
    "streets_utils/streets_signal_optimization"
    "streets_utils/streets_snmp_cmd"
)

# only make for these subdirectories
MAKE_ONLY_DIRS=(
    "scheduling_service"
    "intersection_model"
    "message_services"
    "signal_opt_service"
    "tsc_client_service"
    "sensor_data_sharing_service"
)

for DIR in "${MAKE_INSTALL_DIRS[@]}" "${MAKE_ONLY_DIRS[@]}"; do
    cd /home/carma-streets/"$DIR"
    # Avoid catkin rebuilding gtest/gmock executables
    # Build on libraries position independent
    cmake -Bbuild -DCMAKE_CXX_FLAGS="${COVERAGE_FLAGS}" -DCMAKE_C_FLAGS="${COVERAGE_FLAGS}" -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_POSITION_INDEPENDENT_CODE=ON  -DCATKIN_ENABLE_TESTING=OFF -DCMAKE_PREFIX_PATH="/opt/carma/cmake/;/opt/carma_lanelet2/" 
    cmake --build build
    for MAKE_INSTALL_DIR in "${MAKE_INSTALL_DIRS[@]}"; do
        if [ "$DIR" == "$MAKE_INSTALL_DIR" ]; then
            cmake --install build 
        fi
    done
done
