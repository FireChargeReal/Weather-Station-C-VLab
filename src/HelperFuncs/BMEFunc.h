#ifndef BMEFUNC_H
#define BMEFUNC_H

#include <BME280I2C.h>

// Declare the BME object as extern for global access
extern BME280I2C bme;

// Function declarations
void setup_BME();
void read_BME(float &temperature, float &humidity, float &pressure);

#endif // BMEFUNC_H
