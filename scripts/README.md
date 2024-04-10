# Testing Scripts
## Introduction
The python scripts and json files in this directory are used for mocking message communication to provide an environment in which CARMA Streets Service functionality can be integration tested in isolation. Directions for using the scripts should be included below under the appropriate heading.

## Prerequisites
Please add any script dependencies into requirements.txt so that they can be easily installed using 
```sh
pip3 install -r requirements.txt
```
## simulate_detections.py
### Description
This script is used to mock Detected Object message sent from sensor integrated with CARMA Streets. Currently this only includes CARLA Lidar sensor integrated under the Vunerable Road User Use Case 1 project. It will currently read `detected_cyclist.json`, `detected_pedestrian.json`, and `detected_truck.json` and publish those messages at 10 hz to the detections kafka topic. This script is used to provide input to the **Sensor Data Sharing Service** to generate J2735 SDSMs. To add new detections, create a new json file and update the script to send its content on a given interval. For json format of the [Detected Object Message](../streets_utils/streets_messages/DetectedObjectsMessage.md). 
## simulate_vehicle_status_intent_pub_uc1.py
Instructions for sending sample status and intent messages for UC1 scheduling logic:
    
    - In the simulate_vehicle_status_intent_pub_uc1.py, change the input of the read_json() method on line 23 to 
        vehicle_status_intent_test_uc1.json (or any other json message that you have prepared).
    - The current sample status and intent message (vehicle_status_intent_test_uc1.json) includes 6 updates for 
        3 vehicles.
    - In the manifest.json, set the vehicle_count to the number of vehicles in one set of updates that has the same
        timestamp (3 in vehicle_status_intent_test_uc1.json), and set the sleeping_time to the desired time gap 
        between two set of updates.
    - RUN python3 simulate_vehicle_status_intent_pub_uc1.py
    
    
    * Note: The current timestamp and the future timestamps of the updates will updated automatically before the message is published.
    * Note: One of the limitation of the python script is that the number of vehicles in each set of updates shall be the same.
        Otherwise, the time gap between two set of updates will not be correctly applied. 
    
## simulate_vehicle_status_intent_pub_uc3.py
Instructions for sending sample status and intent messages for UC3 scheduling logic:
    
    - In the simulate_vehicle_status_intent_pub_uc3.py, change the input of the read_json() method on line 23 to 
        vehicle_status_intent_test_uc3.json (or any other json message that you have prepared).
    - The current sample status and intent message (vehicle_status_intent_test_uc3.json) includes 5 updates for 
        9 vehicles.
    - In the manifest.json, set the vehicle_count to the number of vehicles in one set of updates that has the same
        timestamp (9 in vehicle_status_intent_test_uc3.json), and set the sleeping_time to the desired time gap 
        between two set of updates.
    - RUN python3 simulate_vehicle_status_intent_pub_uc3.py

    
    * Note: The current timestamp and the future timestamps of the updates will updated automatically before the message is published.
    * Note: One of the limitation of the python script is that the number of vehicles in each set of updates shall be the same.
        Otherwise, the time gap between two set of updates will not be correctly applied. 