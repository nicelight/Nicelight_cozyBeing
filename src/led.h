/*
  пример использования
  LED indikator(LED1_PIN, 1000 3, 50, 20); //каждые 1000 милисек мигаем 3 раза каждых 50 мс, время горения 20 мсек
  indikator.setPeriod(1000, 2, 200, 50); //раз в  секунду два раза взмигнем - по 200 милисек, гореть будем 50 милисек
  indikator.blink(); // in loop
*/



// мигание с временем периода и временем отработки
#pragma once
#include <Arduino.h>
#include "timer.h"
class LED {
  public:
    LED (byte pin, int all, byte cnt, int period, int duratn) : _pin(pin) {
      pinMode(_pin, OUTPUT);
      tmrAll.setPeriod(all);
      tmr = millis();
      prdBlink = period;
      prdOn = period - duratn;
      counter = cnt;
      tmrAll.force();
    }

    void setPeriod(uint16_t al, byte cn, uint16_t prd, uint16_t dur) {
      tmrAll.setPeriod(al);
      tmr = millis();
      prdBlink = prd;
      prdOn = prd - dur;
      counter = cn;
      tmrAll.force();
    }
    void tick() {
      if (tmrAll.ready()) { // по прохождению периода взводим мигалку на режим работы
        counterCur = counter;
        digitalWrite(_pin, 0);
        tmr = millis();
      }
      // пока еще работаем
      if (counterCur) {
        //ждем  сработку таймера и тушим
        if (millis() - tmr >= prdOn) {
          digitalWrite(_pin, 1);
        }
        if (millis() - tmr >= prdBlink) {
          tmr = millis();
          digitalWrite(_pin, 0);
          counterCur--;
        }
      }// counter
    }//tick
  private:
    const byte _pin;
    uint32_t tmr = 0;
    uint32_t prdBlink = 0;
    uint32_t prdOn = 0;
    byte counter;
    byte counterCur;
    Timer tmrAll;
    Timer tmrPer;
    Timer tmrDur;
};




/*
  пример использования
  LED led1(LED1_PIN, 500, 100); //раз в 500 милисек загорается на 100 милисек
  // изменить мигалку
  led1.setPeriod(500, 100); //раз в 500 милисек загорается на 100 милисек
  led1.blink(); // in loop
*/

//// мигание с временем периода и временем отработки
//#pragma once
//#include <Arduino.h>
//#include "timer.h"
//class LED {
//  public:
//    LED (byte pin, int period, int duratn) : _pin(pin) {
//      pinMode(_pin, OUTPUT);
//      tmrPer.setPeriod(period);
//      tmrDur.setPeriod(duratn);
//    }
//
//    void setPeriod(uint16_t prd, uint16_t dur) {
//      tmrPer.setPeriod(prd);
//      tmrDur.setPeriod(dur);
//    }
//    void tick() {
//      if (tmrPer.ready()) {
//        digitalWrite(_pin, 1);
//        flagOn = 1;
//      }
//      if (!flagOn) {  // пока светодиод не горит,
//        tmrDur.rst(); //таймер возжигания обнуляется
//      }
//      else { // если уже загорелся, ждем  сработку таймера и тушим
//        if (tmrDur.ready()) {
//          digitalWrite(_pin, 0);
//          flagOn = 0;
//        }
//      }
//    }//blink
//  private:
//    const byte _pin;
//    bool flag, flagOn = 0;
//    Timer tmrPer;
//    Timer tmrDur;
//};


/*
  //старый простой вариант реализации
  #pragma once
  #include <Arduino.h>
  #include "timer.h"
  class LED {
  public:
    LED (byte pin, int period) : _pin(pin) {
      pinMode(_pin, OUTPUT);
      tmr.setPeriod(period);
    }

    void setPeriod(uint16_t prd) {
      tmr.setPeriod(prd);
    }
    void blink() {
      if (tmr.ready()) {
        digitalWrite(_pin, flag);
        flag = !flag;
      }
    }
  private:
    const byte _pin;
    bool flag;
    Timer tmr;
  };
*/
