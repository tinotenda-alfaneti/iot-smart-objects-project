/* This code will be run by smart object */

//------imports
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoOTA.h>

#include "config.h"
#include "index.h"
#include "history.h"

//------ imports of website
#include "ESPAsyncWebServer.h"

//------ saving to flush
#include <Preferences.h>
#include "SPIFFS.h"


Preferences preferences;

//------define pins and constants
#define DHT11_PIN 19
#define LDR_PIN 34
#define AC_FAN 15
#define MSG_BUFFER_SIZE  (50)
int LCD_COL = 16;
int LCD_ROW = 2;
unsigned long last_temp = 0;
unsigned long last_humi = 0;
unsigned long last_ldr = 0;
unsigned long last_save = 0;
unsigned long last_update = 0;
unsigned long last_heartbeat = 0;
int display_mode = 0;

//------mqtt constants
const char* mqtt_server = "192.168.195.170";
const int mqtt_port =1884;
const char* command_topic="command";
String base_topic="smartOutside/";
String temp_topic= base_topic + "temperature";
String humi_topic= base_topic + "humidity";


//------initialize 
DHT dht11(DHT11_PIN, DHT22);
LiquidCrystal_I2C lcd(0x27, LCD_COL, LCD_ROW);
WiFiClient espClient;
PubSubClient client(espClient);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


//------wifi settings
const char* ssid = "MuAtarist";
const char* password = "123forIV6ix";

const char* serverName = "http://192.168.195.170/iot/smartObjectsWeb/smartobjectapi.php";

// ------ mqtt settings
String clientId = "ESP32Client-" + String(random(0xffff), HEX);


//------ config variables
String smartObjectName = "smartieOut";
String toRemove = "ssid";
String triggerTemperature = "20";
int communicationMode = 1;
int fanControl = 1;

//dht11
float humi;
float tempC;
int LDR = 0;

//AC Fan
String fanMode = "AUTO";

//----connect to wifi
void connectWifi() {
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("-");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP()); // this should be on the LCD
}

//-----connect to MQTT
void reconnectMqtt() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect(clientId.c_str())){//, mqtt_username, mqtt_password)) {
      Serial.println("MQTT connected");

      client.subscribe(command_topic);   // subscribe the topics here

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 0.5 seconds");   // Wait 0.5 seconds before retrying
      delay(500);
    }
  }
}

//update readings values
void fetchCurrentReading(AsyncWebServerRequest *request) {
  humi  = dht11.readHumidity();
  tempC = dht11.readTemperature();
  LDR = analogRead(LDR_PIN);
  String tempStr = String(tempC);
  String humiStr = String(humi);
  String ldrStr = String(LDR);
  //only this is sent
  request->send(200, "text/plain", tempStr + "," + humiStr + "," + ldrStr);
}



void setup() {
  
    Serial.begin(115200);

    analogReadResolution(12);

    dht11.begin(); // initialize the DHT11 sensor

    // Initialize SPIFFS
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }

    last_update = millis();
    preferences.begin("config", false); // Open preferences with "config" namespace, read-only access
    // Load saved values or defaults if not yet saved
    smartObjectName = preferences.getString("smartObjectName", "smartieOut");
    triggerTemperature = preferences.getString("triggerTemperature", "20");
    communicationMode = preferences.getInt("communicationMode", 1); //1 -> HTTP, 0 -> MQTT
    fanControl = preferences.getInt("fanControl", 1); // 0 -> AUTO, 1 -> MANUAL

    if (fanControl == 1) {
      fanMode = "MANUAL"; 
    } else {
      
      fanMode = "AUTO";  
    }
    preferences.end(); // Close preferences

    //setup topics
    base_topic = smartObjectName + "/";
    temp_topic= base_topic + "temperature";
    humi_topic= base_topic + "humidity";

    pinMode(BUILTIN_LED, OUTPUT);
    pinMode(LDR_PIN, INPUT);
    pinMode(AC_FAN, OUTPUT); 

    connectWifi();

    //mqtt setup
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    reconnectMqtt();

    //OTA
    ArduinoOTA.setHostname(smartObjectName.c_str());
    ArduinoOTA.setPassword("12345678");
    ArduinoOTA.begin();   // initialise ArduinoOTA
    
    // Start server
    server.begin();


    // initialize LCD
    lcd.init();
    // turn on LCD backlight                      
    lcd.backlight();

  // Route for root / web page
   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", indexPage);
  });
  
  server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", configPage);
  });

  server.on("/history", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", historyPage);
  });


    // Route for starting the fan
    server.on("/startFan", HTTP_GET, [](AsyncWebServerRequest *request) {
        startFan(request);
    });

    // Route for stopping the fan
    server.on("/stopFan", HTTP_GET, [](AsyncWebServerRequest *request) {
        stopFan(request);
    });

  server.on("/updateConfig", HTTP_POST, configHandler);
  server.on("/getSensorData", HTTP_GET, loadDataHandler);
  server.on("/fetchConfig", HTTP_GET, fetchConfigHandler);
  server.on("/currentReading", HTTP_GET, fetchCurrentReading);

}

