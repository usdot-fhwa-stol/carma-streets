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

# script to run tests, generate test-coverage, and store coverage reports in a place
# easily accessible to sonar. Test names should follow convention run<pluginName>Tests

cd /home/carma-streets
mkdir test_results
ls -a

cd /home/carma-streets/kafka_clients/build/
ls -a
./kafka_clients_test --gtest_output=xml:../../test_results/
cd /home/carma-streets/kafka_clients/
mkdir coverage
cd /home/carma-streets/
gcovr --sonarqube kafka_clients/coverage/coverage.xml -s -f kafka_clients/ -r .

cd /home/carma-streets/scheduling_service/build/
ls -a
./scheduling_service_test --gtest_output=xml:../../test_results/
cd /home/carma-streets/scheduling_service/
mkdir coverage
gcovr --sonarqube coverage/coverage.xml -s -r .

cd /home/carma-streets/streets_utils/streets_service_base/build/
ls -a
./streets_service_base_test --gtest_output=xml:../../../test_results/
cd /home/carma-streets/streets_utils/streets_service_base
mkdir coverage
cd /home/carma-streets/
gcovr --sonarqube streets_utils/streets_service_base/coverage/coverage.xml -s -f streets_utils/streets_service_base/ -r .
ls -l


cd /home/carma-streets/message_services/build/
ls -a
./message_services_test --gtest_output=xml:../../test_results/
cd /home/carma-streets/message_services/
ls -a
mkdir coverage
cd /home/carma-streets/
gcovr --sonarqube message_services/coverage/coverage.xml -s -f message_services/ -r .

cd /home/carma-streets/intersection_model/build/
ls -a
./intersection_model_test ---gtest_output=xml:../../test_results/
cd /home/carma-streets/intersection_model/
ls -a
mkdir coverage
cd /home/carma-streets/
gcovr --sonarqube intersection_model/coverage/coverage.xml -s -f intersection_model/ -r .