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
    obj = time.gmtime(0)
    epoch = time.asctime(obj)
    print("The epoch is:",epoch)

    # Send a new schedule
    curr_time = round(time.time()*1000)
    while True :
        data = read_json('detected_pedestrian.json')
        producer.send('detections', value=data)
        print(f'Sent a detection.{data}')
        producer.flush()
        data = read_json('detected_cyclist.json')
        producer.send('detections', value=data)
        print(f'Sent a detection.{data}')
        data = read_json('detected_truck.json')
        producer.send('detections', value=data)
        print(f'Sent a detection.{data}')
        producer.flush()
        time.sleep(0.1)


    