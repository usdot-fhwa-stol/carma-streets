from os import read
from kafka import KafkaProducer
import time
from time import sleep
import json


def read_json(json_name):
    data = {}
    with open(json_name, "r") as json_file:
        data = json.load(json_file)
    return data


if __name__ == "__main__":
    producer = KafkaProducer(bootstrap_servers=["127.0.0.1:9092"],
                             value_serializer=lambda x: json.dumps(x).encode('utf-8'))
    count = 10
    i = 0
    while(i < count):
        data = read_json('mobilityoperation.json')
        strategy_params = "msg_count:" + \
            str(i) + ",access: 0,max_accel:1.500000,max_decel: -1.000000,react_time: 4.500000, min_gap: 5.000000, depart_pos: " \
            + str(i) + ", turn_direction:straight"
        data["strategy_params"] = strategy_params
        timestamp = int(data["metadata"]["timestamp"]) + i
        data["metadata"]["timestamp"] = str(timestamp)
        producer.send('v2xhub_mobility_operation_in', value=data)
        print('Sent a mobilityoperation.')
        i += 1
        producer.flush()
