import json
import asyncio
import sys
from aiokafka import AIOKafkaConsumer
from aiokafka import AIOKafkaProducer

#The SimulationHelper class is used to create a Kafka Consumer and Producer. When the appropriate time_sync message is received
#by the Consumer, the Producer sends a desired phase plan to the desired phase plan topic.
class SimulationHelper():
    #Initialize the SimulationHelper with the input Kafka IP address and port
    def __init__(self, kafkaIP, kafkaPort):
        self.kafka_ip = kafkaIP
        self.kafka_port = kafkaPort
        print("Initialized SimulationHelper")

    #Async function that creates a KafkaConsumer and adds all available topics to a list
    async def kafka_client(self):
        self.kafka_consumer = AIOKafkaConsumer(
            bootstrap_servers=[self.kafka_ip+":"+self.kafka_port],
            auto_offset_reset="latest",
            enable_auto_commit=True,
            group_id=None,
            value_deserializer=lambda x: json.loads(x.decode('utf-8')))

        await self.kafka_consumer.start()

        # Get all kafka topics
        self.streets_topics = []
        for topic in await self.kafka_consumer.topics():
            self.streets_topics.append(topic)
        print("In createAsyncKafkaConsumer: All available Kafka topics = " + str(self.streets_topics))
        self.kafka_consumer.subscribe(topics= {"time_sync"})
        await self.kafka_read()

    #Async function that creates a Kafka Producer and listens for time_sync messages. When a certain time_sync message is received
    #by the Kafka Consumer, the Kafka Producer will send a desired phase plan to the desired phase plan topic
    async def kafka_read(self):

        ## Create Kafka producer and send DPP message over DPP topic
        self.kafka_producer = AIOKafkaProducer(
            bootstrap_servers=[self.kafka_ip+":"+self.kafka_port],
            enable_idempotence=True) #When enable_idempotence set to True, the producer will ensure that exactly one copy of each message is written in the stream. 
            #If False, producer retries due to broker failures, etc., may write duplicates of the retried message in the stream.
        
        await self.kafka_producer.start()
        print("Kafka producer created")

        #Read in messages from Kafka using Consumer
        try:
            async for consumed_msg in self.kafka_consumer:
                print("message consumed {}", consumed_msg.value)
                #Check the time_sync messages
                message = {}
                message["payload"] = consumed_msg.value
                timestep = message["payload"]["timestep"]
                print("Timestep {}", timestep)
                if  timestep == 78300:
                    desired_phase_plan_json = {
                        "timestamp" : timestep,
                        "desired_phase_plan": [
                            {
                                "signal_groups": [4,8],
                                "start_time": 78300,
                                "end_time": 98300
                            },
                            {
                                "signal_groups": [2, 6],
                                "start_time": 102300,
                                "end_time": 132300
                            }
                        ]
                    }
                    ### TODO Listen for time synchronization message equal to some value and publish to dpp topic
                    try:
                        await self.kafka_producer.send_and_wait("desired_phase_plan", json.dumps(desired_phase_plan_json).encode('utf-8') )
                    finally:
                        print("Desired Phase Plan sent successfully")
                        await self.kafka_producer.stop()        
        except:
            print(" In kafka_read: Error reading kafka traffic")
       

def main():
    if len(sys.argv) < 3:
        print('Run with: "python3 simulation_dpp_sender.py kafkaIP kafkaPort"')

        exit()
    else:   
        #Read in user input  
        kafkaIP = sys.argv[1]
        kafkaPort = sys.argv[2]

        #Create SimulationHelper object
        simulationHelper = SimulationHelper(kafkaIP, kafkaPort)

        #Create asyncio loop
        loop = asyncio.get_event_loop()

        #Create individual asyncio task to create kafka client
        tasks = [
            loop.create_task(simulationHelper.kafka_client())
        ]

        #Run asyncio until tasks are complete
        loop.run_forever()
        loop.run_until_complete(asyncio.wait(tasks))
        loop.close()

if __name__ == '__main__':
    main()