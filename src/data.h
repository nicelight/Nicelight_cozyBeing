/* как пользоваться кошерно данными
читаем тут
 https://alexgyver.ru/docs/program-flow/cpp-files/#9-toc-title
пункт Составные типы

вот пример организации файлов
// data.h
#pragma once
struct Data {};
extern Data data;

// data.cpp
#include "data.h"
Data data;

//main.cpp
#include "data.h"
// переменная data уже видна тут
*/

#pragma once
#include <Arduino.h>

// подструктруа структуры Data
struct Ds18b20_sensor {
    int16_t tx10 = 0;       // температура переведенная из флоат в интежер * 10
    int16_t tTrigx10 = 0;   // температура срабатывания реле
    int16_t tTreshold = 0;  // температура гистерезиса отключения реле
    float tfloat = -80.0;
    bool rel_on = false;  // флаг включения реле
    byte State = 0;       // автомат состояний работы реле
};


    // int16_t hdht2 = -80;  // темп, влажность первого DHT22
    // int16_t hdht2Min = 0;
    // int16_t dht2Treshold = 0;
    // float floattdht2 = -80.0;
    // bool dht2Rel_on = false;
    // byte dht2State = 0;  // автомат работы реле


struct DHTxx_sensor {
    int16_t hum = -80;  // влажность DHT22
    int16_t tx10 = -80;  // температура DHT22 интежер * 10
    int16_t tTrigx10 = 0; // температура сработки реле 
    int16_t hTrig = 0; //влажность сработки реле
    int16_t hTreshold = 0;
    int16_t tTreshold = 0;
    float tfloat = -80.0;
    bool Rel_on = false;
    byte State = 0;  // автомат работы реле
};

struct Data  // обьявляем класс структуры
{
    uint32_t secondsNow = 44000ul;
    uint32_t secondsUptime = 1;
    byte uptime_Days = 0;
    // uint32_t secondsUptime = 86390;
    // byte uptime_Days = 1;

    struct DHTxx_sensor dhtOne;
    struct DHTxx_sensor dhtTwo;

    // int16_t hdht2 = -80;  // темп, влажность первого DHT22
    // int16_t hdht2Min = 0;
    // int16_t dht2Treshold = 0;
    // float floattdht2 = -80.0;
    // bool dht2Rel_on = false;
    // byte dht2State = 0;  // автомат работы реле

    struct Ds18b20_sensor dsOne;
    struct Ds18b20_sensor dsTwo;

    // было в структуре Data, перенесено в подструктуры DS18B20_sensor
    // int16_t temp_ds1x10 = 0;
    // int16_t tempMax_ds1x10 = 0;
    // int16_t temp_ds1Treshold = 0;
    // float floattDS1 = -80.0;  // температура первого DS18B20
    // bool DS1Rel_on = false;
    // byte ds1State = 0;  // автомат работы реле

    // int16_t temp_ds2x10 = 0;
    // int16_t tempMin_ds2x10 = 0;
    // int16_t temp_ds2Treshold = 0;
    // float floattDS2 = -80.0;  // температура второго DS18B20
    // bool DS2Rel_on = false;
    // byte ds2State = 0;  // автомат работы реле

    // чтобы не проверять булевы значения ползунков в интерфейсе из базы, пишем их сюда сначала
    bool t1discr_enbl = 0;
    bool rel1_on = 0;
    bool t2discr_enbl = 0;
    bool rel2_on = 0;
    bool t3discr_enbl = 0;
    bool rel3_on = 0;
    bool t4discr_enbl = 0;
    bool rel4_on = 0;
    bool t5discr_enbl = 0;
    bool rel5_on = 0;
    bool t6discr_enbl = 0;
    bool rel6_on = 0;

    int16_t temp_ds1 = 18;  // датчик температуры

    // природное освещение
    bool t1f_enbl = 0;  // ползунок переключатель в веб интерфесе()
    // bool t1isWorks = 0; // на данный момент Природное светится
    uint32_t natureSec = 0;         // яркость обновлять будем каждую секунду, эта пер. для таймера
    uint32_t t1Sunrise_step = 1;    // шаг рассвета
    uint32_t t1f1_time = 21600;     // рассвет в 6 по дефолту
    uint32_t t1f2_time = 25200;     // утро начинается в 7
    uint32_t t1f3_time = 43200;     // обед в 12
    uint32_t t1f4_time = 64800;     // ужин в 18
    uint32_t t1f5_time = 72000;     // закат в  в 20
    uint32_t t1f6_time = 75600;     // полная тьма в  в 21
    uint32_t t1Sunset_step = 1;     // шаг заката
    uint32_t t1f2_dim = 50;         // яркость утром
    uint32_t t1f3_dim = 80;         // яркость обед
    uint32_t t1f4_dim = 70;         // яркость ужин
    bool timer_nature_applied = 1;  // флаг на обновление  настроек природного освещения

    // аквамен дозатор
    bool relFerti_on = 0;
    uint32_t relefertiSec = 0;
    int32_t untilNextDoze = 86340;  // через сколько следующее включение

    //  из примеров Гайвера
    int number = 123456;
    String label = "label";
    bool led = 1;
    String paragr = "paragraph";
    String input = "input";
    char inputc[20] = "input";
    String pass = "pass";
    uint32_t color = 0xff0000;
    bool sw = true;
    uint32_t datew = 1728123055;
    uint32_t datetime = 1728123055;
    float slider = 33;
    uint8_t sel = 1;
};

// for cpp
extern Data data;  // объявляем что у нас будет переменная data класса Data
extern DHTxx_sensor dhtOne;
extern DHTxx_sensor dhtTwo;

extern Ds18b20_sensor dsOne;
extern Ds18b20_sensor dsTwo;

extern uint16_t brightn[101];  // если есть data.cpp

// constexpr size_t BRIGHT_SIZE = 100;// размер массива
// extern uint16_t brightn[BRIGHT_SIZE];

/*
byte red, green, blue;
unsigned long rgb = 0x6f56a3;

red = rgb >> 16
green = (rgb & 0x00ff00) >> 8;
blue = (rgb & 0x0000ff);

или



String hexstring = "B787B7";
    // long number = (long) strtol( &hexstring[1], NULL, 16);
    long number = (long) strtol( &hexstring[0], NULL, 16);
    int r = number >> 16;
    int g = number >> 8 & 0xFF;
    int b = number & 0xFF;
*/

// bool    t1Discr_enabled,
// uint32_t    t1Discr_startTime,
// uint32_t    t1Discr_endTime,

// bool    t2Discr_enabled,
// uint32_t    t2Discr_startTime,
// uint32_t    t2Discr_endTime,
// bool    t3Discr_enabled,
// uint32_t    t3Discr_startTime,
// uint32_t    t3Discr_endTime,

// bool    t4Discr_enabled,
// uint32_t    t4Discr_startTime,
// uint32_t    t4Discr_endTime,

// bool    t5Discr_enabled,
// uint32_t    t5Discr_startTime,
// uint32_t    t5Discr_endTime,
