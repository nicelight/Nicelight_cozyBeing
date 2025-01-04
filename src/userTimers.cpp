#include "userTimers.h"

#include <Arduino.h>

#include "data.h"
#include "nastroyki.h"
#include "sensors.h"
#include "settings.h"

static uint16_t RED_PWM, GREEN_PWM, BLUE_PWM;  // Значения компонентов Red, Green, Blue (0–4095)
static bool t6_rightDay = 0;
static uint16_t brightness = 4095;  // Максимальная яркость (4095 для 12-битного PWM)
static byte t1fase = 0;             // автомат состояний каждой фазы природного освещения
// static bool data.t1isWorks = 0;                     // прямо сейчас работает
static uint32_t t1fase_prevSeconds = 0;  // секунденый таймер для отрисовки природного освещения
static byte curr_sunrise_dim = 0;        // шаги яркости при рассвете
static byte curr_sunset_dim = 0;         // шаги яркости при закате

byte releFertiProcess = 0;  // автомат подачи дозы удобрений

void init_pins() {
    pinMode(DHT1RELAY, OUTPUT);
    digitalWrite(DHT1RELAY, OFF);
    pinMode(DHT2RELAY, OUTPUT);
    digitalWrite(DHT2RELAY, OFF);

    pinMode(RELE_1, OUTPUT);
    digitalWrite(RELE_1, OFF);
    delay(50);  // чтоб не перегружать блоки питания и контакты
    pinMode(RELE_2, OUTPUT);
    digitalWrite(RELE_2, OFF);
    delay(50);
    pinMode(RELE_3, OUTPUT);
    digitalWrite(RELE_3, OFF);
    delay(50);
    pinMode(RELE_4, OUTPUT);
    digitalWrite(RELE_4, OFF);
    delay(50);
    pinMode(RELE_5, OUTPUT);
    digitalWrite(RELE_5, OFF);
    delay(50);
    pinMode(RELE_6, OUTPUT);
    digitalWrite(RELE_6, OFF);
    delay(50);
    pinMode(DS_1_RELAY, OUTPUT);
    digitalWrite(DS_1_RELAY, OFF);
    delay(50);
    pinMode(DS_2_RELAY, OUTPUT);
    digitalWrite(DS_2_RELAY, OFF);
    
    pinMode(BTN, INPUT_PULLUP);

    // настройка ШИМ
    ledcSetup(RED_PWM_CHANNEL, 12000, 12);    // Канал 0
    ledcSetup(GREEN_PWM_CHANNEL, 12000, 12);  // Канал 1
    ledcSetup(BLUE_PWM_CHANNEL, 12000, 12);   // Канал 2

    // Привязка пинов к каналам
    ledcAttachPin(RED_RGB_LENTA, RED_PWM_CHANNEL);      // Пин 0 к каналу 0
    ledcAttachPin(GREEN_RGB_LENTA, GREEN_PWM_CHANNEL);  // Пин 2 к каналу 1
    ledcAttachPin(BLUE_RGB_LENTA, BLUE_PWM_CHANNEL);    // Пин 4 к каналу 2

    dht1.begin();  // датчик dht инициализируем
    dht2.begin();  // датчик dht инициализируем
    ds1.requestTemp();  // первый запрос на измерение DS18B20 1
    ds2.requestTemp();  // первый запрос на измерение DS18B20 2

    // тесты 
    // data.dsOne.t = 5; 
}  // init_pins()
//
//
//

// ЧИТАЕМ ИЗ БАЗЫ В RAM data.xxx
void read_t1_from_db() {
    // рассчет времени шага увеличения яркости при рассвете (в сек)
    if (!db[kk::t1f2_dim].toInt()) db[kk::t1f2_dim] = 1ul;
    data.t1Sunrise_step = (db[kk::t1f2_startTime].toInt() - db[t1f1_startTime].toInt()) / db[kk::t1f2_dim].toInt();
    if (!data.t1Sunrise_step) data.t1Sunrise_step = 1;  // чтобы не было нулем, если меньше единицы
    data.t1f1_time = db[kk::t1f1_startTime].toInt();    // рассвет с этого времени
    data.t1f2_time = db[kk::t1f2_startTime].toInt();    // утро с этого времени
    data.t1f3_time = db[kk::t1f3_startTime].toInt();    // обед с
    data.t1f4_time = db[kk::t1f4_startTime].toInt();    // ужин с
    data.t1f5_time = db[kk::t1f5_startTime].toInt();    // закат с
    data.t1f6_time = db[kk::t1_stopTime].toInt();       // полная тьма
    data.t1f2_dim = db[kk::t1f2_dim].toInt();           // яркость утром
    data.t1f3_dim = db[kk::t1f3_dim].toInt();           // яркость в обед
    data.t1f4_dim = db[kk::t1f4_dim].toInt();           // яркость вечером
    // шаг уменьшения яркости при закате (в секундах)
    if (!db[kk::t1f4_dim].toInt()) db[kk::t1f4_dim] = 1ul;
    data.t1Sunset_step = (db[kk::t1_stopTime].toInt() - db[t1f5_startTime].toInt()) / db[kk::t1f4_dim].toInt();
    if (!data.t1Sunset_step) data.t1Sunset_step = 1;  // чтобы не было нулем, если меньше единицы
}  // read_t1_from_db()
//
//
//

