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
    
    manifest_json = read_json('manifest.json')  
    sleeping_time = manifest_json["sleeping_time"]
    vehicle_count = manifest_json["vehicle_count"]

    data = read_json('vehicle_status_intent_test_access.json')
    indicator = 0
    for key, items in data.items():        
         for item in items:             
            item['metadata']['timestamp'] = int(time.time() * 1000)
            for i in range(0, len(item['payload']['est_paths'])):
                item['payload']['est_paths'][i]['ts'] = int(item['metadata']['timestamp'] + ((i + 1) * 200))

            producer.send('vehicle_status_intent_output', value=item)
            producer.flush()
            #print(item)
            print('Sent a vehicle status and intent.')
            indicator += 1
            if (indicator == vehicle_count):
                indicator = 0
                sleep(sleeping_time) # produce vehicle status and intent every one second
            else:
                sleep(0.1)
