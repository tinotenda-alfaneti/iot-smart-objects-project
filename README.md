# Smart Objects Project

### Introduction
This project focuses on creating smart devices that monitor environmental parameters such as temperature, humidity, and light intensity in indoor and outdoor locations simultaneously. The devices collect data, display real-time readings, store data locally, and transmit it to a database using `HTTP POST` or `MQTT` protocols. 

### Project Components
1. Smart Devices - Two smart devices are created, each equipped with sensors to monitor temperature, humidity, and light intensity. The devices run the same code but are placed in different locations - one indoors and one outdoors.
2. Data Collection and Transmission - Data is collected every 6 seconds for temperature, every 3 seconds for humidity, and every 6 seconds for light intensity. Readings are displayed on a local LCD screen and saved to flash memory once every minute.
Sensor data is transmitted to a database using either HTTP POST or MQTT protocols, based on user selection.
A separate script fetches published readings from MQTT and stores them in the database.
3. Database Design - The database stores sensor data along with a timestamp and the name/location of the smart node.
The design allows for adding additional smart nodes without modifying the database structure.
4. Web Interface - A web server provides pages for browsing historical data, adding new smart nodes, and viewing data submitted by specific smart objects.
The smart object presents a web portal for user interaction and configuration.
5. Over-the-Air (OTA) Update -
OTA updates allow for updating the smart object's code, tested by changing the heartbeat rate.

### Running the Project
To run the project, follow these steps:

1. Assemble the circuit with an ESP32 and connect the pins as specified in the .ino file in the Arduino code.
2. Run the smart objects in indoor and outdoor locations.
3. Start the backend code and MQTT script.
4. Access the web interface to interact with the smart objects and configure settings.

### Authors

[Tinotenda R. Alfaneti](https://github.com/tinotenda-alfaneti)