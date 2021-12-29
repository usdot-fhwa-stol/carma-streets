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

cd /home/carma-streets/kafka_clients
ls -a
mkdir coverage
gcovr -k -r .
mv *.gcov coverage

cd /home/carma-streets/streets_utils/streets_api/intersection_client_api
ls -a
mkdir coverage
gcovr -k -r .
mv *.gcov coverage

cd /home/carma-streets/scheduling_service
ls -a
mkdir coverage
gcovr -k -r .
mv *.gcov coverage

cd /home/carma-streets/message_services
ls -a
mkdir coverage
gcovr -k -r .
mv *.gcov coverage

cd /home/carma-streets/intersection_model
ls -a
mkdir coverage
gcovr -k -r .
mv *.gcov coverage
