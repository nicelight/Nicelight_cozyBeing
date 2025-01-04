#include "settings.h"

#include <GyverNTP.h>
#include <LittleFS.h>
#include <SettingsGyver.h>
#include <WiFiConnector.h>

#include "data.h"  // тут лежит структура data по кошерному
#include "nastroyki.h"
#include "userTimers.h"

// GyverDBFile db(&LittleFS, "/data.db");
// static SettingsESP sett(PROJECT_NAME " v" PROJECT_VER, &db);

GyverDBFile db(&LittleFS, "/database2025.db");      // база данных для хранения настроек будет автоматически записываться в файл при изменениях
SettingsGyver sett("Природный уют", &db);  // указывается заголовок меню, подключается база данных
Datime curDataTime(NTP);                         // NTP это объект типа GyverNTPClient, наследует stampticker (С) Гайвер
static bool notice_f;                            // флаг на отправку уведомления о подключении к wifi

static const char *const WEEKdays[] = {
    "как будто бы вчера",
    "Понедельник",
    "Вторник",
    "Среда",
    "Четверг",
    "Пятница",
    "Суббота",
    "Воскресенье"};
// это апдейтер. Функция вызывается, когда вебморда запрашивает обновления

sets::Logger logger(150);

void update(sets::Updater &upd) {
    // отправляем свежие значения по имени (хэшу) виджета

    upd.update(kk::secondsNow, data.secondsNow);
    upd.update(kk::secondsUptime, data.secondsUptime);

    // upd.update(kk::datime, String(curDataTime)); // старое
    upd.update(kk::datime, NTP.dateToString());
    // upd.update(kk::testlabel,  NTP.dateToString()); //https://github.com/GyverLibs/Stamp

    if (!data.uptime_Days) {
        upd.update(kk::uptimeDays, (String)("0 дней"));
    } else if (data.uptime_Days == 1)
        upd.update(kk::uptimeDays, (String)("1 день"));
    else if (data.uptime_Days < 5)
        upd.update(kk::uptimeDays, (String)(data.uptime_Days + String(" дня")));
    else if (data.uptime_Days >= 5)
        upd.update(kk::uptimeDays, (String)(data.uptime_Days + String(" дней")));
    // день недели выводим, оч красиво, Гайвер посоветовал

    upd.update(kk::dayofweek, (String)(WEEKdays[curDataTime.weekDay]));

    upd.update("t1Discr_led"_h, data.rel1_on);
    upd.update("t2Discr_led"_h, data.rel2_on);
    upd.update("t3Discr_led"_h, data.rel3_on);
    upd.update("t4Discr_led"_h, data.rel4_on);
    upd.update("t5Discr_led"_h, data.rel5_on);
    upd.update("t6Discr_led"_h, data.rel6_on);

    // upd.update("t1f_led"_h, data.t1isWorks);
    upd.update("aquaDoz1_led"_h, data.relFerti_on);
    upd.update("aquaDoz1_nextDozeIn"_h, data.untilNextDoze);

    upd.update(kk::floattempdht1, data.dhtOne.tfloat);
    upd.update(kk::humdht1, data.dhtOne.hum);
    upd.update(kk::dht1Rele_led, data.dhtOne.Rel_on);
    upd.update(kk::floattempdht2, data.dhtTwo.tfloat);
    upd.update(kk::humdht2, data.dhtTwo.hum);
    upd.update(kk::dht2Rele_led, data.dhtTwo.Rel_on);

    upd.update(kk::floattempDS1, data.dsOne.tfloat);
    upd.update(kk::DS1Rele_led, data.dsOne.rel_on);
    upd.update(kk::floattempDS2, data.dsTwo.tfloat);
    upd.update(kk::DS2Rele_led, data.dsTwo.rel_on);

    upd.update("lbl1"_h, (String)(curDataTime.weekDay + String(" день недели")));
    upd.update("lbl2"_h, millis());
    if (notice_f)  // уведомление при вводе wifi данных
    {
        notice_f = false;
        upd.notice("Если не ошибся с вводом, устройство подключится к  wifi сети, светодиод медленно замигает");
        //    upd.alert("Ошибка");
    }
}  // update
//
//
//

