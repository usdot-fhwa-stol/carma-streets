#!/bin/bash
#  Copyright (C) 2024 LEIDOS.
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
# script collects all service logs files and zips them into an archive and then removes the source files.
#######################################
# Prints the script's argument descriptions
# Globals:
#   None
# Arguments:
#   None
# Returns:
#   0
#######################################
set -e
function print_help() {
  command cat <<-HELP
options:
-h, --help                Show usage
--output, -o              Name of the resulting zip file container CARMA Streets Service logs
--clear, -c               Adding this flag will delete log directories after creating zip.

HELP
}
#######################################
# Prints a string to standard error
# Globals:
#   None
# Arguments:
#   String to print
# Returns:
#   0
#######################################
function err() {
  echo "$*" >&2
}
#######################################
# Prints the script's usage
# Globals:
#   None
# Arguments:
#   None
# Returns:
#   0
#######################################
function print_usage() {
  command cat <<-USAGE
usage: collect_service_logs.sh [-h | --help] [--output <zip_file_name>.zip]
                                     [-c | --clear]
USAGE
}
function main() {
 while :; do
    case "$1" in
      -h|--help)
        print_usage
        echo ""
        print_help
        exit 0
        ;;
      -c|--clear)
        echo "Clearing log directories after zip file is created!"
        clear=true
        ;;
      --output|-o)
        shift
        if [ "$#" -eq 0 ]; then
          err "error: option 'output' requires a value"
          print_usage
          exit 129
        fi
        output="$1"
        ;;
      *)
        if [[ "$1" == -* ]]; then
          err "unknown option '$1'"
          print_usage
          exit 129
        fi
        break
    esac
    shift
  done
  if [ "$output" ]; 
  then
    echo "Writing zip file ${output}.zip"
    zip -rq ./${output}.zip ./scheduling_service/logs ./intersection_model/logs ./tsc_client_service/logs ./message_services/logs ./signal_opt_service/logs ./sensor_data_sharing_service/logs
  else
    filename=$(date "+%FT%H%M%S")
    echo "Writing zip file ${filename}.zip"
    zip -rq ./${filename}.zip ./scheduling_service/logs ./intersection_model/logs ./tsc_client_service/logs ./message_services/logs ./signal_opt_service/logs ./sensor_data_sharing_service/logs
  fi
  if [ "$clear" = true ]; 
  then
    rm ./scheduling_service/logs/* ./intersection_model/logs/* ./tsc_client_service/logs/* ./message_services/logs/* ./signal_opt_service/logs/* ./sensor_data_sharing_service/logs/*
  fi
}
main "$@"