void userDhtRelays() {
    // === термореле DHT1 для охлаждения воздуха
    switch (data.dhtOne.State) {
        // инициализация
        //  ползунок включен - отрабатываем
        // выключен и включено реле - уйдем на выключение
        case 0:
            //            // if (data.dht1TempRele_enbl != 0) {
            if (db[kk::dht1TempRele_enabled].toInt() != 0) {
                data.dhtOne.State = 5;
            } else if (data.dhtOne.Rel_on) {
                data.dhtOne.State = 20;  // выключим по перемещению ползунка в OFF
            }
            break;
        case 5:  // ожидание превышения теспературы
            if (data.dhtOne.tx10 >= data.dhtOne.tTrigx10) {
                data.dhtOne.State = 10;
            }
            break;
        case 10:  // включаем охлаждение
            digitalWrite(DHT1RELAY, ON);
            data.dhtOne.Rel_on = true;
            data.dhtOne.State = 15;
            break;
        case 15:  // ожидаем понижения температуры + трешхолд
            if (data.dhtOne.tx10 <= data.dhtOne.tTrigx10 - data.dhtOne.tTreshold) {
                data.dhtOne.State = 20;
            }
            break;
        case 20:  // используется при переключении ползунка в морде
            digitalWrite(DHT1RELAY, OFF);
            data.dhtOne.Rel_on = false;
            data.dhtOne.State = 0;
            break;
    }  // switch (dht1State)
    //
    // === термореле DHT2 для увлажнения воздуха
    switch (data.dhtTwo.State) {
        // инициализация
        //  ползунок включен - отрабатываем
        // выключен и включено реле - уйдем на выключение
        case 0:
            //            // if (data.dht1TempRele_enbl != 0) {
            if (db[kk::dht2HumRele_enabled].toInt() != 0) {
                data.dhtTwo.State = 5;
            } else if (data.dhtTwo.Rel_on) {
                data.dhtTwo.State = 20;  // выключим по перемещению ползунка в OFF
            }
            break;
        case 5:  // ожидание понижения влажности
            if (data.dhtTwo.hum <= data.dhtTwo.hTrig) {
                data.dhtTwo.State = 10;
            }
            break;
        case 10:  // включаем охлаждение
            digitalWrite(DHT2RELAY, ON);
            data.dhtTwo.Rel_on = true;
            data.dhtTwo.State = 15;
            break;
        case 15:  // ожидаем повышения влажности + трешхолд
            if (data.dhtTwo.hum >= data.dhtTwo.hTrig + data.dhtTwo.hTreshold) {
                data.dhtTwo.State = 20;
            }
            break;
        case 20:  // используется при переключении ползунка в морде
            digitalWrite(DHT2RELAY, OFF);
            data.dhtTwo.Rel_on = false;
            data.dhtTwo.State = 0;
            break;
    }  // switch (dhtTwo.State)
}  // userDhtRelays()
//
//
//
void userDSRelays() {
    // === термореле DS18B20_1 для охлаждения воды\почвы
    switch (data.dsOne.State) {
        // инициализация
        //  ползунок включен - отрабатываем
        // выключен и включено реле - уйдем на выключение
        case 0:
            if (db[kk::DS1Rele_enabled].toInt() != 0) {
                data.dsOne.State = 5;
            } else if (data.dsOne.rel_on) {
                data.dsOne.State = 20;  // выключим по перемещению ползунка в OFF
            }
            break;
        case 5:  // ожидание превышения теспературы
            if (data.dsOne.tx10 >= data.dsOne.tTrigx10) {
                data.dsOne.State = 10;
            }
            break;
        case 10:  // включаем охлаждение
            digitalWrite(DS_1_RELAY, ON);
            data.dsOne.rel_on = true;
            data.dsOne.State = 15;
            break;
        case 15:  // ожидаем понижения температуры + трешхолд
            if (data.dsOne.tx10 <= data.dsOne.tTrigx10 - data.dsOne.tTreshold) {
                data.dsOne.State = 20;
            }
            break;
        case 20:  // используется при переключении ползунка в морде
            digitalWrite(DS_1_RELAY, OFF);
            data.dsOne.rel_on = false;
            data.dsOne.State = 0;
            break;
    }  // switch (ds1State)
    //
    // === термореле DS18B20 2 для нагрева воды\почвы
    switch (data.dsTwo.State) {
        // инициализация
        //  ползунок включен - отрабатываем
        // выключен и включено реле - уйдем на выключение
        case 0:
            if (db[kk::DS2Rele_enabled].toInt() != 0) {
                data.dsTwo.State = 5;
            } else if (data.dsTwo.rel_on) {
                data.dsTwo.State = 20;  // выключим по перемещению ползунка в OFF
            }
            break;
        case 5:  // ожидание понижения теспературы
            if (data.dsTwo.tx10 <= data.dsTwo.tTrigx10) {
                data.dsTwo.State = 10;
            }
            break;
        case 10:  // включаем нагрев
            digitalWrite(DS_2_RELAY, ON);
            data.dsTwo.rel_on = true;
            data.dsTwo.State = 15;
            break;
        case 15:  // ожидаем повышение температуры + трешхолд
            if (data.dsTwo.tx10 >= data.dsTwo.tTrigx10 + data.dsTwo.tTreshold) {
                data.dsTwo.State = 20;
            }
            break;
        case 20:  // так же используется при переключении ползунка в морде
            digitalWrite(DS_2_RELAY, OFF);
            data.dsTwo.rel_on = false;
            data.dsTwo.State = 0;
            break;
    }  // switch (dsTwo.State)
    //
}  // userDSRelays()
//
//
//

