<a id="anchor"></a>
<h1 align="center"> Nicelight </h1>

## Контроллер для управления системами поддержания жизни.  
Содержит веб морду с гибкими настройками, отображением параметров окружающей среды и тонкого упралвения таймерами, реле с любыми подключаемыми устройствами.  
Прошивка основана на Гайверовской библиотеке Settings
Пока что на контроллере ESP32. Легко адаптируется под es8266.
Не стесняйтесь задавать любые вопросы в телеграм сообществе [Smartfarm_diy](https://t.me/smartfarm_diy)

![Веб морда для управления открывается в Chrome c телефона или компьютера](https://github.com/nicelight/minihub-pio/blob/main/pic/promo.png)

## Установка прошивки
### 1. Устанавливаем драйвер для связи компа и платы
Подключаем плату esp32 WROOM32 к компьютеру. При подключении должен быть характерный звук тынь-дынь, как будто флешку подключили. Если звука нет, кабель плохой, ищите более хороший. Или звук на компе отключен :) Если подключаете плату первый раз, нужно установить драйвер для связи компа с платой. Бывает [несколько типов драйверов](https://blog.spacehuhn.com/espcomm#heading-drivers) в зависимости от версии платы, которую вы используете. Для нашей WROOM32 обычно подходит драйвер [CP2102](https://www.silabs.com/documents/public/software/CP210x_Universal_Windows_Driver.zip) Если не подошел, перейдите по ссылке выше и по фоткам определите какая микросхема установлена на плате, под фоткой будет ссылка на скачивание драйверов. Скачиваем архив, распаковываем все файлы, нажимаем по файлу с именем в конце .inf правой кнопкой мыши - установить. Аналогично в папке x64 правой кнопкой мыши по файлу - установить. Теперь плата должна быть на связи с компьютером.

### 2. Прошиваем плату через сайт
На [этом сатйе](https://esp.huhn.me/) можно прошить плату. 
Открываем сайт с браузера Chrome!
 Жмем зеленую кнопку Connect. Всплывает маленькое окно с выбором нашей платы из списка. В списке отобразятся все подключенные к компу платы на данный момент, но обчно она там одна. У меня это CP2102, выбираем - Connect. Плата связана с прошивальщиком. 
![подключаем ](https://github.com/nicelight/minihub-pio/blob/main/pic/web-connect.png)

Если пишет Connecting и ничего не происходит, отключите плату от компьютера, обновите страницу, подключите плату и снова жмите зеленую кнопку. Успешным подключением будет окно с 4мя строчками:

![подключились ](https://github.com/nicelight/minihub-pio/blob/main/pic/connected.png)

Теперь настало время скачать прошивку. Свежие прошивки можно найти в [релизах](https://github.com/nicelight/Nicelight_cozyBeing/releases), на данный момент [скачиваем v0.1](https://github.com/nicelight/Nicelight_cozyBeing/releases/download/v0.1/firmware_full.bin), браузер может ругаться что мол опасный файл, нельзя такое качать. постарайтесь его уговорить, мол download anyway. Скачиваем, запоминаем путь куда скачалась эта прошивка firmware_full.bin
Возвращаемся в браузере на страницу прошивальщика, в первой строчке исправляем значение 1000 на 0000, правее жмем светло-синюю кнопку Select и выбираем файл прошивки. Нажимаем ниже зеленую кнопку Program. Процесс пошел, займет около 1 минуты.

## Использование контроллера
Чтобы к контроллеру подключить датчики и реле, удобно использовать: "Плата расширения Wroom32", легко найти ее на Озоне или Алиэкспресс.


nicelight/Nicelight_cozyBeing