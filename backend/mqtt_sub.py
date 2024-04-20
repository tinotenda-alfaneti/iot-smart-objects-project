import paho.mqtt.client as mqtt
import requests
import json 

MQTT_SERVER = "192.168.195.170"
MQTT_PATH = "#"
 
# The callback when conected.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc)) 
    client.subscribe(MQTT_PATH)
 
# Callback when message received
def on_message(client, userdata, msg):
    # Convert the payload to a dictionary
    payload_dict = json.loads(msg.payload)
    
    if "temperature" in str(msg.topic):
        print(f"Temperature msg received from {msg.topic}.")
        print("Received: ", json.dumps(payload_dict))
    elif "humidity" in str(msg.topic):
        print(f"Humidity msg received from {msg.topic}.")
        print(json.dumps(payload_dict))
        

    # Send data to the API
    response = requests.post('http://localhost/iot/smartObjectsWeb/smartobjectapi.php', json={"smartObjectName":payload_dict['smartObjectName'], "sensorName":payload_dict['sensorName'], "sensorType":payload_dict['sensorType'], "currentReading":payload_dict['currentReading']})
    if response.status_code == 200:
        print("Error: ", response.content)
        print("Data sent successfully to the API")
    else:
        print("Error: ", response.status_code)
        print("Failed to send data to the API")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
client.on_connect = on_connect
client.on_message = on_message
 
client.connect(MQTT_SERVER, 1884, 60)

print("waiting for messages")
client.loop_forever()