void userSixTimers() {
    // таймер 1 ===
    // === таймер Реле 1
    // if (db[kk::t1Discr_enabled].toBool()) {
    if (data.t1discr_enbl) {
        if (db[kk::t1Discr_startTime].toInt() < db[kk::t1Discr_endTime].toInt())  // если нет перехода через полночь
        {
            if ((db[kk::t1Discr_startTime].toInt() <= data.secondsNow) && (data.secondsNow <= db[kk::t1Discr_endTime].toInt())) {
                if (!data.rel1_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_1, ON);
                    data.rel1_on = 1;
                }
            } else {
                if (data.rel1_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_1, OFF);
                    data.rel1_on = 0;
                }
            }
        } else if (db[kk::t1Discr_startTime].toInt() > db[kk::t1Discr_endTime].toInt())  // если есть переход через полночь
        {
            if ((db[kk::t1Discr_startTime].toInt() >= data.secondsNow) && (data.secondsNow >= db[kk::t1Discr_endTime].toInt())) {
                if (data.rel1_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_1, OFF);
                    data.rel1_on = 0;
                }
            } else {
                if (!data.rel1_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_1, ON);
                    data.rel1_on = 1;
                }
            }
        }
    } else {
        if (data.rel1_on)  // если было включено, выключим
        {
            digitalWrite(RELE_1, OFF);
            data.rel1_on = 0;
        }
    }
    // таймер 2 ===
    //=== таймер Реле 2
    // if (db[kk::t2Discr_enabled].toBool()) {
    if (data.t2discr_enbl) {
        if (db[kk::t2Discr_startTime].toInt() < db[kk::t2Discr_endTime].toInt())  // если нет перехода через полночь
        {
            if ((db[kk::t2Discr_startTime].toInt() <= data.secondsNow) && (data.secondsNow <= db[kk::t2Discr_endTime].toInt())) {
                if (!data.rel2_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_2, ON);
                    data.rel2_on = 1;
                }
            } else {
                if (data.rel2_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_2, OFF);
                    data.rel2_on = 0;
                }
            }
        } else if (db[kk::t2Discr_startTime].toInt() > db[kk::t2Discr_endTime].toInt())  // если есть переход через полночь
        {
            if ((db[kk::t2Discr_startTime].toInt() >= data.secondsNow) && (data.secondsNow >= db[kk::t2Discr_endTime].toInt())) {
                if (data.rel2_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_2, OFF);
                    data.rel2_on = 0;
                }
            } else {
                if (!data.rel2_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_2, ON);
                    data.rel2_on = 1;
                }
            }
        }
    } else {
        if (data.rel2_on)  // если было включено, выключим
        {
            digitalWrite(RELE_2, OFF);
            data.rel2_on = 0;
        }
    }
    // таймер 3 ===
    //=== таймер Реле 3
    // if (db[kk::t3Discr_enabled].toBool()) {
    if (data.t3discr_enbl) {
        if (db[kk::t3Discr_startTime].toInt() < db[kk::t3Discr_endTime].toInt())  // если нет перехода через полночь
        {
            if ((db[kk::t3Discr_startTime].toInt() <= data.secondsNow) && (data.secondsNow <= db[kk::t3Discr_endTime].toInt())) {
                if (!data.rel3_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_3, ON);
                    data.rel3_on = 1;
                }
            } else {
                if (data.rel3_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_3, OFF);
                    data.rel3_on = 0;
                }
            }
        } else if (db[kk::t3Discr_startTime].toInt() > db[kk::t3Discr_endTime].toInt())  // если есть переход через полночь
        {
            if ((db[kk::t3Discr_startTime].toInt() >= data.secondsNow) && (data.secondsNow >= db[kk::t3Discr_endTime].toInt())) {
                if (data.rel3_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_3, OFF);
                    data.rel3_on = 0;
                }
            } else {
                if (!data.rel3_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_3, ON);
                    data.rel3_on = 1;
                }
            }
        }
    } else {
        if (data.rel3_on)  // если было включено, выключим
        {
            digitalWrite(RELE_3, OFF);
            data.rel3_on = 0;
        }
    }
    // таймер 4 ===
    //=== таймер Реле4
    // if (db[kk::t4Discr_enabled].toBool()) {
    if (data.t4discr_enbl) {
        if (db[kk::t4Discr_startTime].toInt() < db[kk::t4Discr_endTime].toInt())  // если нет перехода через полночь
        {
            if ((db[kk::t4Discr_startTime].toInt() <= data.secondsNow) && (data.secondsNow <= db[kk::t4Discr_endTime].toInt())) {
                if (!data.rel4_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_4, ON);
                    data.rel4_on = 1;
                }
            } else {
                if (data.rel4_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_4, OFF);
                    data.rel4_on = 0;
                }
            }
        } else if (db[kk::t4Discr_startTime].toInt() > db[kk::t4Discr_endTime].toInt())  // если есть переход через полночь
        {
            if ((db[kk::t4Discr_startTime].toInt() >= data.secondsNow) && (data.secondsNow >= db[kk::t4Discr_endTime].toInt())) {
                if (data.rel4_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_4, OFF);
                    data.rel4_on = 0;
                }
            } else {
                if (!data.rel4_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_4, ON);
                    data.rel4_on = 1;
                }
            }
        }
    } else {
        if (data.rel4_on)  // если было включено, выключим
        {
            data.rel4_on = 0;
            digitalWrite(RELE_4, OFF);
        }
    }
    // таймер 5===
    //=== таймер Реле 5
    // if (db[kk::t5Discr_enabled].toBool()) {
    if (data.t5discr_enbl) {
        if (db[kk::t5Discr_startTime].toInt() < db[kk::t5Discr_endTime].toInt())  // если нет перехода через полночь
        {
            if ((db[kk::t5Discr_startTime].toInt() <= data.secondsNow) && (data.secondsNow <= db[kk::t5Discr_endTime].toInt())) {
                if (!data.rel5_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_5, ON);
                    data.rel5_on = 1;
                }
            } else {
                if (data.rel5_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_5, OFF);
                    data.rel5_on = 0;
                }
            }
        } else if (db[kk::t5Discr_startTime].toInt() > db[kk::t5Discr_endTime].toInt())  // если есть переход через полночь
        {
            if ((db[kk::t5Discr_startTime].toInt() >= data.secondsNow) && (data.secondsNow >= db[kk::t5Discr_endTime].toInt())) {
                if (data.rel5_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_5, OFF);
                    data.rel5_on = 0;
                }
            } else {
                if (!data.rel5_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_5, ON);
                    data.rel5_on = 1;
                }
            }
        }
    } else {
        if (data.rel5_on)  // если было включено, выключим
        {
            digitalWrite(RELE_5, OFF);
            data.rel5_on = 0;
        }
    }

    // таймер 6===
    // проверяем, правильный ли день для включения таймера 6
    switch (curDataTime.weekDay) {
        case 1:
            if (db[kk::t6Discr_inMonday].toInt() == 1) t6_rightDay = 1;
            else t6_rightDay = 0;
            break;
        case 2:
            if (db[kk::t6Discr_inTuesday].toInt() == 1) t6_rightDay = 1;
            else t6_rightDay = 0;
            break;
        case 3:
            if (db[kk::t6Discr_inWensday].toInt() == 1) t6_rightDay = 1;
            else t6_rightDay = 0;
            break;
        case 4:
            if (db[kk::t6Discr_inThursday].toInt() == 1) t6_rightDay = 1;
            else t6_rightDay = 0;
            break;
        case 5:
            if (db[kk::t6Discr_inFriday].toInt() == 1) t6_rightDay = 1;
            else t6_rightDay = 0;
            break;
        case 6:
            if (db[kk::t6Discr_inSaturday].toInt() == 1) t6_rightDay = 1;
            else t6_rightDay = 0;
            break;
        case 7:
            if (db[kk::t6Discr_inSunday].toInt() == 1) t6_rightDay = 1;
            else t6_rightDay = 0;
            break;
    }
    //=== таймер Реле 6
    // if (db[kk::t6Discr_enabled].toBool()) {
    if (data.t6discr_enbl && t6_rightDay) {
        if (db[kk::t6Discr_startTime].toInt() < db[kk::t6Discr_endTime].toInt())  // если нет перехода через полночь
        {
            if ((db[kk::t6Discr_startTime].toInt() <= data.secondsNow) && (data.secondsNow <= db[kk::t6Discr_endTime].toInt())) {
                if (!data.rel6_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_6, ON);
                    data.rel6_on = 1;
                }
            } else {
                if (data.rel6_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_6, OFF);
                    data.rel6_on = 0;
                }
            }
        } else if (db[kk::t6Discr_startTime].toInt() > db[kk::t6Discr_endTime].toInt())  // если есть переход через полночь
        {
            if ((db[kk::t6Discr_startTime].toInt() >= data.secondsNow) && (data.secondsNow >= db[kk::t6Discr_endTime].toInt())) {
                if (data.rel6_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_6, OFF);
                    data.rel6_on = 0;
                }
            } else {
                if (!data.rel6_on)  // avoid extra digWrite
                {
                    digitalWrite(RELE_6, ON);
                    data.rel6_on = 1;
                }
            }
        }
    } else {
        if (data.rel6_on)  // если было включено, выключим
        {
            digitalWrite(RELE_6, OFF);
            data.rel6_on = 0;
        }
    }
    // таймер6 ===

}  // userSixTimers()
//
//
//

