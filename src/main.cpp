

// прошивка тут
// C:\Users\Acer\Documents\PlatformIO\Projects\minihub\.pio\build\esp32doit-devkit-v1

#include <Arduino.h>

#include "led.h"
#include "timer.h"
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <GyverDBFile.h>
#include <GyverDS18.h>
#include <GyverNTP.h>
#include <LittleFS.h>
#include <SettingsGyver.h>
#include <WiFiConnector.h>

// #include "driver/temp_sensor.h"
#include "data.h"  // тут лежит структура data по кошерному
#include "nastroyki.h"
#include "sensors.h"
#include "settings.h"
#include "userTimers.h"

// обявление фкнций для их видимости из вкладок.

Timer each5Sec(5000ul);    // таймер раз в 10 сек
Timer each5min(300000ul);  // таймер раз в 5 мин
Timer eachSec(1000ul);     // таймер раз в сек

LED indikator(INDIKATOR, 300, 3, 50, 20);  // каждые 1000 милисек мигаем 3 раза каждых 50 мс, время горения 20 мсек
bool gotWifi = false;                      // если подключено было к сети
int valNum;
uint32_t startSeconds = 0;
uint32_t stopSeconds = 0;
byte initially = 5;        // первых 10 секунд приписываем время в переменную
bool firstSlowSensor = 1;  // опрос датчиков по очереди
void setup() {
    each5min.rst();
    init_pins();

    Serial.begin(115200);
    Serial.println("\n\n\n\t\t\t ESP STARTED !\n\n");

    // ======== SETTINGS ========
    WiFi.mode(WIFI_AP_STA);  // режим AP_STA. Вызываем перед sett.begin(), чтобы settings знал о текущем режиме wifi
    sett.begin();
    sett.onBuild(build);
    sett.onUpdate(update);

    // ======== DATABASE ========
#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif
    db.begin();
    db.init(kk::wifi_ssid, WIFI);
    db.init(kk::wifi_pass, WIFIPASS);
    db.init(kk::ntp_gmt, 5);

    db.init(kk::dht1name, "Имя первого dht22");
    db.init(kk::dht1TempRele_heating, (uint8_t)0);
    db.init(kk::dht1TempRele_cooling, (uint8_t)0);
    db.init(kk::dht1TempRele_startTemp, (uint8_t)26);
    db.init(kk::dht1TempRele_TempThreshold, (uint8_t)1);

    db.init(kk::dht2name, "Имя второго dht22");
    db.init(kk::dht2HumRele_enabled, (uint8_t)0);
    db.init(kk::dht2HumRele_startHum, (uint8_t)30);
    db.init(kk::dht2HumRele_HumThreshold, (uint8_t)1);

    db.init(kk::DS1name, "Имя первого DS18B20");
    db.init(kk::DS1Rele_enabled, (uint8_t)0);
    db.init(kk::DS1Rele_startTemp, (uint8_t)26);
    db.init(kk::DS1Rele_TempThreshold, (uint8_t)1);

    db.init(kk::DS2name, "Имя второго DS18B20");
    db.init(kk::DS2Rele_enabled, (uint8_t)0);
    db.init(kk::DS2Rele_startTemp, (uint8_t)24);
    db.init(kk::DS2Rele_TempThreshold, (uint8_t)1);

    db.init(kk::t1Discr_name, "Реле 1");
    db.init(kk::t1Discr_enabled, (uint8_t)0);
    db.init(kk::t1Discr_startTime, (uint32_t)21600ul);
    db.init(kk::t1Discr_endTime, (uint32_t)72000ul);
    db.init(kk::t2Discr_name, "Реле 2");
    db.init(kk::t2Discr_enabled, (uint8_t)0);
    db.init(kk::t2Discr_startTime, (uint32_t)21600ul);
    db.init(kk::t2Discr_endTime, (uint32_t)72000ul);
    db.init(kk::t3Discr_name, "Реле 3");
    db.init(kk::t3Discr_enabled, (uint8_t)0);
    db.init(kk::t3Discr_startTime, (uint32_t)21600ul);
    db.init(kk::t3Discr_endTime, (uint32_t)72000ul);
    db.init(kk::t4Discr_name, "Реле 4");
    db.init(kk::t4Discr_enabled, (uint8_t)0);
    db.init(kk::t4Discr_startTime, (uint32_t)21600ul);
    db.init(kk::t4Discr_endTime, (uint32_t)72000ul);
    db.init(kk::t5Discr_name, "Реле 5");
    db.init(kk::t5Discr_enabled, (uint8_t)0);
    db.init(kk::t5Discr_startTime, (uint32_t)21600ul);
    db.init(kk::t5Discr_endTime, (uint32_t)72000ul);
    db.init(kk::t6Discr_name, "Реле 6 недельное");
    db.init(kk::t6Discr_enabled, (uint8_t)0);
    db.init(kk::t6Discr_startTime, (uint32_t)21600ul);
    db.init(kk::t6Discr_endTime, (uint32_t)72000ul);
    db.init(kk::t6Discr_inMonday, (uint8_t)0);
    db.init(kk::t6Discr_inTuesday, (uint8_t)0);
    db.init(kk::t6Discr_inWensday, (uint8_t)0);
    db.init(kk::t6Discr_inThursday, (uint8_t)0);
    db.init(kk::t6Discr_inFriday, (uint8_t)0);
    db.init(kk::t6Discr_inSaturday, (uint8_t)0);
    db.init(kk::t6Discr_inSunday, (uint8_t)0);

    db.init(kk::t1f_enabled, (uint8_t)0);
    db.init(kk::t1f1_startTime, (uint32_t)21600ul);
    db.init(kk::t1f2_startTime, (uint32_t)25200ul);
    db.init(kk::t1f2_dim, (uint8_t)70);
    db.init(kk::t1f3_startTime, (uint32_t)43200ul);
    db.init(kk::t1f3_dim, (uint8_t)95);
    db.init(kk::t1f4_startTime, (uint32_t)64800ul);
    db.init(kk::t1f4_dim, (uint8_t)80);
    db.init(kk::t1f5_startTime, (uint32_t)72000ul);
    db.init(kk::t1_stopTime, (uint32_t)75600ul);

    db.init(kk::aquaDoz1_enabled, (uint8_t)0);
    db.init(kk::aquaDoz1_1time, (uint32_t)25200ul);
    db.init(kk::aquaDoz1_2time, (uint32_t)43200ul);
    db.init(kk::aquaDoz1_need3rd, (uint8_t)0);
    db.init(kk::aquaDoz1_3time, (uint32_t)64800ul);
    db.init(kk::aquaDoz1_need4th, (uint8_t)0);
    db.init(kk::aquaDoz1_4time, (uint32_t)72000ul);
    db.init(kk::aquaDoz1_need5th, (uint8_t)0);
    db.init(kk::aquaDoz1_5time, (uint32_t)73000ul);
    db.init(kk::aquaDoz1_need6th, (uint8_t)0);
    db.init(kk::aquaDoz1_6time, (uint32_t)74000ul);
    db.init(kk::aquaDoz1_need7th, (uint8_t)0);
    db.init(kk::aquaDoz1_7time, (uint32_t)75000ul);
    db.init(kk::aquaDoz1_need8th, (uint8_t)0);
    db.init(kk::aquaDoz1_8time, (uint32_t)76000ul);
    db.init(kk::aquaDoze1_dozeTime, (uint16_t)59);

    db.init(kk::btnName, "имечко кнопоньки");
    db.init(kk::btnColor, 0xff00aa);
    db.dump(Serial);

    // первый запуск всех исполнительных механизмов
    data.timer_nature_applied = 1;  // запустим природное освещение
    data.t1f_enbl = db[kk::t1f_enabled];
    userNatureTimer();
    data.t1discr_enbl = db[kk::t1Discr_enabled];  // запустим суточные таймеры
    data.t2discr_enbl = db[kk::t2Discr_enabled];
    data.t3discr_enbl = db[kk::t3Discr_enabled];
    data.t4discr_enbl = db[kk::t4Discr_enabled];
    data.t5discr_enbl = db[kk::t5Discr_enabled];
    data.t6discr_enbl = db[kk::t6Discr_enabled];
    userSixTimers();
    // пересчитываем температуру х10 чтобы не множиться в цикле
    // tdht1MaxX10
    // hdht2Min
    data.dhtOne.tTrigx10 = db[kk::dht1TempRele_startTemp].toInt() * 10;
    data.dhtTwo.hTrig = db[kk::dht2HumRele_startHum].toInt();
    // берем показания
    switch (db[kk::dht1TempRele_TempThreshold].toInt()) {
        case 0:
            data.dhtOne.tTreshold = 5;
            break;
        case 1:
            data.dhtOne.tTreshold = 10;
            break;
        case 2:
            data.dhtOne.tTreshold = 20;
            break;
        case 3:
            data.dhtOne.tTreshold = 30;
            break;
    }
    switch (db[kk::dht2HumRele_HumThreshold].toInt()) {
        case 0:
            data.dhtTwo.hTreshold = 1;
            break;
        case 1:
            data.dhtTwo.hTreshold = 2;
            break;
        case 2:
            data.dhtTwo.hTreshold = 5;
            break;
        case 3:
            data.dhtTwo.hTreshold = 10;
            break;
    }
    // userDhtRelays();
    //
    data.dsOne.tTrigx10 = db[kk::DS1Rele_startTemp].toInt() * 10;
    data.dsTwo.tTrigx10 = db[kk::DS2Rele_startTemp].toInt() * 10;
    switch (db[kk::DS1Rele_TempThreshold].toInt()) {
        case 0:
            data.dsOne.tTreshold = 2;
            break;
        case 1:
            data.dsOne.tTreshold = 5;
            break;
        case 2:
            data.dsOne.tTreshold = 10;
            break;
        case 3:
            data.dsOne.tTreshold = 30;
            break;
    }
    switch (db[kk::DS2Rele_TempThreshold].toInt()) {
        case 0:
            data.dsTwo.tTreshold = 2;
            break;
        case 1:
            data.dsTwo.tTreshold = 5;
            break;
        case 2:
            data.dsTwo.tTreshold = 10;
            break;
        case 3:
            data.dsTwo.tTreshold = 30;
            break;
    }
    // userDSRelays();

    // ======== WIFI ========
    // подключение и реакция на подключение или ошибку
    WiFiConnector.setPass("12345678");  // пароль точки доступа
    WiFiConnector.setTimeout(10);       // сколько секунд пытаться приконнектиттся
    WiFiConnector.onConnect([]() {
        Serial.print("Con with IP: ");
        Serial.println(WiFi.localIP());
        indikator.setPeriod(3000, 1, 200, 150);  // раз в 000 сек, 0 раз взмигнем - по 00 милисек периоды, гореть будем 0 милисек
        gotWifi = true;
        NTP.begin();
        NTP.setPeriod(600);  // обновлять раз в 600 сек
        NTP.tick();
        NTP.setGMT(db[kk::ntp_gmt]);
    });
    WiFiConnector.onError([]() {
        Serial.print("Error! start AP ");
        Serial.println(WiFi.softAPIP());
        indikator.setPeriod(600, 2, 100, 50);  // раз в  секунду два раза взмигнем - по 200 милисек, гореть будем 50 милисек
        if (each5min.ready()) {
            for (int i = 0; i < 10; i++) {
                digitalWrite(17, 1);
                delay(50);
                digitalWrite(17, 0);
                delay(50);
            }
            ESP.restart();  // через 5 минут ребутаемся
        }
    });

    WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);
    //  getdht1(); //опрос медленных датчиков
    //  delay(1);
    //   getdht2();
}  // setup

