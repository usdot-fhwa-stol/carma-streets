from os import read
from kafka import KafkaProducer
from time import sleep
import json

def read_json():
    data = {}
    with open('vehicle_status_intent.json', "r") as json_file:
        data = json.load(json_file)
    return data

if __name__ == "__main__": 
    producer = KafkaProducer(bootstrap_servers=["127.0.0.1:9092"],
                            value_serializer=lambda x: json.dumps(x).encode('utf-8'))
    data = read_json()    
    for key, items in data.items():        
         for item in items:             
            producer.send('examples', value=item)
            print('Sent a vehicle status and intent.')
            sleep(1) # produce vehicle status and intent every one second