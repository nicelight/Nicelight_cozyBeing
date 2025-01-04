#define DEBUG
// имя пароль вашей домашней сети
// можно ввести, подключившись к ESP AP c паролем 1234567890
#define WIFI ""
#define WIFIPASS ""

// #define ON 0  // включение релюшек логическим нулем или единицей
// #define OFF 1 //
#define ON 1  // включение симмисторов лог. единицей
#define OFF 0 //

//      //      //      для тестов пин 17 
#define DS18B20_1 13
#define DS_1_RELAY 18
 
#define DS18B20_2 30
#define DS_2_RELAY 19 // ?

#define DHT1PIN 4
#define DHT1TYPE DHT22
#define DHT1RELAY 32

#define DHT2PIN 16
#define DHT2TYPE DHT22
#define DHT2RELAY 33

#define RELE_1 5
#define RELE_2 4
#define RELE_3 15
#define RELE_4 16
#define RELE_5 17  
#define RELE_6 19 // ?

#define RELE_FERTILIZER 23


//для rgb ленты
#define RED_RGB_LENTA 27
#define GREEN_RGB_LENTA 26
#define BLUE_RGB_LENTA 25

#define RED_PWM_CHANNEL 0  // не менять
#define GREEN_PWM_CHANNEL 1 
#define BLUE_PWM_CHANNEL 2

#define INDIKATOR 2 // на каком пине индикаторный светодиод
#define BTN 0 // встроенная кнопка
































// #define CHECKT1 // отладка глюков таймера природного освещения


// для тестов 
#ifndef RELE_1
#define RELE_1 LED_BUILTIN
#endif 
#ifndef RELE_2
#define RELE_2 LED_BUILTIN
#endif 
#ifndef RELE_3
#define RELE_3 LED_BUILTIN
#endif 
#ifndef RELE_4
#define RELE_4 LED_BUILTIN
#endif 
#ifndef RELE_5
#define RELE_5 LED_BUILTIN
#endif 
