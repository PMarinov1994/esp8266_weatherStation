#pragma once
#include <WString.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiSTA.h>
#include <FS.h>
#include <Wire.h>
#include "AM2320.h"
#include "GY68.h"
#include "Adafruit_BME280.h"

#include <PubSubClient.h>
#include "GlobalDefinitions.h"


enum WorkMode
{
	NORMAL = 0,
	POWER_SAVE = 1
};

class CWorkingStation
{
public:
	CWorkingStation()
		: m_am2320_sensor()
		, m_client(m_espClient)
		//, m_now(millis())
		//, m_lastMeasure(0)
        , m_sensorType(AM2320_SENSOR_KEY)
	{
	};

	virtual ~CWorkingStation()
	{
		if (m_ssid)
			delete[] m_ssid;

		if (m_psk)
			delete[] m_psk;

        bool terminateRes = TerminateSensor();
        Print("Sensor termination result: ");
        Println(terminateRes);
	};

	bool Init();
	void Work(WorkMode mode);

private:

	bool ReconnectMQTT();

	void NormalTask();
	void DeepSleepTask(int secs);

	void DoWork();
	void ConnectToWifi();

	void WifiSleep(long sec);

    bool InitSensor();
    bool TerminateSensor();

    void OperateAM2320Sensor();
    void OperateGY68Sensor();
    void OperateBME280Sensor();

private:

	//long m_now;
	//long m_lastMeasure;

    // Measurement in minutes.
    long m_measureInterval;

	char* m_ssid;
	char* m_psk;

    int m_sensorType;

	AM2320* m_am2320_sensor;
    Adafruit_BMP085_Unified* m_gy68_sensor;
    Adafruit_BME280* m_bme280_sensor;

	PubSubClient m_client;
	WiFiClient m_espClient;
};