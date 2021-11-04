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
    i = 0
    count = 10
    while(i < count):
        data = read_json('mobilitypath.json')
        timestamp = int(data["metadata"]["timestamp"]) + i
        data["metadata"]["timestamp"] = str(timestamp)
        producer.send('v2xhub_mobility_path_in', value=data)
        print('Sent a mobilitypath.')
        i += 1
        producer.flush()