void build(sets::Builder &b) {
    // обработка действий от виджетов:
    switch (b.build.id) {
        // case kk::logUpdate:  // если ввели импут
        //     // logger.println(b.build.id, HEX);
        //     logger.print("старт таймера 1 в ");
        //     logger.println(db[kk::t1Discr_startTime]);
        //     logger.print(" curDataTime.weekDay:");
        //     logger.println(curDataTime.weekDay);
        //     break;
    }

    // костыль на моментальное обновление индикаторных светодиодов
    switch (b.build.id) {
        // перезапись NTP времени
        case kk::ntp_gmt:
            NTP.setGMT(b.build.value);
            b.reload();
            break;
        case kk::t1Discr_startTime:
            userSixTimers();
            b.reload();
            break;
        case kk::t1Discr_endTime:
            userSixTimers();
            b.reload();
            break;
        case kk::t2Discr_startTime:
            userSixTimers();
            b.reload();
            break;
        case kk::t2Discr_endTime:
            userSixTimers();
            b.reload();
            break;
        case kk::t3Discr_startTime:
            userSixTimers();
            b.reload();
            break;
        case kk::t3Discr_endTime:
            userSixTimers();
            b.reload();
            break;
        case kk::t4Discr_startTime:
            userSixTimers();
            b.reload();
            break;
        case kk::t4Discr_endTime:
            userSixTimers();
            b.reload();
            break;
        case kk::t5Discr_startTime:
            userSixTimers();
            b.reload();
            break;
        case kk::t5Discr_endTime:
            userSixTimers();
            b.reload();
            break;
        case kk::t6Discr_startTime:
            userSixTimers();
            b.reload();
            break;
        case kk::t6Discr_endTime:
            userSixTimers();
            b.reload();
            break;
        // обовляем отрисовку природного освещения если в вебморде изменения
        case kk::t1f1_startTime:
            data.timer_nature_applied = 1;
            userNatureTimer();
            b.reload();
            break;
        case kk::t1f2_startTime:
            data.timer_nature_applied = 1;
            userNatureTimer();
            b.reload();
            break;
        case kk::t1f2_dim:
            data.timer_nature_applied = 1;
            userNatureTimer();
            b.reload();
            break;
        case kk::t1f3_startTime:
            data.timer_nature_applied = 1;
            userNatureTimer();
            b.reload();
            break;
        case kk::t1f3_dim:
            data.timer_nature_applied = 1;
            userNatureTimer();
            b.reload();
            break;
        case kk::t1f4_startTime:
            data.timer_nature_applied = 1;
            userNatureTimer();
            b.reload();
            break;
        case kk::t1f4_dim:
            data.timer_nature_applied = 1;
            userNatureTimer();
            b.reload();
            break;
        case kk::t1f5_startTime:
            data.timer_nature_applied = 1;
            userNatureTimer();
            b.reload();
            break;
        case kk::t1_stopTime:
            data.timer_nature_applied = 1;
            userNatureTimer();
            b.reload();
            break;
        case kk::dht1TempRele_startTemp:
            // пересчитываем температуру х10 чтобы не множиться в цикле. аналогично в setup()
            data.dhtOne.tTrigx10 = db[kk::dht1TempRele_startTemp].toInt() * 10;
            userDhtRelays();
            b.reload();
            break;
        case kk::dht1TempRele_TempThreshold:
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
            userDhtRelays();
            b.reload();
            break;

        case kk::dht2HumRele_startHum:
            // пересчитываем влажность
            data.dhtTwo.hTrig = db[kk::dht2HumRele_startHum].toInt();
            userDhtRelays();
            b.reload();
            break;
        case kk::dht2HumRele_HumThreshold:
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
            userDhtRelays();
            b.reload();
            break;
        case kk::DS1Rele_startTemp:
            // пересчитываем температуру х10 чтобы не множиться в цикле. аналогично в setup()
            data.dsOne.tTrigx10 = db[kk::DS1Rele_startTemp].toInt() * 10;
            userDSRelays();
            b.reload();
            break;

        case kk::DS1Rele_TempThreshold:
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
            userDSRelays();
            b.reload();
            break;

        case kk::DS2Rele_startTemp:
            // пересчитываем температуру х10 чтобы не множиться в цикле. аналогично в setup()
            data.dsTwo.tTrigx10 = db[kk::DS2Rele_startTemp].toInt() * 10;
            userDSRelays();
            b.reload();
            break;

        case kk::DS2Rele_TempThreshold:
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
            userDSRelays();
            b.reload();
            break;
    }  //  switch (b.build.id)

    // b.Label(kk::testlabel, "тестовый лейбл");

    // WEB интерфейс ВЕБ морда формируется здесь
    {
        sets::Group g(b, "Nicelight");
        if (NTP.synced()) {
            {
                sets::Row g(b);
                // sets::Row g(b, "Row");
                // b.DateTime(kk::datime, "Сегодня ");
                b.Label(kk::dayofweek, "Сегодня");  // текущая дата
                b.Label(kk::datime, " ");           // текущая дата
            }
        }  // NTP.synced()

        {
            sets::Row g(b);
            // sets::Row g(b, "Row");
            b.Label(kk::uptimeDays, "Аптайм");
            b.Time(kk::secondsUptime, " ");
        }

        b.Time(kk::secondsNow, "Времечко");
        // b.Label(kk::secondsNow, "Времечко");
    }

    {
        sets::Group g(b, "Воздух");
        {
            sets::Row g(b);
            // b.LabelFloat(kk::floattemp1, "dht1", 1);
            b.LabelFloat(kk::floattempdht1, db[kk::dht1name], data.dhtOne.tfloat, 1, 0xec9736);  // DHT22 темп 1
            b.Label("°С");
        }
        {
            sets::Row g(b);
            // b.LabelNum(kk::humdht1, "Влажность", data.dhtOne.hum, sets::Colors::Aqua);  // влажность 1
            b.LabelNum(kk::humdht1, "Влажность", data.dhtOne.hum, 0xd17e1f);  // 0xd17e1f влажность 1
            // b.Label("%", "");
            b.Label("%");
        }
        if (b.Switch(kk::dht1TempRele_enabled, "Охлаждение", nullptr, 0xb7701e)) {  // Реле 1
            // data.dht1TempRele_enbl = db[kk::dht1TempRele_enabled].toInt();

            if (db[kk::dht1TempRele_enabled].toInt() == 0)
                data.dhtOne.State = 0;  // принудительно выключаем реле
            userDhtRelays();
            b.reload();
        }
        if (db[kk::dht1TempRele_enabled].toInt() != 0) {
            {
                sets::Row g(b);
                b.LED(kk::dht1Rele_led, "Cтатус >>", data.dhtOne.Rel_on, sets::Colors::Gray, sets::Colors::Yellow);
                b.Label(" ");
            }
            b.Number(kk::dht1TempRele_startTemp, "Включается при превышении, °C");
            b.Select(kk::dht1TempRele_TempThreshold, "Порог отключения", "0,5 °C;1 °C;2 °C;3 °C");
        }
        b.Label(" ");
        {
            sets::Row g(b);
            b.LabelFloat(kk::floattempdht2, db[kk::dht2name], data.dhtTwo.tfloat, 1, 0x3da7f2);  // DHT22 темп 2
            b.Label("°С");
        }
        {
            sets::Row g(b);
            b.LabelNum(kk::humdht2, "Влажность", data.dhtTwo.hum, 0x2680bf);  // Влажность 2
            b.Label("%");
        }
        if (b.Switch(kk::dht2HumRele_enabled, "Увлажнение", nullptr, 0x3da7f2)) {  // Реле 1
            if (db[kk::dht2HumRele_enabled].toInt() == 0)
                data.dhtTwo.State = 0;  // принудительно выключаем реле
            userDhtRelays();
            b.reload();
        }
        if (db[kk::dht2HumRele_enabled].toInt() != 0) {
            {
                sets::Row g(b);
                b.LED(kk::dht2Rele_led, "Cтатус >>", data.dhtTwo.Rel_on, sets::Colors::Gray, sets::Colors::Blue);
                b.Label(" ");
            }
            b.Number(kk::dht2HumRele_startHum, "Включается, если ниже");
            b.Select(kk::dht2HumRele_HumThreshold, "Порог отключения,", "1 %;2 %;5 %;10 %");
        }
    }  //"Воздух"
    //
    //

    {  // "Вода"
        sets::Group g(b, "Вода");
        {
            sets::Row g(b);
            // b.LabelFloat(kk::floattemp1, "dht1", 1);
            b.LabelFloat(kk::floattempDS1, db[kk::DS1name], data.dsOne.tfloat, 1, sets::Colors::Aqua);  // DHT22 темп 1
            b.Label("°С");
        }
        if (b.Switch(kk::DS1Rele_enabled, "Охлаждение", nullptr, sets::Colors::Aqua)) {  // Реле 1
            // data.DS1Rele_enbl = db[kk::DS1Rele_enabled];
            userDSRelays();
            b.reload();
        }
        // if (data.DS1Rele_enbl) {
        if (db[kk::DS1Rele_enabled].toInt() != 0) {
            {
                sets::Row g(b);
                b.LED(kk::DS1Rele_led, "Cтатус >>", data.dsOne.rel_on, sets::Colors::Gray, sets::Colors::Aqua);
                b.Label(" ");
            }
            b.Number(kk::DS1Rele_startTemp, "Включается при превышении, °C");
            b.Select(kk::DS1Rele_TempThreshold, "Порог отключения", "0,2 °C;0,5 °C;1 °C;3 °C");
        }
        b.Label(" ");
        {
            sets::Row g(b);
            // b.LabelFloat(kk::floattemp1, "dht1", 1);
            b.LabelFloat(kk::floattempDS2, db[kk::DS2name], data.dsTwo.tfloat, 1, sets::Colors::Aqua);  // DHT22 темп 1
            b.Label("°С");
        }
        if (b.Switch(kk::DS2Rele_enabled, "Нагрев", nullptr, sets::Colors::Aqua)) {  // Реле 1
            // data.DS2Rele_enbl = db[kk::DS2Rele_enabled];
            userDSRelays();
            b.reload();
        }
        // if (data.DS2Rele_enbl) {
        if (db[kk::DS2Rele_enabled].toInt() != 0) {
            {
                sets::Row g(b);
                b.LED(kk::DS2Rele_led, "Cтатус >>", data.dsTwo.rel_on, sets::Colors::Gray, sets::Colors::Aqua);
                b.Label(" ");
            }  // LED row
            b.Number(kk::DS2Rele_startTemp, "Включается, если ниже °C");
            b.Select(kk::DS2Rele_TempThreshold, "Порог отключения", "0,2 °C;0,5 °C;1 °C;3 °C");
        }
    }  //"Вода"

    { /* суточные таймеры */
        sets::Group g(b, "Суточные таймеры");
        // if (b.Switch(kk::t1Discr_enabled, " Реле света 1"))
        if (b.Switch(kk::t1Discr_enabled, db[kk::t1Discr_name], nullptr, sets::Colors::Yellow)) {  // Реле 1
            data.t1discr_enbl = db[kk::t1Discr_enabled];
            userSixTimers();
            b.reload();
        }
        if (data.t1discr_enbl) {
            // if (db[kk::t1Discr_enabled].()) {
            {
                sets::Row g(b);
                b.LED("t1Discr_led"_h, "Cтатус >>", data.rel1_on, sets::Colors::Gray, sets::Colors::Yellow);
                b.Label(" ");
            }  // LED row
            // b.Time(kk::t1Discr_startTime, "Включается в ..", 0xf7e5b2);
            b.Time(kk::t1Discr_startTime, "Включается в ..");
            b.Time(kk::t1Discr_endTime, ".. и отключается в");
            b.Label(" ", " ");
        }
        // if (b.Switch(kk::t2Discr_enabled, " Реле света 2"))
        // if (b.Switch(kk::t2Discr_enabled, db[kk::t2Discr_name])) // Реле 2
        if (b.Switch(kk::t2Discr_enabled, db[kk::t2Discr_name], nullptr, sets::Colors::Green))  // Реле 2
        {
            data.t2discr_enbl = db[kk::t2Discr_enabled];
            userSixTimers();
            b.reload();
        }
        if (data.t2discr_enbl) {
            // if (db[kk::t2Discr_enabled].()) {
            {
                sets::Row g(b);
                b.LED("t2Discr_led"_h, "Cтатус >>", data.rel2_on, sets::Colors::Gray, sets::Colors::Green);
                b.Label(" ");
            }  // LED row
            b.Time(kk::t2Discr_startTime, "Вкл в ..");
            b.Time(kk::t2Discr_endTime, ".. откл ");
            b.Label(" ", " ");
        }
        // if (b.Switch(kk::t3Discr_enabled, " Реле 3"))
        // if (b.Switch(kk::t3Discr_enabled, db[kk::t3Discr_name])) // Реле 3
        if (b.Switch(kk::t3Discr_enabled, db[kk::t3Discr_name], nullptr, sets::Colors::Mint))  // Реле 3
        {
            data.t3discr_enbl = db[kk::t3Discr_enabled];
            userSixTimers();
            b.reload();
        }
        if (data.t3discr_enbl) {
            // if (db[kk::t3Discr_enabled].()) {
            {
                sets::Row g(b);
                b.LED("t3Discr_led"_h, "Cтатус >>", data.rel3_on, sets::Colors::Gray, sets::Colors::Mint);
                b.Label(" ");
            }  // LED row
            b.Time(kk::t3Discr_startTime, "Вкл в ..");
            b.Time(kk::t3Discr_endTime, ".. откл");
            b.Label(" ", " ");
            //    b.Time("", &data.secondsStart);// так было
        }
        // if (b.Switch(kk::t4Discr_enabled, " Реле 4"))
        // if (b.Switch(kk::t4Discr_enabled, db[kk::t4Discr_name])) // Реле 4
        if (b.Switch(kk::t4Discr_enabled, db[kk::t4Discr_name], nullptr, sets::Colors::Aqua))  // Реле 4
        {
            data.t4discr_enbl = db[kk::t4Discr_enabled];
            userSixTimers();
            b.reload();
        }
        if (data.t4discr_enbl) {
            // if (db[kk::t4Discr_enabled].()) {
            {
                sets::Row g(b);
                b.LED("t4Discr_led"_h, "Cтатус >>", data.rel4_on, sets::Colors::Gray, sets::Colors::Aqua);
                b.Label(" ");
            }  // LED row
            b.Time(kk::t4Discr_startTime, "Вкл в ..");
            b.Time(kk::t4Discr_endTime, ".. откл");
            b.Label(" ", " ");
            //    b.Time("", &data.secondsStart);// так было
        }
        // if (b.Switch(kk::t5Discr_enabled, " Реле 5"))
        // if (b.Switch(kk::t5Discr_enabled, db[kk::t5Discr_name])) // Реле 5
        if (b.Switch(kk::t5Discr_enabled, db[kk::t5Discr_name], nullptr, sets::Colors::Blue))  // Реле 5
        {
            data.t5discr_enbl = db[kk::t5Discr_enabled];
            userSixTimers();
            b.reload();
        }
        if (data.t5discr_enbl) {
            // if (db[kk::t5Discr_enabled].()) {
            {
                sets::Row g(b);
                b.LED("t5Discr_led"_h, "Cтатус >>", data.rel5_on, sets::Colors::Gray, sets::Colors::Blue);
                b.Label(" ");
            }  // LED row
            b.Time(kk::t5Discr_startTime, "Вкл в ..");
            b.Time(kk::t5Discr_endTime, ".. откл");
            b.Label(" ", " ");
        }
        // if (b.Switch(kk::t6Discr_enabled, " Реле 6"))
        // if (b.Switch(kk::t6Discr_enabled, db[kk::t6Discr_name])) // Реле 6
        if (b.Switch(kk::t6Discr_enabled, db[kk::t6Discr_name], nullptr, sets::Colors::Violet))  // Реле 6
        {
            data.t6discr_enbl = db[kk::t6Discr_enabled];
            userSixTimers();
            b.reload();
        }
        if (data.t6discr_enbl) {
            // if (db[kk::t6Discr_enabled].()) {
            {
                sets::Row g(b);
                b.LED("t6Discr_led"_h, "Статус >>", data.rel6_on, sets::Colors::Gray, sets::Colors::Violet);
                b.Label(" ");
            }  // LED row
            b.Time(kk::t6Discr_startTime, "Вкл в ..");
            b.Time(kk::t6Discr_endTime, ".. откл");
            b.Label("Дни недели", " ");
            b.Switch(kk::t6Discr_inMonday, "Понедельник", nullptr, sets::Colors::Violet);
            b.Switch(kk::t6Discr_inTuesday, "Вторник", nullptr, sets::Colors::Violet);
            b.Switch(kk::t6Discr_inWensday, "Среда", nullptr, sets::Colors::Violet);
            b.Switch(kk::t6Discr_inThursday, "Четверг", nullptr, sets::Colors::Violet);
            b.Switch(kk::t6Discr_inFriday, "Пятница", nullptr, sets::Colors::Violet);
            b.Switch(kk::t6Discr_inSaturday, "Суббота", nullptr, sets::Colors::Violet);
            b.Switch(kk::t6Discr_inSunday, "Воскресенье", nullptr, sets::Colors::Violet);
            b.Label(" ", " ");
            b.Label(" ", " ");
        }
    } /* суточные таймеры */
    //
    //

    /* аквариумистика */
    {
        sets::Group g(b, "Природное освещение");
        if (b.Switch(kk::t1f_enabled, "Сделать красиво", nullptr, sets::Colors::Orange)) {
            Serial.print("\n\t\t\t Switch Природное enabled");
            data.t1f_enbl = db[kk::t1f_enabled];  // переносим свич в RAM (а то он гонит когда его .() опрашиваешь)
            if (!data.t1f_enbl) {
                ledcWrite(RED_PWM_CHANNEL, 0);
                ledcWrite(GREEN_PWM_CHANNEL, 0);
                ledcWrite(BLUE_PWM_CHANNEL, 0);
            }
            data.timer_nature_applied = 1;  // обновление считывания данных из web морды в RAM

            userNatureTimer();
            // Serial.print("\tdata.t1f_enbl = ");
            // Serial.print(data.t1f_enbl);
            // Serial.print("\n\n");
            b.reload();
        }

        if (data.t1f_enbl) {
            // if (db[kk::t1f_enabled]) {
            // if (db[kk::t1f_enabled].toInt()) {
            // b.LED("t1f_led"_h, "Cтатус >>", data.t1isWorks, sets::Colors::Gray, sets::Colors::Orange);
            b.Time(kk::t1f1_startTime, "Рассвет начинается с");
            b.Time(kk::t1f2_startTime, "Утро с");
            b.Slider(kk::t1f2_dim, "яркость утром");
            b.Time(kk::t1f3_startTime, "День с");
            b.Slider(kk::t1f3_dim, "яркость днем");
            b.Time(kk::t1f4_startTime, "Вечер  с");
            b.Slider(kk::t1f4_dim, "яркость вечером");
            b.Time(kk::t1f5_startTime, "Закат начинается");
            b.Time(kk::t1_stopTime, "полная тьма к");
            // if (b.Button(kk::t1_btn_accept, "Обновить", sets::Colors::Orange)) {
            //     //  Serial.println("\n\n\tApplied\n\n");
            //     data.timer_nature_applied = 1;
            //     userNatureTimer();
            //     b.reload();
            // }
            b.Label(" ", " ");

        }  // if enabled
    }  // природное освещение
    /* аквариумистика */

    /* аквамен дозатор шприцы */
    // TODO
    // допилить userTImers
    // добавить в базу состояние вклю выкл и временные шняги
    {
        sets::Group g(b, "Мультитаймер");
        if (b.Switch(kk::aquaDoz1_enabled, "Дозатор", nullptr, sets::Colors::Aqua)) {
            // data.tFerti_enbl = db[kk::aquaDoz1_enabled].toInt();
            userFertiTimer();
            b.reload();
        }
        if (db[kk::aquaDoz1_enabled].toInt()) {
            {
                sets::Row g(b);
                b.LED("aquaDoz1_led"_h, "Cтатус >>", data.relFerti_on, sets::Colors::Gray, sets::Colors::Aqua);
                b.Label(" ");
            }  // LED row
            b.Time(kk::aquaDoz1_1time, "Первый раз в");
            b.Time(kk::aquaDoz1_2time, "Второй раз в");
            if (b.Switch(kk::aquaDoz1_need3rd, "Больше доз", nullptr, sets::Colors::Aqua)) {
                // userNatureTimer();
                b.reload();
            }
            if (db[kk::aquaDoz1_need3rd].toInt()) {
                b.Time(kk::aquaDoz1_3time, "Еще подача в");
                if (b.Switch(kk::aquaDoz1_need4th, "Еще подача", nullptr, sets::Colors::Aqua)) {
                    // userNatureTimer();
                    b.reload();
                }
                if (db[kk::aquaDoz1_need4th].toInt()) {
                    b.Time(kk::aquaDoz1_4time, "в");
                    if (b.Switch(kk::aquaDoz1_need5th, "Еще подача", nullptr, sets::Colors::Aqua)) {
                        // userNatureTimer();
                        b.reload();
                    }
                    if (db[kk::aquaDoz1_need5th].toInt())
                        b.Time(kk::aquaDoz1_5time, "в");
                    if (b.Switch(kk::aquaDoz1_need6th, "Еще подача", nullptr, sets::Colors::Aqua)) {
                        // userNatureTimer();
                        b.reload();
                    }
                    if (db[kk::aquaDoz1_need6th].toInt())
                        b.Time(kk::aquaDoz1_6time, "в");
                    if (b.Switch(kk::aquaDoz1_need7th, "Еще подача", nullptr, sets::Colors::Aqua)) {
                        // userNatureTimer();
                        b.reload();
                    }
                    if (db[kk::aquaDoz1_need7th].toInt())
                        b.Time(kk::aquaDoz1_7time, "в");
                    if (b.Switch(kk::aquaDoz1_need8th, "Еще подача", nullptr, sets::Colors::Aqua)) {
                        // userNatureTimer();
                        b.reload();
                    }
                    if (db[kk::aquaDoz1_need8th].toInt())
                        b.Time(kk::aquaDoz1_8time, "в");
                }  // 4th
            }  // 3rd

            b.Time(kk::aquaDoze1_dozeTime, "Подача дозы в течении");
            // if (b.Button(kk::aquaDoz1_btn_accept, "Обновить", sets::Colors::Aqua)) {
            //     //  Serial.println("\n\n\tApplied\n\n");
            //     // data.timer_nature_applied = 1;
            //     // userNatureTimer();
            //     b.reload();
            // }

            b.Label(" ", " ");
            b.Label(" ", " ");
            b.Time("aquaDoz1_nextDozeIn"_h, "Следующая доза через");
            if (b.Button(kk::aquaDoz1_makeDoze, "Дать дозу", sets::Colors::Gray)) {
                //  Serial.println("\n\n\tApplied\n\n");
                // data.timer_nature_applied = 1;
                // userNatureTimer();
                releFertiProcess = 10;
                b.reload();
            }

            b.Label(" ", " ");

        }  // if enabled
    } /* аквамен дозатор шприцы */

    /* Настройки , внизу страницы*/
    {
        sets::Group g(b, " ");
        {
            sets::Menu g(b, "Опции");

            {
                sets::Menu g(b, "Интерфейс");
                /*
                      //пример изменения имени виджета
                      b.Input(kk::btnName, "новое имя кнопки:");
                      if(b.Button(kk::btnflex, db[kk::btnName], db[kk::btnColor])) b.reload();
                */

                b.Input(kk::t1Discr_name, "Имя Реле1:");
                b.Input(kk::t2Discr_name, "Имя Реле2:");
                b.Input(kk::t3Discr_name, "Имя Реле3:");
                b.Input(kk::t4Discr_name, "Имя Реле4:");
                b.Input(kk::t5Discr_name, "Имя Реле5:");
                b.Input(kk::t6Discr_name, "Имя Реле6:");
                b.Input(kk::dht1name, "dht22 1:");
                b.Input(kk::dht2name, "dht22 2:");
                b.Input(kk::DS1name, "DS18B20 1:");
                b.Input(kk::DS2name, "DS18B20 2:");
            }
            {
                sets::Menu g(b, "Расширенные");
                /// провалились в расширенные пристройки
                {
                    sets::Group g(b, "настройки WiFi");
                    b.Input(kk::wifi_ssid, "SSID");
                    b.Pass(kk::wifi_pass, "Password");
                    if (b.Button(kk::apply, "Save & Restart")) {
                        db.update();  // сохраняем БД не дожидаясь таймаута
                        WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);
                        notice_f = true;
                        //          ESP.restart();
                    }  // button Save
                }  // настройки wifi
                b.Input(kk::ntp_gmt, "Часовой пояс");
                b.Label(" ", " ");
                b.Label(" ", " ");

                // // и просто виджеты
                // b.Label("lbl3"_h, "Another label", "Val", sets::Colors::Green);
                // b.Label("lbl4"_h, "Привет", "Val", sets::Colors::Blue);

                // кнопки являются "групповым" виджетом, можно сделать несколько кнопок в одной строке
                if (b.beginButtons()) {
                    // кнопка вернёт true при клике
                    // if (b.Button(kk::btn1, "-")) {
                    //     Serial.println("reload");
                    //     b.reload();
                    // }

                    if (b.Button(kk::btn2, "стереть базу(аккуратно!)", sets::Colors::Red)) {
                        Serial.println("could clear db");
                        // db.clear();
                        // db.update();
                    }
                    b.endButtons();  // завершить кнопки
                }
            }  // Расширенные

            {
                sets::Menu g(b, "примеры");
                {
                    sets::Group g(b, "Логи");

                    b.Log(logger);
                    if (b.Button(kk::logUpdate, "Обновить лог", 0x2f7d34)) {
                        logger.print(millis() >> 10);
                        logger.print(" ");
                        logger.print("день недели:");
                        logger.println(curDataTime.weekDay);
                        // logger.print("dht1 sw enbl: ");
                        // logger.println(data.dht1TempRele_enbl);
                        b.reload();
                    }
                }  // g(b, "Логи");
                b.Input(kk::txt, "Text");
                b.Pass(kk::pass, "Password");
                b.Input(kk::uintw, "uint");
                {
                    sets::Group g(b, "групка");
                    // пример изменения имени виджета
                    b.Input(kk::btnName, "новое имя кнопки:");
                    if (b.Button(kk::btnflex, db[kk::btnName], db[kk::btnColor]))
                        b.reload();
                    b.Input("intw"_h, "int");
                    b.Switch("sw1"_h, "switch 1");
                }
                //      b.Input(kk::intw, "int");
                b.Input(kk::int64w, "int 64");
                // тут тоже могут быть группы
                {
                    sets::Group g(b, "тумблерки");
                    b.Switch("sw2"_h, "switch 2");
                    b.Switch("sw3"_h, "switch 3");
                }

                {
                    sets::Group g(b, "Group 3");
                    b.Label(kk::lbl1, "lable1");
                    // b.Label(kk::uptimeDays, "Uptime");
                    b.Label(kk::lbl2, "millis()", "", sets::Colors::Red);
                    b.Date(kk::date, "Date");
                    // b.DateTime(kk::datime, "Datime");
                    b.Color(kk::color, "Color");
                    //    b.Switch(kk::toggle2, "Switch");
                    b.Select(kk::selectw, "Select", "var1;var2;hello");
                    b.Slider(kk::slider, "Slider", -10, 10, 0.5, "deg");
                }

                // вне группы. Так тоже можно
                b.Switch(kk::toggle, "тыр-тырка");
            }  // Menu-примеры
        }
    }  // Подстройки

}  // builder
