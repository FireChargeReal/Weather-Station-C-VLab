#include "BMEFunc.h"
#include <Arduino.h>
#include <Wire.h>
#include "Conf.h"


// Initialize the BME sensor object
BME280I2C bme;

void setup_BME()
{
    Wire.begin(); // Initialize I2C communication

    while (!bme.begin())
    {
#ifdef DEBUG
        Serial.println("Could not find BME280 sensor!");
#endif
        delay(5*1000);
        ESP.restart();
    }

    switch (bme.chipModel())
    {
    case BME280::ChipModel_BME280:
#ifdef DEBUG
        Serial.println("Found BME280 sensor! Success.");
#endif
        break;
    case BME280::ChipModel_BMP280:
#ifdef DEBUG
        Serial.println("Found BMP280 sensor! No Humidity available.");
#endif
        break;
    default:
#ifdef DEBUG
        Serial.println("Found UNKNOWN sensor! Error!");
#endif
        break;
    }
}

void read_BME(float &temperature, float &humidity, float &pressure)
{
    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_hPa);

    // Read data from the sensor
    bme.read(pressure, temperature, humidity, tempUnit, presUnit);

// Log the results (optional)
#ifdef DEBUG
    Serial.println("BME280 Readings:");
    Serial.println("Temp: " + String(temperature));
    Serial.println("Hum: " + String(humidity));
    Serial.println("Pressure: " + String(pressure));
#endif
}
