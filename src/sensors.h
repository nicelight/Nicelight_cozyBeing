#pragma once
#include <Arduino.h>

#include <GyverDS18.h>
#include <DHT.h>

extern GyverDS18Single ds1;  // пин PIN_DS18B20_1
extern GyverDS18Single ds2;  // пин PIN_DS18B20_2

extern DHT dht1;
extern DHT dht2;

void get1ds18();
void get2ds18();
void getdht1();
void getdht2();
