
Requirements:
    pip3 install -r requirements.txt


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