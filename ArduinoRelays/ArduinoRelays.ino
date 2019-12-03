#include <SPI.h>
#include <WiFi101.h>
#include <MQTT.h>
#include <MQTTClient.h>
#include <Arduino_MKRENV.h>
#include <string.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP)
char mqttUser[] = MQTT_USER;      // User
char mqttPass[] = MQTT_PASS;      // Password
int status = WL_IDLE_STATUS;     // the WiFi radio's status
char mqttServer[] = SECRET_MQTT_HOST;
char mqttClientId[] = SECRET_MQTT_CLIENT_ID;
const int mqttport = 1883;

// Relays on pins 1 and 2
const int relay_1 = 1;
const int relay_2 = 2;

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

//Process Message
void messageReceived(String &topic, String &payload);

// Initialize the client library
WiFiClient wifiClient;
MQTTClient mqttClient;
//IPAddress server(192,168,1,46);

long lastReconnectAttempt = 0;
long now;


//boolean reconnect() {
//  Serial.print("Attempting MQTT connection...");
//  if (mqttClient.connect("arduinoClient", mqttuser, mqttpass)) {
//    Serial.println("Arduino connected to MQTT");
//    // Once connected, publish an announcement...
//    mqttClient.publish("outTopic","hello world");
//    // ... and resubscribe
//    mqttClient.subscribe("inTopic");
//  }else {
//    Serial.print("Failed, rc=");
//    Serial.print(mqttClient.connected());
//    Serial.println(" try again in 5 seconds");
//  }
//
//  return mqttClient.connected();
//}

void connectMqttServer()
{
    Serial.print("checking wifi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
    /*
    randomSeed(analogRead(0));
    randNumber = random(300);
    char mqttClientId[] = String(randNumber);
    */
    // MQTT client connection request
    mqttClient.begin(mqttServer, wifiClient);
    Serial.print("\nconnecting to MQTT server...");
    while (!mqttClient.connect(mqttClientId, mqttUser, mqttPass))
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nconnected!");

    mqttClient.subscribe("/hello");
}


int incomingByte = 0; // for incoming serial data
String input = "";

void setup() {
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2, OUTPUT);
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Initialize MRKENV
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }  

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWiFiData();

  // MQTT client connection request
  mqttClient.begin(mqttServer, wifiClient);
  Serial.print("Attempting MQTT connection...");
  Serial.print(mqttClient.connected());
  if (mqttClient.connect("arduinoClient", mqttUser, mqttPass)) {
    Serial.println("connected");
    //mqttClient.publish("outTopic","hello world");
    //mqttClient.subscribe("inTopic");
     Serial.println("Connected to MQTT");  
     mqttClient.subscribe("homie/mkr1000/mkrrelay/relay_1/set");  
     mqttClient.subscribe("homie/mkr1000/mkrrelay/relay_2/set");   
     mqttClient.onMessage(messageReceived);  
  
     //client.subscribe("/hello");  
 
     mqttClientPublishInit();
  }
  lastReconnectAttempt = 0;
}

void mqttClientPublishInit(){
  mqttClient.publish("homie/arduino/$homie", "3.0", true);
  mqttClient.publish("homie/arduino/$name", "Arduino MKR1000", true);
  mqttClient.publish("homie/arduino/$nodes", "mkrenv", true);
  mqttClient.publish("homie/arduino/$state", "ready", true);
  
//  mqttClient.publish("homie/arduino/mkrenv/$name", "Lisardo's MKRENV", true);
//  mqttClient.publish("homie/arduino/mkrenv/$properties", "(temperature, humidity, pressure, illuminance, uva, uvb, uvindex)", true);
//  //Temperature
//  mqttClient.publish("homie/arduino/mkrenv/temperature/$name", "Temperature", true);
//  mqttClient.publish("homie/arduino/mkrenv/temperature/$unit", "ºC", true);
//  mqttClient.publish("homie/arduino/mkrenv/temperature/$datatype", "float", true);
//  mqttClient.publish("homie/arduino/mkrenv/temperature/$settable", "true", true);
//  //Humidity
//  mqttClient.publish("homie/arduino/mkrenv/humidity/$name", "Humidity", true);
//  mqttClient.publish("homie/arduino/mkrenv/humidity/$unit", "%", true);
//  mqttClient.publish("homie/arduino/mkrenv/humidity/$datatype", "float", true);
//  mqttClient.publish("homie/arduino/mkrenv/humidity/$settable", "true", true);  
//  //Pressure
//  mqttClient.publish("homie/arduino/mkrenv/pressure/$name", "Pressure", true);
//  mqttClient.publish("homie/arduino/mkrenv/pressure/$unit", "kPa", true);
//  mqttClient.publish("homie/arduino/mkrenv/pressure/$datatype", "float", true);
//  mqttClient.publish("homie/arduino/mkrenv/pressure/$settable", "true", true); 
//  //Illuminance
//  mqttClient.publish("homie/arduino/mkrenv/illuminance/$name", "Illuminance", true);
//  mqttClient.publish("homie/arduino/mkrenv/illuminance/$unit", "lx", true);
//  mqttClient.publish("homie/arduino/mkrenv/illuminance/$datatype", "float", true);
//  mqttClient.publish("homie/arduino/mkrenv/illuminance/$settable", "true", true); 
//  //UVA
//  mqttClient.publish("homie/arduino/mkrenv/uva/$name", "UVA", true);
//  mqttClient.publish("homie/arduino/mkrenv/uva/$unit", "", true);
//  mqttClient.publish("homie/arduino/mkrenv/uva/$datatype", "float", true);
//  mqttClient.publish("homie/arduino/mkrenv/uva/$settable", "true", true);   
//  //UVB
//  mqttClient.publish("homie/arduino/mkrenv/uvb/$name", "UVB", true);
//  mqttClient.publish("homie/arduino/mkrenv/uvb/$unit", "", true);
//  mqttClient.publish("homie/arduino/mkrenv/uvb/$datatype", "float", true);
//  mqttClient.publish("homie/arduino/mkrenv/uvb/$settable", "true", true);   
//  //UVIndex
//  mqttClient.publish("homie/arduino/mkrenv/uvindex/$name", "UV Index", true);
//  mqttClient.publish("homie/arduino/mkrenv/uvindex/$unit", "", true);
//  mqttClient.publish("homie/arduino/mkrenv/uvindex/$datatype", "float", true);
//  mqttClient.publish("homie/arduino/mkrenv/uvindex/$settable", "true", true);  
}

