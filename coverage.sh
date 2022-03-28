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
gcovr --exclude=./build/CMakeFiles/kafka_clients.dir --exclude=./build/CMakeFiles/kafka_clients_test.dir/test -k -r .
mv *.gcov coverage

cd /home/carma-streets/streets_utils/streets_service_base/build/
ls -a
./streets_service_base_test --gtest_output=xml:../../../test_results/
cd /home/carma-streets/streets_utils/streets_service_base/
mkdir coverage
gcovr --exclude=./build/CMakeFiles/streets_service_base_test.dir/test --exclude=./build/CMakeFiles/streets_service_base.dir -k -r .
mv *.gcov coverage

cd /home/carma-streets/scheduling_service/build/
ls -a
./scheduling_service_test --gtest_output=xml:../../test_results/
cd /home/carma-streets/scheduling_service/
mkdir coverage
gcovr --exclude=./build/CMakeFiles/scheduling_service_test.dir/test --exclude=./build/CMakeFiles/scheduling_service.dir -k -r .
mv *.gcov coverage

cd /home/carma-streets/message_services/build/
ls -a
./message_services_test --gtest_output=xml:../../test_results/
cd /home/carma-streets/message_services/
ls -a
mkdir coverage
gcovr --exclude=./build/CMakeFiles/message_services_test.dir/test --exclude=./build/CMakeFiles/message_services.dir -k -r .
mv *.gcov coverage

cd /home/carma-streets/intersection_model/build/
ls -a
./intersection_model_test ---gtest_output=xml:../../test_results/
cd /home/carma-streets/intersection_model/
ls -a
mkdir coverage
gcovr --exclude=./test --exclude=./build/CMakeFiles/intersection_model_test.dir/test --exclude=./build/CMakeFiles/intersection_model.dir/ -k -r .
mv *.gcov coverage
