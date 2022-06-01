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
        data = read_json('map_1001_test.json')
        producer.send('v2xhub_map_msg_in', value=data)
        print('Sent a map.')
        i += 1
        producer.flush()