void loop()
{
    // MQTT client:
    mqttClient.loop();

    if (!mqttClient.connected())
    {
        connectMqttServer();
    }
}

//void mqttPublish(){
//   // read all the sensor values
//  float temperature = ENV.readTemperature();
//  float humidity    = ENV.readHumidity();
//  float pressure    = ENV.readPressure();
//  float illuminance = ENV.readIlluminance();
//  float uva         = ENV.readUVA();
//  float uvb         = ENV.readUVB();
//  float uvIndex     = ENV.readUVIndex();
//  
//  // print each of the sensor values and publish its value in mqtt
//  Serial.print("Temperature = ");
//  Serial.print(temperature);
//  Serial.println(" °C");
//  //dtostrf(temperature,0, 0, buffer);
//  mqttClient.publish("homie/arduino/mkrenv/temperature", String(temperature).c_str(), true);  
//
//  Serial.print("Humidity    = ");
//  Serial.print(humidity);
//  Serial.println(" %");
//  mqttClient.publish("homie/arduino/mkrenv/humidity", String(humidity).c_str(), true);
//
//  Serial.print("Pressure    = ");
//  Serial.print(pressure);
//  Serial.println(" kPa");
//  mqttClient.publish("homie/arduino/mkrenv/pressure", String(pressure).c_str(), true);
//
//  Serial.print("Illuminance = ");
//  Serial.print(illuminance);
//  Serial.println(" lx");
//  mqttClient.publish("homie/arduino/mkrenv/illuminance", String(illuminance).c_str(), true);
//
//  Serial.print("UVA         = ");
//  Serial.println(uva);
//  mqttClient.publish("homie/arduino/mkrenv/uva", String(uva).c_str(), true);
//
//  Serial.print("UVB         = ");
//  Serial.println(uvb);
//  mqttClient.publish("homie/arduino/mkrenv/uvb", String(uvb).c_str(), true);
//
//  Serial.print("UV Index    = ");
//  Serial.println(uvIndex);
//  mqttClient.publish("homie/arduino/mkrenv/uvindex", String(uvIndex).c_str(), true);
//
//  // print an empty line
//  Serial.println();
//
//}


void printWiFiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);

}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}


void messageReceived(String &topic, String &payload) { 
  String statusRelay1= "homie/mkr1000/mkrrelay/relay_1";
  String statusRelay2= "homie/mkr1000/mkrrelay/relay_2";
  
  Serial.println("incoming: " + topic + " : " + payload);  
    if(topic.equals("homie/mkr1000/mkrrelay/relay_1/set"))  {          
        if (payload.equals("HIGH")) {
          digitalWrite(relay_1, HIGH);
          if (mqttClient.publish(statusRelay1, payload, true,2)){
            Serial.println("Relay 1: SWITCHED ON");
          }
        } 
        if (payload.equals("LOW")){
          digitalWrite(relay_1, LOW);
          if (mqttClient.publish(statusRelay1, payload, true,2)) {
            Serial.println("Relay 1: SWITCHED OFF");
          }
        }
    }
    if(topic.equals("homie/mkr1000/mkrrelay/relay_2/set"))  {          
        if (payload.equals("HIGH")) {
          digitalWrite(relay_2, HIGH);
          if (mqttClient.publish(statusRelay2, payload, true,2)) {   
            Serial.println("Relay 2: SWITCHED ON");
          }
        } 
        if (payload.equals("LOW")){
          digitalWrite(relay_2, LOW);
          if (mqttClient.publish(statusRelay2, payload, true,2)) {
            Serial.println("Relay 2: SWITCHED OFF");
          }
        }
    }
}
    
