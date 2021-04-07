# esp8266 WeatherStation

This is a firmware for ESP8266 Generic board. It features two operation modes: working and cofiguration.
In Working mode, the firmware publishes data got from an AM2320 Sensor to an MQTT Broker. In configuration mode,
it creates an Wi-Fi access point and can be used to configure the MQTT Broker connection and data pull time.

To build this project you need Arduino IDE and/or Visual Studio 2017 with Visual Micro Extension installed.

Most customizations are done in GlobalDefinitions.h file.

## TODO:
- Add support for GY-68 Sensor.
- Add Presure and altitude data pull.
- Change configuration HTML page's fields to be opptional. Right now, if *ssid*, *psk*, *mqtt*, *mqttPort* are left empty, then the configuration will fail.
They sould be read from *config.txt* and overridden if not left empty.

## Visual Micro settings
- Builtin Led: 2
- Upload Speed: 115200
- CPU Frequency: 80MHz
- Crystal Frequency: 26MHz
- Flash Size: 512KB (FS: 32KB) [Can be wilth 1Mb as well]
- Flash Mode: DIO
- Flash Frequency: 40MHz
- Reset Method: no dtr (aka ck)
- Debug port: Disabled
- Debug Level: None
- iwIP Variant: v2 Lower Memory
- VTables: Flash
- Exceptions: Legacy (new can return nullptr)
- Erase Flash: Only Sketch
- Espressif FW: nonos-sdk 2.2.1+100 (190703) [Not important]
