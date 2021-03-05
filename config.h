#ifndef CONFIG_H
#define CONFIG_H

#define ESP8266_LED 5
#define GM_PULSE 12

//This is the config file for ha-sts/mqtt-sensor-geiger-muller
const char* ssid = "example-ssid";
const char* password = "password";
const char* mqtt_server = "mqtt.example.com";
const int mqtt_port = 8883;

#endif // CONFIG_H
