#ifndef __GlobalDefinitions__H
#define __GlobalDefinitions__H

#pragma once

// CConfigurationManager Definitions
#define ESP8266_LED 2

const char WiFiAPPID[] = "ESP8266 ConfigWifi";
const char WiFiAPPSK[] = "123456789";

// AM2320 Definitions
#define AM2320_address (0xB8 >> 1)

// CConfigurationFile Definitions
#define CONFIG_FILE_NAME "/config.txt"
#define CONFIG_FILE_SEPAREATOR '&'
#define CONFIG_FILE_EQUALS '='
#define CONFIG_FILE_END '#'

// Sensor Types defines
#define UNDEFINED_SENSOR_KEY -1
#define AM2320_SENSOR_KEY 0
#define GY68_SENSOR_KEY 1
#define BME280_SENSOR_KEY 2

// CConfigurationManager Definitions

// CWorkingStation Definitions
#define SCL 2
#define SDA 0

#define MQTT_PUBLISH_INTERVAL_SEC 60 * 5

// Define the sensor MQTT Publish topic
#define BED_ROOM_SENSOR      0
#define LIVING_ROOM_SENSOR   1
#define OUTSIDE_ROOM_SENSOR  2
#define KIDS_ROOM_SENSOR     3

#define CURRENT_SENSOR_LOCATION KIDS_ROOM_SENSOR

// Real Global Definitions
#define PRINT_LINES 1

// BED_ROOM Sensor defines
#if (CURRENT_SENSOR_LOCATION == BED_ROOM_SENSOR)
const char stationID[] = "ESP8266_BedRoom";
const char temp_topic[] = "bedroom/temperature";
const char hum_topic[] = "bedroom/humidity";
const char refreshInt_topic[] = "bedroom/refreshInterval";
const char battery_topic[] = "bedroom/battery";
#endif

// LIVING_ROOM Sensor defines
#if (CURRENT_SENSOR_LOCATION == LIVING_ROOM_SENSOR)
const char stationID[] = "ESP8266_LivingRoom";
const char temp_topic[] = "livingRoom/temperature";
const char hum_topic[] = "livingRoom/humidity";
const char refreshInt_topic[] = "livingRoom/refreshInterval";
const char battery_topic[] = "livingRoom/battery";
#endif

// KIDS_ROOM Sensor defines
#if (CURRENT_SENSOR_LOCATION == KIDS_ROOM_SENSOR)
const char stationID[] = "ESP8266_KidsRoom";
const char temp_topic[] = "kidsRoom/temperature";
const char hum_topic[] = "kidsRoom/humidity";
const char refreshInt_topic[] = "kidsRoom/refreshInterval";
const char battery_topic[] = "kidsRoom/battery";
#endif

const char plant_water_topic[] = "kidsRoom/plant_water";

// OUTSIDE Sensor defines
const char pressure_topic[] = "outside/pressure";
const char altitude_topic[] = "outside/altitude";

#if (CURRENT_SENSOR_LOCATION == OUTSIDE_ROOM_SENSOR)
const char stationID[] = "ESP8266_Outside";
const char temp_topic[] = "outside/temperature";
const char hum_topic[] = "outside/humidity";
const char refreshInt_topic[] = "outside/refreshInterval";
const char battery_topic[] = "outside/battery";
#endif

#if (PRINT_LINES == 1)
#define Print(x) Serial.print(x)
#define Println(x) Serial.println(x)
#define SERIAL_CONFIGURE Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY)
#define SERIAL_END Serial.flush(); \
                   Serial.end()
#else
#define Print(x) ((void)0)
#define Println(x) ((void)0)
#define SERIAL_CONFIGURE ((void)0)
#define SERIAL_END ((void)0)
#endif // DEBUG


// Define ERRORS Values
// TODO How will the values be encoded and send?
#define AM2320_SENSOR_CRC_ERROR 0
#define AM2320_SENSOR_OFFLINE_ERROR 1

#define MQTT_PUBLISH_TEMP_ERROR 3
#define MQTT_PUBLISH_HUM_ERROR 4
#define MQTT_PUBLISH_REFRESH_INT_ERROR 5
#define MQTT_PUBLISH_BATTERY_ERROR 6

#define ESP8266_BATTERY_LOW_ERROR 2

#define ESP_MIN_OPERATE_VOLTAGE 2.8

#define SEALEVELPRESSURE_HPA (1013.25)

#endif // !__GlobalDefinitions__H
