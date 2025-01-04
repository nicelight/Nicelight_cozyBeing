
#include "sensors.h"

#include "data.h"
#include "nastroyki.h"
#include "settings.h"

GyverDS18Single ds1(DS18B20_1);  // пин
GyverDS18Single ds2(DS18B20_2);  // пинDHT2PIN

DHT dht1(DHT1PIN, DHT1TYPE);
DHT dht2(DHT2PIN, DHT2TYPE);

void get1ds18() {
    if (ds1.ready()) {         // измерения готовы по таймеру
        if (ds1.readTemp()) {  // если чтение успешно
            Serial.print("temp1: ");
            // Serial.println(ds1.getTemp());
            data.dsOne.tfloat = ds1.getTemp();
            data.dsOne.tx10 = (int)(data.dsOne.tfloat * 10);
        } else {
            // Serial.println("error");
            data.dsOne.tfloat = -80.0;
        }

        ds1.requestTemp();  // запрос следующего измерения
    }
}  // get1ds18()
//

void get2ds18() {
    if (ds2.ready()) {         // измерения готовы по таймеру
        if (ds2.readTemp()) {  // если чтение успешно
            Serial.print("temp2: ");
            // Serial.println(ds2.getTemp());
            data.dsTwo.tfloat = ds2.getTemp();
            data.dsTwo.tx10 = (int)(data.dsTwo.tfloat * 10);
        } else {
            // Serial.println("error");
            data.dsTwo.tfloat = random(41, 44);
        }

        ds2.requestTemp();  // запрос следующего измерения
    }
}  // get2ds18()
//
//

void getdht1() {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht1.readHumidity();
    float t = dht1.readTemperature();
    // float hic = dht1.computeHeatIndex(t, h, false);  // Compute heat index in Celsius (isFahreheit = false)
    if (isnan(h) || isnan(t)) {
        data.dhtOne.hum = -80;
        data.dhtOne.tfloat = -80.0;
    } else {
        data.dhtOne.hum = (int)h;
        data.dhtOne.tx10 = (int)(t * 10);
        data.dhtOne.tfloat = t;
    }
}  // getdht()

void getdht2() {
    float h = dht2.readHumidity();
    float t = dht2.readTemperature();
    if (isnan(h) || isnan(t)) {
        data.dhtTwo.hum = -80;
        data.dhtTwo.tfloat = -80.0;
    } else {
        data.dhtTwo.hum = (int)h;
        data.dhtTwo.tfloat = t;
    }
}  // getdht()
//
//
//