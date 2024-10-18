#include "SDSFunc.h"
#include <Arduino.h>
// Initialize global variables
int pm25_table[pm_tablesize];
int pm10_table[pm_tablesize];

int opm25;
int opm10;

bool is_SDS_running = true;

HardwareSerial &serialSDS(Serial2);
Sds011Async<HardwareSerial> sds011(serialSDS);

void start_SDS()
{
  Serial.println("Start wakeup SDS011");

  if (sds011.set_sleep(false))
  {
    is_SDS_running = true;
  }

  Serial.println("End wakeup SDS011");
}

void stop_SDS()
{
  Serial.println("Start sleep SDS011");

  if (sds011.set_sleep(true))
  {
    is_SDS_running = false;
  }

  Serial.println("End sleep SDS011");
}

void handle_SDS_data(int n)
{
  Serial.println(F("Begin Handling SDS011 query data"));
  int pm25;
  int pm10;
  Serial.print(F("n = "));
  Serial.println(n);
  if (sds011.filter_data(n, pm25_table, pm10_table, pm25, pm10) && !isnan(pm10) && !isnan(pm25))
  {
    Serial.print(F("PM10: "));
    Serial.println(float(pm10) / 10);
    Serial.print(F("PM2.5: "));
    Serial.println(float(pm25) / 10);
    opm10 = pm10;
    opm25 = pm25;
  }
  Serial.println(F("End Handling SDS011 query data"));
}

void setup_SDS()
{
#ifdef ESP32
  serialSDS.begin(9600, SERIAL_8N1, SDS_PIN_RX, SDS_PIN_TX);
  delay(100);
#else
  serialSDS.begin(9600, SWSERIAL_8N1, SDS_PIN_RX, SDS_PIN_TX, false, 192);
#endif

  Serial.println(F("SDS011 start/stop and reporting sample"));

  Sds011::Report_mode report_mode;
  constexpr int GETDATAREPORTINMODE_MAXRETRIES = 2;
  for (auto retry = 1; retry <= GETDATAREPORTINMODE_MAXRETRIES; ++retry)
  {
    if (!sds011.get_data_reporting_mode(report_mode))
    {
      if (retry == GETDATAREPORTINMODE_MAXRETRIES)
      {
        Serial.println(F("Sds011::get_data_reporting_mode() failed"));
      }
    }
    else
    {
      break;
    }
  }
  if (Sds011::REPORT_ACTIVE != report_mode)
  {
    Serial.println(F("Turning on Sds011::REPORT_ACTIVE reporting mode"));
    if (!sds011.set_data_reporting_mode(Sds011::REPORT_ACTIVE))
    {
      Serial.println(F("Sds011::set_data_reporting_mode(Sds011::REPORT_ACTIVE) failed"));
    }
  }
  sds011.on_query_data_auto_completed(handle_SDS_data);
}

void loop_SDS()
{
  uint32_t deadline;

  start_SDS();
  Serial.print(F("started SDS011 (is running = "));
  Serial.print(is_SDS_running);
  Serial.println(')');

  if (!sds011.query_data_auto_async(pm_tablesize, pm25_table, pm10_table))
  {
    Serial.println(F("measurement capture start failed"));
  }

  deadline = millis() + duty_s * 1000;
  while (static_cast<int32_t>(deadline - millis()) > 0)
  {
    delay(1000);
    Serial.println(static_cast<int32_t>(deadline - millis()) / 1000);
    sds011.perform_work();
  }

  stop_SDS();
  Serial.print(F("stopped SDS011 (is running = "));
  Serial.print(is_SDS_running);
  Serial.println(')');
}