void loop() {
  ArduinoOTA.handle();
  if (!client.connected()) reconnectMqtt();
  
  client.loop();

  lcdScreenControl();

  if (communicationMode == 0) {

    publishLightIntensity();

    publishHumidity();

    publishTemperature();
    
  } else {
    
    postHumidity();
    
    postTemperature();
    
    postLightIntensity();
    
  }

  saveToFile();
  
  turnAcFan();

  heartBeat();
 

}


void turnAcFan() {

  if (fanMode == "AUTO") {

    tempC = dht11.readTemperature();
    if (tempC > triggerTemperature.toFloat()) {
      digitalWrite(AC_FAN, HIGH);
    } else {
      digitalWrite(AC_FAN, LOW);  
    }
     
  }
}

//------- write to file
void writeToSPIFFS(String reading) {
  // Write to file
  Serial.println("Writing to temperature reading");
  File file = SPIFFS.open("/readings.txt", FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  Serial.println(reading);
  file.println(reading);
  file.close();  
  
}

// saving readings to SPIFFS
void saveToFile() {

  if (millis() - last_save > 60000) {

    last_save = millis();

    String readings = readHumi() + "\n" + readTemp() + "\n" + readLdr();
    Serial.println(readings);
    writeToSPIFFS(readings);
  }
}

//update readings values
void lcdScreenControl() {
  humi  = dht11.readHumidity();
  tempC = dht11.readTemperature();
  LDR = analogRead(LDR_PIN);

  // Calculate the time difference since the last update
  unsigned long timeDiff = millis() - last_update;

  // Rotate the display every 6 seconds
  if (timeDiff > 3000) {
    display_mode = (display_mode + 1) % 2; // Rotate between 0, 1, and 2
    last_update = millis(); // Update the last update time
    lcd.clear(); // Clear the display for new content
  }

  // Display based on the current mode
  switch (display_mode) {
    case 0: // Display temperature and LDR
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.setCursor(2, 0); 
      lcd.print(tempC);
      lcd.setCursor(7, 0);
      lcd.print(" L:");
      lcd.setCursor(10, 0); 
      lcd.print(LDR);
      if (WiFi.status()== WL_CONNECTED) {
        lcd.setCursor(0, 1);
        lcd.print(WiFi.localIP());
      }
      break;
    case 1: // Display humidity
      lcd.setCursor(0, 0);
      lcd.print("Humid:");
      lcd.setCursor(6, 0); 
      lcd.print(humi);
      if (WiFi.status()== WL_CONNECTED) {
        lcd.setCursor(0, 1);
        lcd.print(WiFi.localIP());
      }
      break;
  }
}



//------heartbeat for smart object
void heartBeat() {
  //heartbeat
  if (millis() - last_heartbeat > 2000) {
    last_heartbeat = millis();
    digitalWrite(BUILTIN_LED, HIGH);
  } else {
    digitalWrite(BUILTIN_LED, LOW);
  }
  
 }

// read temperature value to JSON
String readTemp() {

  tempC  = dht11.readTemperature();

  String sensorName = "DHT11";

  // Construct JSON string with dynamic variables
  String jsonString = "{\"smartObjectName\":\"" + smartObjectName +
                "\",\"sensorName\":\"" + sensorName +
                "\",\"currentReading\":\"" + String(tempC) +
                "\",\"sensorType\":\"" + "Temperature" + "\"}";
 return jsonString; 

}

// read humidity value to JSON
String readHumi() {

    humi  = dht11.readHumidity();
    String sensorName = "DHT11";

    // Construct JSON string with dynamic variables
    String jsonString = "{\"smartObjectName\":\"" + smartObjectName +
                  "\",\"sensorName\":\"" + sensorName +
                  "\",\"currentReading\":\"" + String(humi) +
                  "\",\"sensorType\":\"" + "Humidity" + "\"}";
    return jsonString; 
  
  
}

// read LDR value to JSON
String readLdr() {

    LDR = analogRead(LDR_PIN);

    String sensorName = "LDR";

    // Construct JSON string with dynamic variables
    String jsonString = "{\"smartObjectName\":\"" + smartObjectName +
                  "\",\"sensorName\":\"" + sensorName +
                  "\",\"currentReading\":\"" + String(LDR) +
                  "\",\"sensorType\":\"" + "Light Intensity" + "\"}";

    return jsonString;
}

//------ publishing temperature after 6 seconds
void publishTemperature() {
  
  //publish temp sensor values every 6 sec
  if (millis() - last_temp > 6000) {

    Serial.println("MQTT Publishing Mode");

    last_temp = millis();
    
    String jsonString = readTemp();
  
    // Publish the JSON payload to the MQTT topic
    publishMessage(temp_topic.c_str(),jsonString,true);
    
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP()); // this should be on the LCD

  } 
 }

