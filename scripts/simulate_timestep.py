from os import read
from kafka import KafkaProducer
import time
from time import sleep
import json




if __name__ == "__main__":
    producer = KafkaProducer(bootstrap_servers=["127.0.0.1:9092"],
                             value_serializer=lambda x: json.dumps(x).encode('utf-8'))
    count = 10
    i = 0
    start = 2000
    while(i < count):
        time_sync_json = {
        "seq":1, 
        "timestep":int(start)
        }
        producer.send('time_sync', value=time_sync_json)
        print('Sent a timesync.')
        i += 1
        start += 100
        producer.flush()
