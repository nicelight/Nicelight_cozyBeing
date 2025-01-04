#pragma once
#include <Arduino.h>

#include "nastroyki.h"

// тут мы говорим что есть такие объекты и они доступны для других функций,
//  а сами объекты определяем уже в userTimers.cpp файле
extern byte releFertiProcess; 



void hueToRGB(uint8_t hue, uint16_t brightness);
void init_pins();
void read_t1_from_db();
void userDhtRelays();
void userDSRelays();
void userNatureTimer();
void userSixTimers();
void userFertiTimer();



// sensorsProbe