#include "WorkingStation.h"
#include "ConfigurationFile.h"

/*
*	\brief Initialize the component
*
*/
bool CWorkingStation::Init()
{
    CConfigurationFile configFile;
    configFile.ParseConfiguration();

    int ssidLen = strlen(configFile.m_ssid) + 1;
    int pskLen = strlen(configFile.m_psk) + 1;

    if (m_ssid)
        delete[] m_ssid;

    m_ssid = new char[ssidLen];

    if (m_psk)
        delete[] m_psk;

    m_psk = new char[pskLen];

    memcpy(m_ssid, configFile.m_ssid, ssidLen);
    memcpy(m_psk, configFile.m_psk, pskLen);

    ConnectToWifi();

    IPAddress mqttServerIPAddr;
    mqttServerIPAddr.fromString(configFile.m_mqttServerIP);

    m_client.setServer(mqttServerIPAddr, configFile.m_mqttServerPort);
    m_measureInterval = configFile.m_measure;

    m_sensorType = configFile.m_sensorType;
    bool result = InitSensor();

    Print("Sensor Init Result: ");
    Println(result);

    return result;
}



/*
*	\brief This will loop in main
*/
void CWorkingStation::Work(WorkMode mode)
{
    switch (mode)
    {
    case NORMAL:
        Println("Executing Normal Task");
        NormalTask();
        break;
    case POWER_SAVE:
        Println("Executing Deep Sleep Task");
        DeepSleepTask(m_measureInterval * 60); // m_measureInterval is in mins.
        break;
    default:
        break;
    }
}



/*
*	\brief This will loop in main
*/
bool CWorkingStation::ReconnectMQTT()
{
    int tryCounter = 0;

    while (!m_client.connected())
    {
        if (++tryCounter > 4)
            break;

        if (m_client.connect(stationID))
        {
            Println("Client connected!");
            return true;
        }
        else
        {
            Print("failed, rc=");
            Print(m_client.state());
            Println(" try again in 2 seconds");
            // Wait 2 seconds before retrying
            delay(2000);
        }
    }

    return false;
}

void CWorkingStation::NormalTask()
{
    DoWork();
    WifiSleep(MQTT_PUBLISH_INTERVAL_SEC);
}

void CWorkingStation::DeepSleepTask(int secs)
{
    long sleepUs = 1000000 * secs; // 1 second = 1 000 000 microseconds

    DoWork();

    Println("Entering deep sleep mode...");
    Print("Wakeup after (s): ");
    Println(secs);

    digitalWrite(1, LOW);
    SERIAL_END;

    // Wait 1 second for MQTT to finish publishing.
    delay(1000);

    // Disconnect MQTT client.
    m_client.disconnect();
    delay(500);

    ESP.deepSleep(sleepUs);
    delay(100);
}

void CWorkingStation::DoWork()
{
    if (!m_client.connected())
    {
        bool bRes = ReconnectMQTT();

        if (!bRes)
        {
            Println("MQTT: More than 3 attemps were made. Skipping work.");
            return;
        }
    }

    float espVcc = (ESP.getVcc() / 1000.0) + 0.005; // In Volts

    Print("Battery VCC: ");
    Println(espVcc);

    if (espVcc < ESP_MIN_OPERATE_VOLTAGE)
    {
        Println("Low on power. Not going to work");
        return;
    }

    char battery_payload[10] = { 0 };
    sprintf(battery_payload, "%.2f", espVcc);

    boolean bBatteryResult = m_client.publish(battery_topic, battery_payload);

    Print("Battery publish result: ");
    Println(bBatteryResult);

    switch (m_sensorType)
    {
    case AM2320_SENSOR_KEY:

        Println("Reading AM2320 Sensor...");
        OperateAM2320Sensor();
        break;

    case GY68_SENSOR_KEY:

        Println("Reading GY68 Sensor...");
        OperateGY68Sensor();

        break;
    case BME280_SENSOR_KEY:

        Println("Reading BME280 Sensor...");
        OperateBME280Sensor();

        break;
    }
}

void CWorkingStation::ConnectToWifi()
{
    WiFi.mode(WIFI_STA);
    delay(20);

    WiFi.begin(m_ssid, m_psk);

    int tries = 0;
    Print("Connecting");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Print(".");

        if (tries++ > 10)
        {
            Println("Failed to connect! Enter deep sleep mode.");
            ESP.deepSleep(MQTT_PUBLISH_INTERVAL_SEC * 1000000);
        }
    }

    Println("");
    Print("Connected, IP address: ");
    Println(WiFi.localIP());
}

void CWorkingStation::WifiSleep(long sec)
{
    Println("Putting Wifi down...");

    m_client.disconnect();
    delay(10);

    WiFi.setSleepMode(WiFiSleepType::WIFI_LIGHT_SLEEP);
    WiFi.disconnect();
    delay(10);

    WiFi.mode(WIFI_OFF);
    bool wifisleep = WiFi.forceSleepBegin(sec * 1000000L); //In uS. Must be same length as your delay
    delay(10);

    delay((sec + 1/* buffer sec*/) * 1000); //

    WiFi.forceSleepWake();
    Println("Awake!");
    ConnectToWifi();

    ReconnectMQTT();
}

