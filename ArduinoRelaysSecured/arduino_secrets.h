#define SECRET_SSID "IotWifiBridged"
#define SECRET_PASS "Iot-2019"
#define SECRET_MQTT_CLIENT_ID "MKR1000"
//#define SECURED
//#define EC2
//#define ZT
#ifdef EC2
  #ifdef ZT
    #define SECRET_MQTT_HOST  "172.24.71.188" //EC2 ZT  
    #ifdef SECURED
      #define MQPORT          8883           
    #else 
      #define MQPORT          1883           
    #endif
  #else
    #define SECRET_MQTT_HOST  "54.194.149.95" //EC2 
    #define MQPORT          1883           
  #endif
  #define MQTT_USER      "soc-iot"
  #define MQTT_PASS      "unic0s19"
#else
  #define MQTT_USER "mqtt_iotuser"
  #define MQTT_PASS "mqttiot.2019"
  #define SECRET_MQTT_HOST "192.168.1.40"  
  #ifdef SECURED
    #define MQPORT          8883           
  #else 
    #define MQPORT          1883           
  #endif
#endif
