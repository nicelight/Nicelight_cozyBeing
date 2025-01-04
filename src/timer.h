/*
  пример использования
  Timer eachSec(1000);

  if(eachSec.ready()){
  // по срабатыванию делаем что надо
  }
*/


#pragma once
#include <Arduino.h>
class Timer {
  public:
    Timer(uint32_t nprd = 0) {
      setPeriod(nprd);
    }
    void setPeriod(uint32_t nprd) {
      prd = nprd;
    }
    uint16_t getPeriod() {
      return prd;
    }
    bool ready() {
      if (millis() - tmr >= prd) {
        tmr = millis();
        return true;
      }
      return false;
    }
    void rst() {
      tmr = millis();
    }
    void force() {
      tmr = millis()-prd;
    }
  private:
    uint32_t tmr = 0;
    uint32_t prd = 0;
};
