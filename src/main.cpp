////////////////////////////////////
/*
BME
Esp32 pins SDA 21 | SCL 22
------------------------
SDS
Esp32 RX 16, TX 17
*/
///////////////////////////////////

#define VID 001

#include <Arduino.h>
#include "Conf.h"
#include "HelperFuncs/SDSFunc.h"
#include "HelperFuncs/BMEFunc.h"

///////////////////
/// lora
#include "TTN_esp32.h"
#include "Cayenne/TTN_CayenneLPP.h"

TTN_esp32 ttn;
TTN_CayenneLPP lpp;

void message(const uint8_t *payload, size_t size, int rssi)
{
  Serial.println("-- MESSAGE");
  Serial.print("Received " + String(size) + " bytes RSSI= " + String(rssi) + "dB");

  for (int i = 0; i < size; i++)
  {
    Serial.print(" " + String(payload[i]));
    Serial.write(payload[i]);
  }

  Serial.println();
}
/////////////////////////////////////////////////////////////////////////
void enterDeepSleep()
{
  // Put the SDS011 sensor to sleep
  stop_SDS();
  delay(5000);
  // Print a message before going to sleep
  Serial.println("Entering deep sleep for " + String(sleepTime) + " seconds...");
  delay(10);
  digitalWrite(LED_BUILTIN, LOW);

  // Configure deep sleep for X seconds (time is in microseconds)
  esp_sleep_enable_timer_wakeup(sleepTime * 1000000);
  delay(50);
  // Enter deep sleep
  esp_light_sleep_start();
}

void setup()
{
  Serial.begin(115200);
  setup_SDS();
  setup_BME();
  ////////////////////////////////////////////////////////////////
  ////////lora

  Serial.println("Starting Lora TTN");
  ttn.begin();
  ttn.onMessage(message); // declare callback function when is downlink from server
  ttn.join(devEUI, appEUI, appSKey, 10);
  Serial.print("Joining TTN ");
  while (!ttn.isJoined())
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\njoined !");

  ttn.showStatus();
}

void loop()
{
  uint32_t deadline;
  digitalWrite(LED_BUILTIN, HIGH);
  loop_SDS();
  float temp(NAN), hum(NAN), pres(NAN);

  read_BME(temp, hum, pres);
  ////////////////////////////////////////////
  /// lora

  lpp.reset();
  lpp.addAnalogInput(99, VID);
  lpp.addTemperature(1, temp);
  lpp.addRelativeHumidity(2, hum);
  lpp.addPM25(3, float(opm25) / 10);
  lpp.addPM10(4, float(opm10) / 10);

  Serial.println("Buffer size: " + lpp.getSize());

  for (size_t i = 0; i < lpp.getSize(); i++)
  {
    Serial.print(lpp.getBuffer()[i], HEX);
  }
  Serial.println("");

  if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize()))
  {
    // Serial.printf("Temp: %f TTN_CayenneLPP: %d %x %02X%02X\n", hum, lpp.getBuffer()[0], lpp.getBuffer()[1],
    //     lpp.getBuffer()[2], lpp.getBuffer()[3], lpp.getBuffer()[4],lpp.getBuffer()[5],lpp.getBuffer()[6]);
    Serial.println("SENT TTN");
    Serial.println("BME280");
    Serial.println("Temp: " + String(temp));
    Serial.println("Hum: " + String(hum));
    Serial.println("SDS011");
    Serial.println("PM2.5: " + String((float)opm25 / 10));
    Serial.println("PM10: " + String((float)opm10 / 10));
    Serial.println("");
    Serial.println("");
    delay(5000);
  }
  enterDeepSleep();
  /////////////////////////
  // delay(60000);
}