void userNatureTimer() {  //     // Природное освещение
    // нажали кнопку "Утвердить" в настройках природного освещения, или первый старт
    if (data.timer_nature_applied) {
        data.timer_nature_applied = 0;
        data.natureSec = data.secondsNow - 1;  // ускоряем срабатывание отрисовки света

        // проверка и корректировка настроек времен
        if (db[kk::t1f1_startTime].toInt() > 75600ul) db[kk::t1f1_startTime] = 28800ul;  // нельзя ставить поздний рассвет
        // каждая последующая фаза на 1мин  позже предыдущей
        if (db[kk::t1f2_startTime].toInt() <= db[kk::t1f1_startTime].toInt()) db[kk::t1f2_startTime] = db[kk::t1f1_startTime].toInt() + 60;
        if (db[kk::t1f3_startTime].toInt() <= db[kk::t1f2_startTime].toInt()) db[kk::t1f3_startTime] = db[kk::t1f2_startTime].toInt() + 60;
        if (db[kk::t1f4_startTime].toInt() <= db[kk::t1f3_startTime].toInt()) db[kk::t1f4_startTime] = db[kk::t1f3_startTime].toInt() + 60;
        if (db[kk::t1f5_startTime].toInt() <= db[kk::t1f4_startTime].toInt()) db[kk::t1f5_startTime] = db[kk::t1f4_startTime].toInt() + 60;
        if (db[kk::t1_stopTime].toInt() <= db[kk::t1f5_startTime].toInt()) db[kk::t1_stopTime] = db[kk::t1f5_startTime].toInt() + 60;
        // если яркость
        if (!db[kk::t1f2_dim].toInt()) db[kk::t1f2_dim] = 1;
        if (!db[kk::t1f3_dim].toInt()) db[kk::t1f3_dim] = 1;
        if (!db[kk::t1f4_dim].toInt()) db[kk::t1f4_dim] = 1;
        read_t1_from_db();  // прочитаем из базы в data.xxx все эти значения для природного освещения
        curr_sunrise_dim = 0;
        curr_sunset_dim = data.t1f4_dim;
    }  // data.timer_nature_applied

    // если включили работу таймера, каждую секунду отрисовываем
    if (data.t1f_enbl && (data.natureSec != data.secondsNow)) {
        data.natureSec = data.secondsNow;
        // if (db[kk::t1f_enabled]) { //заглючивает что то , не верно читается
#ifdef CHECKT1
        Serial.print("\n\t\tNATURE ENABLED\n");
#endif  // проверим в какой фазе мы сейчас
        // если не в ожидании и время за пределами работы света
        if (t1fase && ((data.secondsNow >= data.t1f6_time) || (data.secondsNow < data.t1f1_time))) {
            t1fase = 70;  //// автомат --- тушим и идем на ожидание
        }
        // рассвет
        if (((data.secondsNow >= data.t1f1_time) && (data.secondsNow < data.t1f2_time))) {
            t1fase = 10;
            // data.t1isWorks = 1;
        }
        // утро
        else if (((data.secondsNow >= data.t1f2_time) && (data.secondsNow < data.t1f3_time))) {
            t1fase = 20;
            // data.t1isWorks = 1;
        }
        // обед
        else if (((data.secondsNow >= data.t1f3_time) && (data.secondsNow < data.t1f4_time))) {
            t1fase = 30;
            // data.t1isWorks = 1;
        }
        // вечер
        else if (((data.secondsNow >= data.t1f4_time) && (data.secondsNow < data.t1f5_time))) {
            t1fase = 40;
            // data.t1isWorks = 1;
        }
        // закат
        else if (((data.secondsNow >= data.t1f5_time) && (data.secondsNow < data.t1f6_time))) {
            t1fase = 50;
            // Serial.println("Z A K A T ");
            // data.t1isWorks = 1;
        }
#ifdef DEBUGNATURELIGHT
        static byte prevFase = 100;
        if (prevFase != t1fase) {
            prevFase = t1fase;
            Serial.print("\n\tt1fase: ");
            Serial.print(t1fase);
            Serial.print("\t\t");
        }
#endif

        // автомат состояний природного освещения
        switch (t1fase) {
            case 0:
                curr_sunrise_dim = 0;
                curr_sunset_dim = data.t1f4_dim;
                t1fase_prevSeconds = data.secondsNow - 1000ul;  // ускорение первого шага
                break;
            case 10:  // рассвет
                // пока яркость не на максимуме
                if (curr_sunrise_dim < data.t1f2_dim) {
// ждем время инкремента яркости,  увеличиваем яркость на 1 позицию
#ifdef DEBUGNATURELIGHT
                    Serial.print("\nrise_dim: ");
                    Serial.print(curr_sunrise_dim);
                    Serial.print("\tsecNow: ");
                    Serial.print(data.secondsNow);
                    Serial.print("\tprevsec: ");
                    Serial.print(t1fase_prevSeconds);
                    Serial.print("\tstep: ");
                    Serial.print(data.t1Sunrise_step);
                    Serial.print("\tdata.secondsNow - t1fase_prevSeconds: ");
                    Serial.print(data.secondsNow - t1fase_prevSeconds);
#endif
                    if ((data.secondsNow - t1fase_prevSeconds) >= data.t1Sunrise_step) {
                        t1fase_prevSeconds = data.secondsNow;
                        curr_sunrise_dim++;
#ifdef DEBUG
                        Serial.print("\n1rise_dim: ");
                        Serial.print(curr_sunrise_dim);
                        Serial.print("\tnext in ");
                        Serial.print(data.t1Sunrise_step);
                        Serial.print(" sec..");
#endif
                        ledcWrite(RED_PWM_CHANNEL, brightn[curr_sunrise_dim]);
                        ledcWrite(GREEN_PWM_CHANNEL, brightn[curr_sunrise_dim >> 1]);
                        ledcWrite(BLUE_PWM_CHANNEL, brightn[curr_sunrise_dim >> 3]);
                        // ledcWrite(BLUE_PWM_CHANNEL, 0);

                    }  // if ms
                }  // step not max
                break;
            case 20:  // утро
#ifdef DEBUGNATURELIGHT
                Serial.print("\n1f2dim: ");
                Serial.print(data.t1f2_dim);
                Serial.print("\tbright: ");
                Serial.print(brightn[data.t1f2_dim]);
#endif
                ledcWrite(RED_PWM_CHANNEL, brightn[data.t1f2_dim]);
                ledcWrite(GREEN_PWM_CHANNEL, brightn[data.t1f2_dim]);
                ledcWrite(BLUE_PWM_CHANNEL, brightn[data.t1f2_dim]);
                break;
            case 30:  // обед
#ifdef DEBUG
                Serial.print("\n1f3dim: ");
                Serial.print(data.t1f3_dim);
                Serial.print("\tbright: ");
                Serial.print(brightn[data.t1f3_dim]);
#endif
                ledcWrite(RED_PWM_CHANNEL, brightn[data.t1f3_dim]);
                ledcWrite(GREEN_PWM_CHANNEL, brightn[data.t1f3_dim]);
                ledcWrite(BLUE_PWM_CHANNEL, brightn[data.t1f3_dim]);
                break;
            case 40:  //  вечер
#ifdef DEBUGNATURELIGHT
                Serial.print("\n1f4dim: ");
                Serial.print(data.t1f4_dim);
                Serial.print("\tbright: ");
                Serial.print(brightn[data.t1f4_dim]);
#endif
                ledcWrite(RED_PWM_CHANNEL, brightn[data.t1f4_dim]);
                ledcWrite(GREEN_PWM_CHANNEL, brightn[data.t1f4_dim]);
                ledcWrite(BLUE_PWM_CHANNEL, brightn[data.t1f4_dim]);
                curr_sunset_dim = data.t1f4_dim;
                break;
            case 50:  // закат
                // пока яркость не на минимуме
                if (curr_sunset_dim) {
#ifdef DEBUGNATURELIGHT
                    Serial.print("\nset_dim: ");
                    Serial.print(curr_sunset_dim);
                    Serial.print("\tsecNow: ");
                    Serial.print(data.secondsNow);
                    Serial.print("\tprevsec: ");
                    Serial.print(t1fase_prevSeconds);
                    Serial.print("\tstep: ");
                    Serial.print(data.t1Sunset_step);
                    Serial.print("\tdata.secondsNow - t1fase_prevSeconds: ");
                    Serial.print(data.secondsNow - t1fase_prevSeconds);
#endif
                    // ждем время декремента яркости,  увеличиваем яркость на 1 позицию
                    if ((data.secondsNow - t1fase_prevSeconds) >= data.t1Sunset_step) {
                        t1fase_prevSeconds = data.secondsNow;
                        curr_sunset_dim--;
#ifdef DEBUGNATURELIGHT
                        Serial.print("\nsunset_dim: ");
                        Serial.print(curr_sunset_dim);
                        Serial.print("\tnext step in: ");
                        Serial.print(data.t1Sunset_step);
                        Serial.print(" sec..");
#endif
                        ledcWrite(RED_PWM_CHANNEL, brightn[curr_sunset_dim]);
                        ledcWrite(GREEN_PWM_CHANNEL, brightn[curr_sunset_dim >> 1]);
                        ledcWrite(BLUE_PWM_CHANNEL, brightn[curr_sunset_dim >> 3]);
                        // ledcWrite(BLUE_PWM_CHANNEL, 0);

                    }  // if ms
                }  // step not max
                else {
                    t1fase = 70;
                }
                break;

            case 70:  // тушим свет, и на исходную
                // data.t1isWorks = 0;
                ledcWrite(RED_PWM_CHANNEL, 0);
                ledcWrite(GREEN_PWM_CHANNEL, 0);
                ledcWrite(BLUE_PWM_CHANNEL, 0);
                t1fase = 0;  // GO  на исходную
                break;
        }  // switch(t1fase)
    }  // timer enabled
    else {
    }
}  // userNatureTimer()
//
//
//