uint32_t prevMs = 0;
byte checker = 0;
void loop() {
    if (millis() - prevMs > 100) {
        Serial.print("Big loop time: ");
        Serial.println(millis() - prevMs);
    }
    prevMs = millis();

    // если wifi связь есть, сбрасываем вочдог таймер 5 минутный.
    // если нет связи, ждем 5 минут и ребутаемся, а то мало ли
    // если связь восстановилась после потери, снова мигаем плавно
    WiFiConnector.tick();  // поддержка wifi связи
    if (WiFiConnector.connected()) {
        if (!gotWifi) indikator.setPeriod(3000, 1, 200, 150);  // спокойное мигание после реконнекта к wifi
        gotWifi = true;
        each5min.rst();
    } else {
        if (gotWifi) {
            gotWifi = false;  // для запуска частой мигалки
            // общее время, кол-во, период одного, один зажжен на.
            indikator.setPeriod(1000, 10, 100, 70);  // часто мигаем
        }
        if (each5min.ready()) {
            for (int i = 0; i < 10; i++) {
                digitalWrite(17, 1);
                delay(70);
                digitalWrite(17, 0);
                delay(70);
            }
            ESP.restart();
        }
    }  // WiFi.connected()
    sett.tick();  // поддержка веб интерфейса
    NTP.tick();
    indikator.tick();  // in loop

    if (each5Sec.ready())  // раз в 5 сек
    {
        // поддержка NTP
        if (!NTP.status() && NTP.synced()) {
            data.secondsNow = NTP.daySeconds();
            curDataTime = NTP.getUnix();
        } else
            Serial.println("\n\n\t\t\t\tNTP not reached\n\n");
        // sensorsProbe(); // опросим датчики
        getdht1();  // опрос датчика медленный и умножение
        delay(1);   //  отдадим управление вайфаю
        getdht2();  // // опрос датчика медленный и умножение
        //         switch (checker) {
        //     case 0:  // INIT
        //         checker = 5;
        //         break;
        //     case 5:  // SOIL
        //         // read_Soil_Hum();  // Опрос датчика почвы
        //         // delay(1);         //  отдадим управление вайфаю
        //         // read_Soil_Temp();
        //         Serial.print("секунды: ");
        //         Serial.print(millis() / 1000);
        //         Serial.println("\t тут читаем датчик 1");
        //         checker = 10;
        //         break;
        //     case 10:  // AIR
        //         Serial.print("секунды: ");
        //         Serial.print(millis() / 1000);
        //         Serial.println("\t а тут читаем датчик 2");
        //         // read_Air_Hum();   // Опрос датчика воздуха
        //         // delay(1);         //  отдадим управление вайфаю
        //         // read_Air_Temp();  // Опрос датчика воздуха
        //         checker = 5;
        //         break;
        // }  // switch(checker)
    }  // each5Sec

    if (eachSec.ready()) {                  // раз в 1 сек
        data.secondsNow++;                  // инкермент реалтайм
        data.secondsUptime++;               // инкермент аптайм
        if (data.secondsUptime == 86399) {  // инкремент дней аптайма
            data.secondsUptime = 0;
            data.uptime_Days++;
        }
        get1ds18();  // тут умножение, часто не вызываем
        get2ds18();  // тут умножение, часто не вызываем
        userSixTimers();
        userNatureTimer();
        userFertiTimer();
    }  // eachSec

    userDhtRelays();  // тут ничего медленного, можно часто
    userDSRelays();   // тут ничего медленного, можно часто

    // if(db.changed()){
    //   Serial.print("База изменена\t");
    //   Serial.println(millis());
    // }
    // static bool prevPinState = 0;
    // if(digitalRead(RELE_4) != prevPinState){
    //     prevPinState = digitalRead(RELE_4);
    //     Serial.print("RELE_4 = ");
    //     Serial.print(prevPinState);
    //     Serial.print("\n");
    // }
    if (!digitalRead(BTN)) {
        delay(20);
        if (!digitalRead(BTN)) {
            int cnt = 0;
            while (!digitalRead(BTN)) {
                digitalWrite(INDIKATOR, 1);
                delay(30);
                digitalWrite(INDIKATOR, 0);
                delay(30);
                if (cnt < 60) cnt++;
                else ESP.restart();
            }
        }
    }  // BTN
}  // loop