//------ publishing temperature after 6 seconds
void publishLightIntensity() {
  
  //publish temp sensor values every 6 sec
  if (millis() - last_ldr > 6000) {

    Serial.println("MQTT Publishing Mode");

    last_ldr = millis();

    String jsonString = readLdr();
  
    // Publish the JSON payload to the MQTT topic
    publishMessage(temp_topic.c_str(),jsonString,true);
    
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP()); // this should be on the LCD

  } 
 }

//------ publishing humidity values after every 3 secs 
 void publishHumidity() {

    //publish humi sensor values every 3 sec
    if (millis() - last_humi > 3000) {
  
      last_humi = millis();

      String jsonString = readHumi(); 
      // Publish the JSON payload to the MQTT topic
      publishMessage(humi_topic.c_str(),jsonString,true);
    }  
  
  }


//----- call back function for subscribing
void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
  
  Serial.println("Message arrived ["+String(topic)+"]"+incommingMessage);
  
  //--- check the incomming message
    if( strcmp(topic,command_topic) == 0){
     if (incommingMessage.equals("1")) digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on 
     else digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off 
  }
}

//------ publising as string
void publishMessage(const char* topic, String payload , boolean retained){
  
  if (client.publish(topic, payload.c_str(), true))
      Serial.println("Message publised ["+String(topic)+"]: "+payload);
}


// Route handler function 
void loadDataHandler(AsyncWebServerRequest *request) {
    // Send a response indicating successful update

    String payload = readingSPIFFStoArray();

    request->send(200, "text/plain", payload);

}


void startFan(AsyncWebServerRequest *request) {

  if(fanMode == "MANUAL") {
    digitalWrite(AC_FAN, HIGH); // Turn on the fan
    request->send(200, "text/plain", "Fan started");
  } else {
    request->send(500, "text/plain", "Auto mode ON"); 
  }


}

// Function to stop the fan
void stopFan(AsyncWebServerRequest *request) {
   if(fanMode == "MANUAL") {
    digitalWrite(AC_FAN, LOW); // Turn off the fan
    request->send(200, "text/plain", "Fan stopped");
  } else {
    request->send(500, "text/plain", "Auto mode ON"); 
  }
}

//------- handle config update

// Route handler function 
void configHandler(AsyncWebServerRequest *request) {
  // Check if this is a POST request
  if (request->method() == HTTP_POST) {
    // Parse the POST data
    smartObjectName = request->getParam("smartObjectName", true)->value();
    toRemove = request->getParam("ssid", true)->value();
    triggerTemperature = request->getParam("triggerTemperature", true)->value();
    String tmp_com = request->hasParam("communicationMode", true) ? request->getParam("communicationMode", true)->value() : "";
    String tmp_fan = request->hasParam("fanControl", true) ? request->getParam("fanControl", true)->value() : "";
    communicationMode = tmp_com == "on" ? 1 : 0;
    fanControl = tmp_fan == "on" ? 1 : 0;

    if (fanControl == 1) {
      fanMode = "MANUAL"; 
    } else {
      
      fanMode = "AUTO";  
    }

    // Update configurations as needed
    Serial.println("Object name: " + smartObjectName);

    savePreferences();

    updateConfigsInDb();

    // Send a response indicating successful update
    request->send(200, "text/plain", "Configuration updated successfully");
  } else {
    // If request method is not POST
    request->send(405, "text/plain", "Method Not Allowed");
  }
}

