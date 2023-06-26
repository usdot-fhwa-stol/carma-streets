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
    data = read_json('phase_control_schedule_simple.json')
    i = 0
    for schedule in data["Schedule"]:
        #update start time with the current time + start time from the json file
        data["Schedule"][i]["commandStartTime"] = curr_time + data["Schedule"][i]["commandStartTime"]
        #update end time with the current time + end time from the json file
        data["Schedule"][i]["commandEndTime"]  = curr_time + data["Schedule"][i]["commandEndTime"]
        i+=1
    producer.send('phase_control_schedule', value=data)
    print(f'Sent a phase control schedule.{data}')
    producer.flush()

    # Send a clear schedule
    print(f'Sleep 10 seconds...')
    sleep(10) # sleep seconds
    data = read_json('phase_control_schedule_clear.json')
    producer.send('phase_control_schedule', value=data)
    print('Sent a phase control schedule clear.')
    producer.flush()
    
    # Send a new schedule to replace the clear schedule
    print(f'Sleep 5 seconds...')
    sleep(5) # sleep seconds
    curr_time = round(time.time()*1000)
    data = read_json('phase_control_schedule_simple.json')
    i = 0
    for schedule in data["Schedule"]:
        #update start time with the current time + start time from the json file
        data["Schedule"][i]["commandStartTime"] = curr_time + data["Schedule"][i]["commandStartTime"]
        #update end time with the current time + end time from the json file
        data["Schedule"][i]["commandEndTime"]  = curr_time + data["Schedule"][i]["commandEndTime"]
        i+=1
    producer.send('phase_control_schedule', value=data)
    print(f'Sent a phase control schedule.{data}')
    producer.flush()

    # Send a new schedule to replace the old schedule
    print(f'Sleep 5 seconds...')
    sleep(5) # sleep seconds
    curr_time = round(time.time()*1000)
    data = read_json('phase_control_schedule_simple.json')
    i = 0
    for schedule in data["Schedule"]:
        #update start time with the current time + start time from the json file
        data["Schedule"][i]["commandStartTime"] = curr_time + data["Schedule"][i]["commandStartTime"]
        #update end time with the current time + end time from the json file
        data["Schedule"][i]["commandEndTime"]  = curr_time + data["Schedule"][i]["commandEndTime"]
        i+=1
    producer.send('phase_control_schedule', value=data)
    print(f'Sent a phase control schedule.{data}')
    producer.flush()
