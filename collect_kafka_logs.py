import os
import time
import re
import signal
import subprocess
import argparse


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
    # topic desire_phase_plan is in the list from confluence, but doesn't exist in kafka as of 2022/12/05
    topics = ['v2xhub_scheduling_plan_sub' ,'v2xhub_bsm_in', 'v2xhub_mobility_operation_in', 'v2xhub_mobility_path_in',
              'vehicle_status_intent_output', 'v2xhub_map_msg_in', 'modified_spat', 'tsc_config_state', 'desired_phase_plan',
              'v2xhub_sdsm_sub', 'v2xhub_sim_sensor_detected_object', 'v2xhub_sdsm_tra']
    timeout = 5

    # Get arguments
    # The idea here was to give the user the bare minimum options, and make the default condition the most used.
    parser = argparse.ArgumentParser(description='Script to grab data from kafka')
    parser.add_argument('outfile', help='Filename for the resulting zip file', type=str)  # Required argument
    start_group = parser.add_mutually_exclusive_group()
    end_group = parser.add_mutually_exclusive_group()
    start_group.add_argument('--start_timestamp', help='Unix timestamp (seconds) for the first message to grab. Exclusive with start_hours_ago. ', type=int)
    end_group.add_argument('--end_timestamp', help='Unix timestamp (seconds) for the last message to grab. Exclusive with end_hours_ago. ', type=int)
    start_group.add_argument('--start_hours_ago', help='float hours before current time to grab first message. Exclusive with start_timestamp. ', type=float)
    end_group.add_argument('--end_hours_ago', help='float hours before current time to grab last message. Exclusive with start_timestamp. ', type=float)
    parser.add_argument('--topics', type=str, nargs='+', help=f'list of topics to grab data from')
    parser.add_argument('--timeout', type=float, help=f'timeout for receiving messages on a topic, default is 5 seconds')

    args = parser.parse_args()

    # Correct and validate outfile name
    if args.outfile[-4:] == '.zip':
        outfile = args.outfile[:-4]
    else:
        outfile = args.outfile

    if os.path.isdir(f'{os.getcwd()}/{outfile}'):
        print(f'folder {outfile} exists, please remove or rename')
        return
    elif os.path.isfile(f'{os.getcwd()}/{outfile}.zip'):
        print(f'zip file {outfile}.zip exists, please remove or rename')
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

    os.system(f'mkdir {outfile}')
    for topic in topics:
        ret = store_kafka_topic(topic, outfile, timeout, start_time, end_time)
        if ret != 0:
            print('received error, stopping execution')
            return

    print('Available logs collected, zipping and removing the temporary folder')
    os.system(f'zip -r {outfile}.zip {outfile}')
    os.system(f'rm -r {outfile}')


if __name__ == "__main__":
    main()