// add the config to DB
void updateConfigsInDb() {

  if(WiFi.status()== WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;
  
        // Define the URL for the PUT request
        String url = String(serverName) + "/updateName";
        // Set the request method to PUT
        http.begin(url);
        http.addHeader("Content-Type", "application/json");
        String jsonString = "{\"smartObjectName\":\"" + smartObjectName + "\"}"; 
        
        // Send the PUT request
        int httpResponseCode = http.sendRequest("PUT", jsonString);

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            // Handle the response
        } else {
            Serial.print("Error on HTTP request: ");
            Serial.println(httpResponseCode);
        }
        // End the request
        http.end();
  }
}

// ------ handler to fetch configs
void fetchConfigHandler(AsyncWebServerRequest *request) {
    String jsonResponse;

    // Populate the JSON response with the current configuration values
    jsonResponse += "{\"smartObjectName\":\"";
    jsonResponse += smartObjectName;
    jsonResponse += "\",\"ssid\":\"";
    jsonResponse += toRemove;
    jsonResponse += "\",\"triggerTemperature\":";
    jsonResponse += triggerTemperature;
    jsonResponse += ",\"communicationMode\":";
    jsonResponse += communicationMode ? "true" : "false";
    jsonResponse += ",\"fanControl\":";
    jsonResponse += fanControl ? "true" : "false";
    jsonResponse += "}";

    // Set the content type header to application/json
    request->send(200, "application/json", jsonResponse);
}

// ------ saving to flush
void savePreferences() {
    preferences.begin("config", false); // Open preferences with "config" namespace, read-write access
    preferences.putString("smartObjectName", smartObjectName);
    preferences.putString("triggerTemperature", triggerTemperature);
    preferences.putInt("communicationMode", communicationMode);
    preferences.putInt("fanControl", fanControl);
    preferences.end(); // Close preferences
}

//----- post using http

void postHumidity() {

    //post humi sensor values every 3 sec
    if (millis() - last_humi > 3000) {
  
      last_humi = millis();
      //Check WiFi connection status
      if(WiFi.status()== WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;
  
        
        http.begin(client, serverName);
        String jsonString = readHumi();
        Serial.println(jsonString);
        int httpResponseCode = http.POST(jsonString.c_str());
       
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
          
        // Free resources
        http.end();
      }
      else {
        Serial.println("WiFi Disconnected");
      }   
      
    }  
}

String readingSPIFFStoArray() {
  String jsonArray = "["; // Start of JSON array

  File file = SPIFFS.open("/readings.txt", FILE_READ); // Open file for reading
  if (!file) {
    Serial.println("Failed to open file for reading");
    return jsonArray; // Return empty array if file opening fails
  }

  // Read each line from the file and add it to the JSON array
  boolean isFirstReading = true;
  while (file.available()) {
    String reading = file.readStringUntil('\n'); // Read one line
    reading.trim(); // Remove leading and trailing whitespaces
    if (!reading.isEmpty()) { // Check if the line is not empty
      if (!isFirstReading) {
        jsonArray += ","; // Add comma if it's not the first reading
      }
      jsonArray += reading; // Add reading to the JSON array
      isFirstReading = false;
    }
  }
  file.close(); // Close the file

  jsonArray += "]"; // End of JSON array

  return jsonArray;
}


//temp
void postTemperature() {

    //post temp sensor values every 6 sec
    if (millis() - last_temp > 6000) {

      Serial.println("HTTP Posting Mode");
  
      last_temp = millis();
      //Check WiFi connection status
      if(WiFi.status()== WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;
 
        http.begin(client, serverName);
  
        // For HTTP request with a content type: application/json:
        http.addHeader("Content-Type", "application/json");
    
        String jsonString = readTemp();
                      
        Serial.println(jsonString);
        int httpResponseCode = http.POST(jsonString.c_str());
       
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
          
        // Free resources
        http.end();
      }
      else {
        Serial.println("WiFi Disconnected");
      }   
      
    }  
}


//light intensity
void postLightIntensity() {

    //post temp sensor values every 6 sec
    if (millis() - last_ldr > 6000) {

      Serial.println("HTTP Posting Mode");
  
      last_ldr = millis();
      //Check WiFi connection status
      if(WiFi.status()== WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;
  
        http.begin(client, serverName);
  
        // For HTTP request with a content type: application/json:
        http.addHeader("Content-Type", "application/json");
    
        // Construct JSON string with dynamic variables
        String jsonString = readLdr();
                      
        Serial.println(jsonString);
        int httpResponseCode = http.POST(jsonString.c_str());
       
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
          
        // Free resources
        http.end();
      }
      else {
        Serial.println("WiFi Disconnected");
      }   
      
    }  
}
