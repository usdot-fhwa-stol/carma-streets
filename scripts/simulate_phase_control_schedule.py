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
    curr_time = round(time.time()*1000)
    data = read_json('phase_control_schedule_simple.json')
    i = 0
    for schedule in data["Schedule"]:
        data["Schedule"][i]["commandStartTime"] = curr_time + data["Schedule"][i]["commandStartTime"]
        data["Schedule"][i]["commandEndTime"]  = curr_time + data["Schedule"][i]["commandEndTime"]
        i+=1
    producer.send('phase_control_schedule', value=data)
    print(f'Sent a phase control schedule.{data}')
    producer.flush()

    sleep(10)
    data = read_json('phase_control_schedule_clear.json')
    producer.send('phase_control_schedule', value=data)
    print('Sent a phase control schedule clear.')
    producer.flush()
