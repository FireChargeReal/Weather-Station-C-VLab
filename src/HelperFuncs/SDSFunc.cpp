#include "SDSFunc.h"
#include <Arduino.h>
#include "Conf.h"

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
#ifdef DEBUG
  Serial.println("Start wakeup SDS011");
#endif

  if (sds011.set_sleep(false))
  {
    is_SDS_running = true;
  }

#ifdef DEBUG
  Serial.println("End wakeup SDS011");
#endif
}

void stop_SDS()
{
#ifdef DEBUG
  Serial.println("Start sleep SDS011");
#endif

  if (sds011.set_sleep(true))
  {
    is_SDS_running = false;
  }

#ifdef DEBUG
  Serial.println("End sleep SDS011");
#endif
}

void handle_SDS_data(int n)
{
#ifdef DEBUG
  Serial.println(F("Begin Handling SDS011 query data"));
#endif
  int pm25;
  int pm10;
#ifdef DEBUG
  Serial.print(F("n = "));
  Serial.println(n);
#endif
  if (sds011.filter_data(n, pm25_table, pm10_table, pm25, pm10) && !isnan(pm10) && !isnan(pm25))
  {
#ifdef DEBUG
    Serial.print(F("PM10: "));

    Serial.println(float(pm10) / 10);

    Serial.print(F("PM2.5: "));

    Serial.println(float(pm25) / 10);
#endif
    opm10 = pm10;
    opm25 = pm25;
  }
#ifdef DEBUG
  Serial.println(F("End Handling SDS011 query data"));
#endif
}

void setup_SDS()
{
#ifdef ESP32
  serialSDS.begin(9600, SERIAL_8N1, SDS_PIN_RX, SDS_PIN_TX);
  delay(100);
#else
  serialSDS.begin(9600, SWSERIAL_8N1, SDS_PIN_RX, SDS_PIN_TX, false, 192);
#endif

#ifdef DEBUG
  Serial.println(F("SDS011 start/stop and reporting sample"));
#endif

  Sds011::Report_mode report_mode;
  constexpr int GETDATAREPORTINMODE_MAXRETRIES = 2;
  for (int retry = 1; retry <= GETDATAREPORTINMODE_MAXRETRIES; ++retry)
  {
    if (!sds011.get_data_reporting_mode(report_mode))
    {
      if (retry == GETDATAREPORTINMODE_MAXRETRIES)
      {
#ifdef DEBUG
        Serial.println(F("Sds011::get_data_reporting_mode() failed. Retry count: "+retry));
#endif
      }
    }
    else
    {
      break;
    }
  }
  if (Sds011::REPORT_ACTIVE != report_mode)
  {
#ifdef DEBUG
    Serial.println(F("Turning on Sds011::REPORT_ACTIVE reporting mode"));
#endif
    if (!sds011.set_data_reporting_mode(Sds011::REPORT_ACTIVE))
    {
#ifdef DEBUG
      Serial.println(F("Sds011::set_data_reporting_mode(Sds011::REPORT_ACTIVE) failed"));
#endif
    }
  }
  sds011.on_query_data_auto_completed(handle_SDS_data);
}

void loop_SDS()
{
  uint32_t deadline;

  start_SDS();
#ifdef DEBUG
  Serial.print(F("started SDS011 (is running = "));
  Serial.print(is_SDS_running);
  Serial.println(')');
#endif

  if (!sds011.query_data_auto_async(pm_tablesize, pm25_table, pm10_table))
  {
#ifdef DEBUG
    Serial.println(F("measurement capture start failed"));
#endif
  }

  deadline = millis() + duty_s * 1000;
  while (static_cast<int32_t>(deadline - millis()) > 0)
  {
    delay(1000);
#ifdef DEBUG
    Serial.println(static_cast<int32_t>(deadline - millis()) / 1000);
#endif
    sds011.perform_work();
  }

  stop_SDS();
#ifdef DEBUG
  Serial.print(F("stopped SDS011 (is running = "));
  Serial.print(is_SDS_running);
  Serial.println(')');
#endif
}
