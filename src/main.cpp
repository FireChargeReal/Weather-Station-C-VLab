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
#include "esp_task_wdt.h"

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
//////////////////////////////////////////////////////////////////////////

void watchdogsSetup()
{
  // Initialize the watchdog timer with a timeout of 5 seconds
  esp_task_wdt_init(WDPanicTime, true); // Enable panic on timeout
  esp_task_wdt_add(NULL);               // Add the current task (loop task) to the watchdog
}

void enterDeepSleep()
{
  // Put the SDS011 sensor to sleep
  stop_SDS();
  delay(5000);
// Print a message before going to sleep
#ifdef DEBUG
  Serial.println("Entering deep sleep for " + String(sleepTime) + " seconds...");
#endif
  delay(10);
  // digitalWrite(LED_BUILTIN, LOW);
  esp_task_wdt_delete(NULL); // Remove main loop task from watchdog

  // Configure deep sleep for X seconds (time is in microseconds)
  esp_sleep_enable_timer_wakeup(sleepTime * 1000000);

  // Enter deep sleep
  esp_deep_sleep_start();
}

// RTC_DATA_ATTR int coldBootCount = 0; // variable to store how many times micro controller crashed or was reset due to bad init

void setup()
{
  Serial.begin(115200);

  // Detect wake-up reason
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER)
  {
#ifdef DEBUG
    Serial.println("Woke up from deep sleep");
#endif
  }
  else
  {
    // coldBootCount++;

#ifdef DEBUG
    Serial.println("Cold boot!!!");
    // Serial.println("Cold boot count: "+ String(coldBootCount));

#endif
  }
  watchdogsSetup();

  // setups
  setup_SDS();
  setup_BME();

  // lora setup

#ifdef DEBUG
  Serial.println("Starting Lora TTN");
#endif
  ttn.begin();
  ttn.onMessage(message); // declare callback function when is downlink from server
  ttn.join(devEUI, appEUI, appSKey, 10);
#ifdef DEBUG
  Serial.print("Joining TTN ");
#endif
  while (!ttn.isJoined())
  {
#ifdef DEBUG
    Serial.print(".");
#endif
    delay(500);
  }
#ifdef DEBUG
  Serial.println("\njoined !");
#endif

  ttn.showStatus();
}

void loop()
{
  esp_task_wdt_reset(); // Reset the watchdog timer at the beginning of each loop

  uint32_t deadline;
  // digitalWrite(LED_BUILTIN, HIGH);
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

#ifdef DEBUG
  Serial.println("Buffer size: " + lpp.getSize());
#endif

  for (size_t i = 0; i < lpp.getSize(); i++)
  {
#ifdef DEBUG
    Serial.print(lpp.getBuffer()[i], HEX);
#endif
  }
#ifdef DEBUG
  Serial.println("");
#endif

  if (ttn.sendBytes(lpp.getBuffer(), lpp.getSize()))
  {
#ifdef DEBUG
    Serial.println("SENT TTN");

    Serial.println("BME280");

    Serial.println("Temp: " + String(temp));

    Serial.println("Hum: " + String(hum));

    Serial.println("SDS011");

    Serial.println("PM2.5: " + String((float)opm25 / 10));

    Serial.println("PM10: " + String((float)opm10 / 10));

    Serial.println("");

    Serial.println("");
#endif
    delay(5000);
  }
  enterDeepSleep();
}