bool CWorkingStation::InitSensor()
{
    bool result = false;

    Wire.begin(SDA, SCL);

    switch (m_sensorType)
    {
    case AM2320_SENSOR_KEY:

        m_am2320_sensor = new AM2320();
        if (m_am2320_sensor)
            result = true;

        break;
    case GY68_SENSOR_KEY:

        m_gy68_sensor = new Adafruit_BMP085_Unified();
        if (m_gy68_sensor)
            result = m_gy68_sensor->begin();

        break;
    case BME280_SENSOR_KEY:

        m_bme280_sensor = new Adafruit_BME280();
        if (m_bme280_sensor)
            result = m_bme280_sensor->begin(0x76);

        break;
    default:
        break;
    }

    return result;
}

bool CWorkingStation::TerminateSensor()
{
    bool result = false;

    switch (m_sensorType)
    {
    case AM2320_SENSOR_KEY:

        if (m_am2320_sensor)
            delete m_am2320_sensor;

        if (!m_am2320_sensor)
            result = true;

        break;
    case GY68_SENSOR_KEY:

        if (m_gy68_sensor)
            delete m_gy68_sensor;

        if (!m_gy68_sensor)
            result = true;

        break;
    case BME280_SENSOR_KEY:

        if (m_bme280_sensor)
            delete m_bme280_sensor;

        if (!m_bme280_sensor)
            result = true;

        break;
    default:
        break;
    }

    return result;
}

void CWorkingStation::OperateAM2320Sensor()
{
    Println("Getting readings AM2320...");

    float temp = 0;
    float hum = 0;

    switch (m_am2320_sensor->Read())
    {
    case 2:
        Println("CRC Checksum failed");
        break;
    case 1:
        Println("Sensor offline");
        break;
    case 0:
        temp = m_am2320_sensor->temp;
        hum = m_am2320_sensor->hum;

        Print("Temp: ");
        Println(temp);
        Print("Hum: ");
        Println(hum);
        break;
    }

    String temp_payload(temp);
    String hum_payload(hum);
    String refreshInt_payload(m_measureInterval);

    // Publishes Temperature and Humidity values
    boolean bTempResult = m_client.publish(temp_topic, temp_payload.c_str());
    boolean bHumResult = m_client.publish(hum_topic, hum_payload.c_str());
    boolean bRefreshIntResult = m_client.publish(refreshInt_topic, refreshInt_payload.c_str());

    Print("Tem publish result: ");
    Println(bTempResult);

    Print("Hum publish result: ");
    Println(bHumResult);

    Print("RefreshInt publish result: ");
    Println(bRefreshIntResult);
}

void CWorkingStation::OperateGY68Sensor()
{
    Println("Getting readings GY68...");

    float pressure;
    float temp;
    float altitude;

    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;

    sensors_event_t sensorData;

    Println("getEvent.");
    bool result = m_gy68_sensor->getEvent(&sensorData);

    Println("Event got.");

    if (result)
    {
        pressure = sensorData.pressure;
        altitude = m_gy68_sensor->pressureToAltitude(seaLevelPressure, sensorData.pressure);

        m_gy68_sensor->getTemperature(&temp);

        Print("Temp: ");
        Print(temp);
        Println(" C");

        Print("Pressure: ");
        Print(pressure);
        Println(" hPa");

        Print("Altitude: ");
        Print(altitude);
        Println(" m");

        String temp_payload(temp);
        String pressure_payload(pressure);
        String altitude_payload(altitude);
        String refreshInt_payload(m_measureInterval);

        // Publish data
        boolean bTempResult = m_client.publish(temp_topic, temp_payload.c_str());
        boolean bPressureResult = m_client.publish(pressure_topic, pressure_payload.c_str());
        boolean bAltitudeResult = m_client.publish(altitude_topic, altitude_payload.c_str());
        boolean bRefreshIntResult = m_client.publish(refreshInt_topic, refreshInt_payload.c_str());

        Print("Tem publish result: ");
        Println(bTempResult);

        Print("Pressure publish result: ");
        Println(bPressureResult);

        Print("Altitude publish result: ");
        Println(bAltitudeResult);

        Print("RefreshInt publish result: ");
        Println(bRefreshIntResult);
    }
}

void CWorkingStation::OperateBME280Sensor()
{
    Println("Getting readings BME280...");

    float pressure = m_bme280_sensor->readPressure() / 100.0F;
    float temp = m_bme280_sensor->readTemperature();
    float hum = m_bme280_sensor->readHumidity();
    float altitude = m_bme280_sensor->readAltitude(SEALEVELPRESSURE_HPA);

    Print("Temp: ");
    Print(temp);
    Println(" C");

    Print("Pressure: ");
    Print(pressure);
    Println(" hPa");

    Print("Humidity: ");
    Print(hum);
    Println(" %");

    String temp_payload(temp);
    String pressure_payload(pressure);
    String altitude_payload(altitude);
    String hum_payload(hum);
    String refreshInt_payload(m_measureInterval);

    // Publish data
    boolean bTempResult = m_client.publish(temp_topic, temp_payload.c_str());
    boolean bPressureResult = m_client.publish(pressure_topic, pressure_payload.c_str());
    boolean bAltitudeResult = m_client.publish(altitude_topic, altitude_payload.c_str());
    boolean bRefreshIntResult = m_client.publish(refreshInt_topic, refreshInt_payload.c_str());
    boolean bHumResult = m_client.publish(hum_topic, hum_payload.c_str());

    Print("Tem publish result: ");
    Println(bTempResult);

    Print("Pressure publish result: ");
    Println(bPressureResult);

    Print("Altitude publish result: ");
    Println(bAltitudeResult);

    Print("RefreshInt publish result: ");
    Println(bRefreshIntResult);

    Print("Hum publish result: ");
    Println(bHumResult);
}
