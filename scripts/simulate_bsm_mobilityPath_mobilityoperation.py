from os import read
from kafka import KafkaProducer
import time
from time import sleep
import json
import threading


def read_json(json_name):
    data = {}
    with open(json_name, "r") as json_file:
        data = json.load(json_file)
    return data


def produce_bsm():
    producer = KafkaProducer(bootstrap_servers=["127.0.0.1:9092"],
                             value_serializer=lambda x: json.dumps(x).encode('utf-8'))
    count = 100
    i = 0
    while(i < count):
        data = read_json('bsm_12_8.json')
        data["core_data"]["msg_count"] = str(i)
        data["core_data"]["speed"] = str(i)
        producer.send('v2xhub_bsm_in', value=data)
        print('Sent a bsm.')
        i += 1
        sleep(0.1)
        producer.flush()


def produce_operation():
    producer = KafkaProducer(bootstrap_servers=["127.0.0.1:9092"],
                             value_serializer=lambda x: json.dumps(x).encode('utf-8'))
    count = 100
    i = 0
    while(i < count):
        data = read_json('mobilityoperation_12_8.json')
        timestamp = int(time.time())*1000 + i*100
        data["metadata"]["timestamp"] = str(timestamp)
        strategy_params = "msg_count:" + \
            str(i) + ",access: 0,max_accel:1.500000,max_decel: -1.000000,react_time: 4.500000, min_gap: 5.000000, depart_pos: " \
            + str(i) + ",turn_direction:straight, sec_mark:37056"
        data["strategy_params"] = strategy_params
        producer.send('v2xhub_mobility_operation_in', value=data)
        print('Sent a mobilityoperation.')
        i += 1
        sleep(0.1)
        producer.flush()


def produce_mobility_path():
    producer = KafkaProducer(bootstrap_servers=["127.0.0.1:9092"],
                             value_serializer=lambda x: json.dumps(x).encode('utf-8'))
    i = 0
    count = 100
    while(i < count):
        data = read_json('mobilitypath_12_8.json')
        timestamp = int(time.time())*1000 + i*100
        data["metadata"]["timestamp"] = str(timestamp)
        producer.send('v2xhub_mobility_path_in', value=data)
        print('Sent a mobilitypath.')
        i += 1
        sleep(0.1)
        producer.flush()


if __name__ == "__main__":
    bsm_th = threading.Thread(target=produce_bsm)
    mobility_path_th = threading.Thread(target=produce_mobility_path)
    mobility_operation_th = threading.Thread(target=produce_operation)
    mobility_path_th.start()
    bsm_th.start()
    mobility_operation_th.start()
