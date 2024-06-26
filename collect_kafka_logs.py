#!/usr/bin/python3
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
import os
import time
import re
import signal
import subprocess
import argparse
from pathlib import Path
import shutil


## Python script to download and store kafka logs
# Opens the kafka docker container, then uses the example script kafka-console-consumer.sh to grab messages.
# Creates a local folder with given name
# For each specified topic (default is all):
#   downloads all messages between the given times (default is all)
#   stores these messages to a file named $(topic).log
# Then once all topics are downloaded, zips up the folder


def store_kafka_topic(topic, dir, timeout, start_time, end_time):

    timestamp_regex = 'CreateTime:(\d+)\t{'
    filename = f'{dir}/{topic}.log'

    # Command to get messages from specified topic, from https://usdot-carma.atlassian.net/wiki/spaces/CRMSRT/pages/2317549999/CARMA-Streets+Messaging
    command = f'docker exec kafka timeout {timeout} kafka-console-consumer.sh --topic {topic} --property print.timestamp=true --from-beginning --bootstrap-server localhost:9092'
    result = subprocess.check_output(f'docker exec kafka kafka-run-class.sh kafka.tools.GetOffsetShell --broker-list localhost:9092 --topic {topic} | awk -F \":\" \'{{sum += $3}} END {{print sum}}\'', shell=True)
    if result == b'\n':
        return 1
    else:
        num_msgs = int(result)

    line_count = 0
    with open(filename, 'w+') as outfile:
        # Popen returns a generator containing the command output line by line
        process = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True, universal_newlines=True, preexec_fn=os.setsid)
        while line_count < num_msgs:
            line = process.stdout.readline()
            if line == '' and process.poll() is not None:
                print(f'got {line_count} messages from {topic}')
                return 0

            # We have a valid line of output, find timestamp
            match = re.search(timestamp_regex, line)
            if not match:
                # Some topics have multiple lines. For those, store each line that doesn't have a timestamp,
                # and therefore is part of the 'current' message
                if topic == 'v2xhub_map_msg_in' or topic == 'v2xhub_bsm_in' \
                        or topic == 'v2xhub_mobility_operation_in' or topic == 'v2xhub_mobility_path_in':
                    outfile.write(line)
                else:
                    print(f'got {line_count} messages from {topic}')
                    print('no timestamp, exiting')
                    os.killpg(os.getpgid(process.pid), signal.SIGTERM)
                    return 1
            else:
                timestamp = int(match.group()[11:-1])

                # If we receive a message past the end time, we are done
                if timestamp > end_time:
                    os.killpg(os.getpgid(process.pid), signal.SIGTERM)
                    print(f'got {line_count} messages from {topic}')
                    return 0
                # If we receive a message between the start and end times, store it
                if timestamp > start_time:
                    outfile.write(line)
                    line_count += 1
        print(f'got all {num_msgs} expected messages from {topic}')
        os.killpg(os.getpgid(process.pid), signal.SIGTERM)
        return 0


def main():

    # Default list of topics, aka all topics from https://usdot-carma.atlassian.net/wiki/spaces/CRMSRT/pages/2317549999/CARMA-Streets+Messaging
    topics = ['v2xhub_scheduling_plan_sub' ,'v2xhub_bsm_in', 'v2xhub_mobility_operation_in', 'v2xhub_mobility_path_in',
              'vehicle_status_intent_output', 'v2xhub_map_msg_in', 'modified_spat', 'tsc_config_state', 'desired_phase_plan',
              'v2xhub_sdsm_sub', 'v2xhub_sim_sensor_detected_object', 'v2xhub_sdsm_tra', 'desire_phase_plan', 'time_sync']
    timeout = 5

    # Get arguments
    # The idea here was to give the user the bare minimum options, and make the default condition the most used.
    parser = argparse.ArgumentParser(description='Script to grab data from kafka')
    parser.add_argument('outdir', help='Folder name for the resulting folder logs are placed in', type=str)  # Required argument
    start_group = parser.add_mutually_exclusive_group()
    end_group = parser.add_mutually_exclusive_group()
    start_group.add_argument('--start_timestamp', help='Unix timestamp (seconds) for the first message to grab. Exclusive with start_hours_ago. ', type=int)
    end_group.add_argument('--end_timestamp', help='Unix timestamp (seconds) for the last message to grab. Exclusive with end_hours_ago. ', type=int)
    start_group.add_argument('--start_hours_ago', help='float hours before current time to grab first message. Exclusive with start_timestamp. ', type=float)
    end_group.add_argument('--end_hours_ago', help='float hours before current time to grab last message. Exclusive with start_timestamp. ', type=float)
    parser.add_argument('--topics', type=str, nargs='+', help='list of topics to grab data from')
    parser.add_argument('--timeout', type=float, help='timeout for receiving messages on a topic, default is 5 seconds')
    parser.add_argument('--zip', type=bool,help='bool flag. When set to true, folder is compressed into a zip file.', default=False)

    args = parser.parse_args()

    # Correct and validate outfile name
    if args.outdir[-4:] == '.zip':
        outdir = args.outdir[:-4]
    else:
        outdir = args.outdir
    # Check if output directory already exists
    if Path(Path.cwd()/outdir).is_dir():
        print(f'folder {outdir} exists, please remove or rename')
        return
    # Check if zip file already exists
    elif Path(Path.cwd()/f'{outdir}.zip').is_file():
        print(f'zip file {outdir}.zip exists, please remove or rename')
        return

    # Convert given time to unix timestamp (in ms)
    if args.start_timestamp:
        start_time = args.start_timestamp * 1000
    elif args.start_hours_ago:
        start_time = int(time.time() - 3600 * args.start_hours_ago) * 1000
    else:
        start_time = 0

    if args.end_timestamp:
        end_time = args.end_timestamp * 1000
    elif args.end_hours_ago:
        end_time = int(time.time() - 3600 * args.end_hours_ago) * 1000
    else:
        end_time = int(time.time()) * 1000

    if args.topics:
        topics = args.topics
    if args.timeout:
        timeout = args.timeout
    Path(outdir).mkdir(exist_ok=False)
    for topic in topics:
        ret = store_kafka_topic(topic, outdir, timeout, start_time, end_time)
        if ret != 0:
            print(f'received error on topic {topic}, going to next topic')

    print('Available logs collected')
    if args.zip:
        print('Zipping and removing the temporary folder')
        shutil.make_archive(base_name=outdir, format='zip', root_dir=outdir)
        shutil.rmtree(path=outdir)


if __name__ == "__main__":
    main()
