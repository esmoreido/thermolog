# thermolog
WiFi-термокоса на ESP32, DS18B20 и витой паре для гидрологических исследований

<p>Простая, недорогая в изготовлении, достаточно точная и надежная термокоса с html-интерфейсом. 
Создана на базе микроконтроллера <a href="https://mcustore.ru/store/espressif-systems/kontroller-na-baze-esp32s-cp2102-wifi+bluetooth/">ESP32</a> со встроенным WiFi и веб-сервером. На сервере запускается скрипт, отображающий график температур с датчиков по длине косы, использующий для отображения google charts. В качестве измерительных элементов используется массив датчиков <a href="https://mcustore.ru/store/datchiki-i-sensory/datchik-temperatury-ds18b20-korpus-to-92/">DS18B20</a>. Датчики используют технологию OneWire, поэтому каждый датчик имеет собственный HEX адрес, что позволяет разделять показания датчиков, подключенных по одному проводу. Носитель - <a href = "https://www.electro-mpo.ru/catalog/provod_i_kabel/p19_setevye_kabeli_utp_ftp_kip_pk/p1923-kabel-setevoy-parlan-u-utp-4x2x0-52-mm-cat-5/">уличная витая пара в толстой ПВХ оплетке </a>. Для герметизации датчики залиты термоклеем и обмотаны изолентой.
Питание - по USB от powerbank.</p>