// таймер автодозатора от Аквамена
// https://www.youtube.com/watch?v=XGLbtAAbIi4&feature=youtu.be
void userFertiTimer() {
    // if (data.tFerti_enbl) {
    if (db[kk::aquaDoz1_enabled].toInt()) {
        // Serial.print ("\n db[kk::aquaDoz1_1time].toInt():");
        // Serial.print(db[kk::aquaDoz1_1time].toInt());
        // Serial.print (":");
        // Serial.print (data.secondsNow);
        // Serial.println (" data.secondsNow");

        int32_t untilNextDoze2 = 86340;  // через сколько следующее включение
        // проверяем разницу текущего времени и времени до след дозы
        // сравниваем с разницей из предыдущего вычисления
        //  та которая ниже, сохраняем в data.untilNextDoze
        data.untilNextDoze = db[kk::aquaDoz1_1time].toInt() - data.secondsNow;
        if (!data.untilNextDoze) releFertiProcess = 10;               // ON
        else if (data.untilNextDoze < 0) data.untilNextDoze = 86340;  // 23:59

        untilNextDoze2 = db[kk::aquaDoz1_2time].toInt() - (int)data.secondsNow;
        if (!untilNextDoze2) releFertiProcess = 10;  // ON
        else if (untilNextDoze2 > 0) {               // ищем минимальное время
            if (untilNextDoze2 < data.untilNextDoze)
                data.untilNextDoze = untilNextDoze2;
        }
        if (db[kk::aquaDoz1_need3rd].toInt()) {
            untilNextDoze2 = db[kk::aquaDoz1_3time].toInt() - data.secondsNow;
            if (!untilNextDoze2) releFertiProcess = 10;  // ON
            else if (untilNextDoze2 > 0) {               // ищем минимальное время
                if (untilNextDoze2 < data.untilNextDoze)
                    data.untilNextDoze = untilNextDoze2;
            }
        }
        if (db[kk::aquaDoz1_need4th].toInt()) {
            untilNextDoze2 = db[kk::aquaDoz1_4time].toInt() - data.secondsNow;
            if (!untilNextDoze2) releFertiProcess = 10;  // ON
            else if (untilNextDoze2 > 0) {               // ищем минимальное время
                if (untilNextDoze2 < data.untilNextDoze)
                    data.untilNextDoze = untilNextDoze2;
            }
        }
        if (db[kk::aquaDoz1_need5th].toInt()) {
            untilNextDoze2 = db[kk::aquaDoz1_5time].toInt() - data.secondsNow;
            if (!untilNextDoze2) releFertiProcess = 10;  // ON
            else if (untilNextDoze2 > 0) {               // ищем минимальное время
                if (untilNextDoze2 < data.untilNextDoze)
                    data.untilNextDoze = untilNextDoze2;
            }
        }
        if (db[kk::aquaDoz1_need6th].toInt()) {
            untilNextDoze2 = db[kk::aquaDoz1_6time].toInt() - data.secondsNow;
            if (!untilNextDoze2) releFertiProcess = 10;  // ON
            else if (untilNextDoze2 > 0) {               // ищем минимальное время
                if (untilNextDoze2 < data.untilNextDoze)
                    data.untilNextDoze = untilNextDoze2;
            }
        }
        if (db[kk::aquaDoz1_need7th].toInt()) {
            untilNextDoze2 = db[kk::aquaDoz1_7time].toInt() - data.secondsNow;
            if (!untilNextDoze2) releFertiProcess = 10;  // ON
            else if (untilNextDoze2 > 0) {               // ищем минимальное время
                if (untilNextDoze2 < data.untilNextDoze)
                    data.untilNextDoze = untilNextDoze2;
            }
        }
        if (db[kk::aquaDoz1_need8th].toInt()) {
            untilNextDoze2 = db[kk::aquaDoz1_8time].toInt() - data.secondsNow;
            if (!untilNextDoze2) releFertiProcess = 10;  // ON
            else if (untilNextDoze2 > 0) {               // ищем минимальное время
                if (untilNextDoze2 < data.untilNextDoze)
                    data.untilNextDoze = untilNextDoze2;
            }
        }

    } else {
        if (data.relFerti_on)       // если было включено, выключим
            releFertiProcess = 30;  // OFF
    }
    // Serial.print("\nreleFertiProcess: ");
    // Serial.println(releFertiProcess);

    switch (releFertiProcess) {
        case 0:
            // data.relefertiSec = data.secondsNow;
            break;
        case 10:
            digitalWrite(RELE_FERTILIZER, ON);
            data.relefertiSec = data.secondsNow;
            data.relFerti_on = 1;
            releFertiProcess = 20;
            // break;
        case 20:
            Serial.print("\n db[kk::aquaDoze1_dozeTime].toInt():");
            Serial.print(db[kk::aquaDoze1_dozeTime].toInt());
            Serial.print(" : ");
            Serial.print(data.secondsNow - data.relefertiSec);
            Serial.print(":Now-fert");
            Serial.print("\t\tsNow:");
            Serial.print(data.secondsNow);
            Serial.print("\tfertiSec:");
            Serial.println(data.relefertiSec);
            if ((data.secondsNow - data.relefertiSec) >= db[kk::aquaDoze1_dozeTime].toInt()) {
                releFertiProcess = 30;
            }
            break;
        case 30:
            digitalWrite(RELE_FERTILIZER, OFF);
            data.relFerti_on = 0;
            releFertiProcess = 0;
            break;
    }  // switch
}  // userFertiTimer()
