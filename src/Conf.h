// Conf.h
#ifndef CONF_H
#define CONF_H

#include <Arduino.h>

#define sleepTime 1*30*60 // 30 min

// Define the appEUI, devEUI, and appSKey as static to limit their linkage to this translation unit
static const char *appEUI = "0000000000000000";
static const char *devEUI = "0000000000000000";
static const char *appSKey = "000000000000000000000000000000000"; // Change to TTN Application Session Key
#define CHANNEL 0


#define SDS_PIN_RX 16
#define SDS_PIN_TX 17

constexpr uint32_t down_s = 30;
constexpr uint32_t duty_s = 30;
constexpr int pm_tablesize = 20;

#endif // CONF_H
