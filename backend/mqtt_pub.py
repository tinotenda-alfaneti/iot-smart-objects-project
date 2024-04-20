import paho.mqtt.publish as publish
import time 
MQTT_SERVER = "192.168.195.170"
#MQTT_SERVER = "172.20.10.2"
MQTT_PATH = "command"

 
while True: 
	publish.single(MQTT_PATH, "0", hostname=MQTT_SERVER, port=1884)
	time.sleep(30)
	publish.single(MQTT_PATH, "1", hostname=MQTT_SERVER, port=1884)

#works