#ifndef SDSFUNC_H
#define SDSFUNC_H
#include "Conf.h"

#include <esp_sds011.h>
#include <HardwareSerial.h>

extern HardwareSerial &serialSDS;
extern Sds011Async<HardwareSerial> sds011;

// SDS011 Var/Functions
extern int pm25_table[pm_tablesize];
extern int pm10_table[pm_tablesize];

extern int opm25;
extern int opm10;

extern bool is_SDS_running;

// Function declarations
void start_SDS();
void stop_SDS();
void setup_SDS();
void loop_SDS();
void handle_SDS_data(int n);

#endif // SDSFUNC_H
