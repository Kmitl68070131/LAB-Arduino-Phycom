# 📡 รับส่งข้อมูลกับเว็บผ่าน MQTT — พร้อม Serial และจอ LCD

[หน้าแรก](../README.md) · [ไปยังคู่มือสถิติเซนเซอร์](../กันเหลี่ยม/README.md)

ทุกตัวอย่างเชื่อม Wi-Fi, publish JSON, subscribe คำสั่งจากเว็บ, แสดง Serial/LCD และ reconnect อัตโนมัติ อัปโหลด **ทีละ sketch** ไม่ต่อทุกชุดพร้อมกัน


> 🔘 **อยากกดปุ่ม 2 ขาให้ขึ้นเว็บ?** [เปิดคู่มือทำตามทีละขั้น](ButtonEvents/README.md) → เลือก `MQTT / ButtonEvents` ในเว็บ
>
> 📚 **ติดตั้งอะไรบ้าง?** [Library + MQTT Protocol + API + วิธีแก้ปัญหา](LIBRARIES.md)

> 📚 **เรียนรู้ array/list กับทุกเซนเซอร์:** [เก็บประวัติ ดูรายการบน LCD และรับส่งเว็บ](../อาร์เรย์และลิสต์/README.md)

## 🧭 เลือกหัวข้อที่ต้องการ

| ต้องการทำอะไร | กดเปิด |
|---|---|
| 🔘 ปุ่ม 2 ขา กด–ปล่อยแล้วขึ้นเว็บ | [ต่อสาย + Upload + เปิดเว็บ + ทดลองกด](ButtonEvents/README.md) |
| 📚 ลงไลบรารีให้ครบ | [Arduino / LCD / Sensor / Node.js](LIBRARIES.md#ไลบรารีที่ต้องติดตั้ง) |
| 🌐 ตั้ง Wi-Fi / Broker / บัญชี | [ตารางค่าที่ต้องแก้](LIBRARIES.md#ตั้งค่าเครือข่ายและบัญชี) |
| 📦 ส่งหรือรับ JSON | [Topic และรูปแบบข้อมูล](LIBRARIES.md#topic-และ-json) |
| 🛠 ดู API และหัวข้อ MQTT | [MQTTClient / QoS / retain / will](LIBRARIES.md#คำสั่ง-mqttclient) |
| 🌡 เลือกเซนเซอร์และต่อวงจร | [ตารางตัวอย่าง](#เลือกตัวอย่างและการต่อสาย) |
| 💻 เปิดเว็บรับ–ส่งจริง | [ขั้นตอน Node bridge](#เปิดเว็บรับส่งจริง) |
| 📊 คำนวณ min/max/avg/median/std | [โฟลเดอร์กันเหลี่ยม](../กันเหลี่ยม/README.md) |
| 🔍 ทำแล้วไม่ขึ้น | [ตารางตรวจปัญหา](LIBRARIES.md#ตรวจปัญหา) |

<details>
<summary>⚡ เริ่มใช้งานด่วน — คลิกขยาย</summary>

1. ต่อปุ่มระหว่าง **D4 กับ GND** และ LCD ที่ **5V/GND/SDA/SCL**
2. เปิด `ButtonEvents/ButtonEvents.ino` ใส่ Wi-Fi และติดตั้งไลบรารีตามคู่มือ
3. Upload ลง UNO R4 WiFi เปิด Serial **115200**
4. ที่ `MQTT/web` รัน `npm ci` แล้ว `npm start`
5. เปิด http://127.0.0.1:3000 เลือก **MQTT / ButtonEvents**
6. กดปุ่มนิ่งประมาณ 0.1–1 วินาทีแล้วปล่อย → เว็บแสดงสถานะและจำนวนครั้ง
7. ส่ง `{"text":"Hello LCD"}` จากเว็บ → ดู Serial, LCD และ ACK

</details>

## เลือกตัวอย่างและการต่อสาย

| อุปกรณ์ | หน่วย | ต่อสายเซนเซอร์ (ทุกชุด GND ร่วม) |
|---|---|---|
| [เซนเซอร์วัดอุณหภูมิ TMP36](TMP36/README.md) | C | Vout → A0; Vs → 5V; GND → GND (ตรวจ pinout รุ่นจริง) |
| [เซนเซอร์วัดอุณหภูมิ MCP9700](MCP9700/README.md) | C | Vout → A0; VDD → 5V; GND → GND (ตรวจ pinout รุ่นจริง) |
| [เซนเซอร์วัดอุณหภูมิ LM35](LM35/README.md) | C | Vout → A0; Vs → 5V; GND → GND (ตรวจ pinout รุ่นจริง) |
| [เทอร์มิสเตอร์ TTC05103 วงจรตัวต้านทาน 330 โอห์ม](TTC05103/README.md) | C | 5V → R330Ω → A0 → TTC05103 → GND; B4050/R25=10k ตามไฟล์เดิม |
| [เทอร์มิสเตอร์ NTC วงจรตัวต้านทาน 10 กิโลโอห์ม](NTC/README.md) | C | 5V → NTC 10k B3950 → A0 → R 10k → GND |
| [ตัวต้านทานปรับค่าได้ (ปุ่มหมุน)](Potentiometer/README.md) | ADC | ขานอก → 5V/GND; ขากลาง → A0 |
| [เซนเซอร์วัดระดับแสง LDR](LDR/README.md) | ADC | 5V → LDR → A0 → R 10k → GND; ค่าสูงเมื่อสว่าง ไม่ใช่ lux |
| [เซนเซอร์วัดระยะอัลตราโซนิก](Ultrasonic/README.md) | cm | HC-SR04 VCC → 5V; GND → GND; TRIG → D2; ECHO → D3 |
| [เซนเซอร์ตรวจจับการเคลื่อนไหว PIR](PIR/README.md) | state | VCC → 5V; GND → GND; OUT → D4; รอ warm-up ตามรุ่น |
| [ปุ่มกด 2 ขา ส่งกด–ปล่อยและจำนวนครั้งขึ้นเว็บ](ButtonEvents/README.md) | state + จำนวนกด | ปุ่ม NO 2 ขา ระหว่าง D4 และ GND; debounce 30ms ส่งขอบกด/ปล่อย |
| [ปุ่มกด 2 ขา อ่านสถานะเป็นรอบ](Button/README.md) | state | ปุ่มระหว่าง D4 กับ GND ใช้ INPUT_PULLUP; อ่าน 1 = กด |
| [แป้นกดตัวเลข 4×4](Keypad/README.md) | code | 4x4 R1–R4 → D9,D8,D7,D6; C1–C4 → D5,D4,D3,D2 |
| [เซนเซอร์วัดอุณหภูมิและความชื้น DHT11](DHT11/README.md) | C | VCC → 5V; GND → GND; DATA → D2; pull-up 10k DATA ไป VCC |
| [เซนเซอร์วัดอุณหภูมิและความชื้น DHT22](DHT22/README.md) | C | VCC → 5V; GND → GND; DATA → D2; pull-up 10k DATA ไป VCC |

## เตรียม Arduino
1. Arduino IDE → Boards Manager ติดตั้ง **Arduino UNO R4 Boards** เลือก **UNO R4 WiFi** และ Port
2. Library Manager: **MQTT by Joel Gaehwiler (256dpi)** (`MQTTClient.h`), **ArduinoJson 7**, **LiquidCrystal I2C by Frank de Brabander** (`init/backlight`)
3. Keypad เพิ่ม **Keypad by Mark Stanley / Alexander Brevig**; DHT เพิ่ม **DHT sensor library by Adafruit** และ **Adafruit Unified Sensor**
4. WiFiS3/Wire มากับ board package; เปิด .ino ในโฟลเดอร์อุปกรณ์พร้อม Statistics.h
5. แก้ WIFI_SSID/WIFI_PASS และ BROKER/PORT หากระบบห้องเรียนเปลี่ยน; Upload แล้วเปิด Serial 115200
6. LCD I2C 16×2: VCC→5V, GND→GND, SDA→SDA/A4, SCL→SCL/A5; address 0x27 (แก้ตามโมดูล) ปรับ contrast

## เปิดเว็บรับ–ส่งจริง
ใช้เว็บพร้อม bridge ใน [MQTT/web](../MQTT/web) สำหรับทั้งสองโหมด:

```sh
cd MQTT/web
npm install
npm start
```

เปิด http://127.0.0.1:3000 บนเครื่องที่รัน Node.js 20+ → เลือกโหมด `MQTT` หรือ `กันเหลี่ยม` และชื่ออุปกรณ์ → รอ telemetry → ส่ง JSON คำสั่ง → ดู ACK จริงและ Serial/LCD
คอมพิวเตอร์และบอร์ดต้องเข้าถึง broker เดียวกันได้; broker ห้องเรียนอาจต้องเครือข่ายมหาวิทยาลัย/VPN ไม่ได้ยืนยันว่าเข้าจากอินเทอร์เน็ตภายนอกได้
เปลี่ยน URL bridge ด้วย env `MQTT_URL=mqtt://host:1883` และแก้ BROKER ใน sketch ให้ตรงกัน
GitHub Pages เปิดเฉพาะ HTML ไม่ได้ทำให้ Node bridge รัน ต้องใช้ localhost นี้หรือ host backend เอง (หากเปิดสาธารณะต้องเพิ่ม auth/TLS/ACL) ไม่มีการสมมติ WebSocket port ของ broker

## Protocol
- prefix โหมดปกติ `68070131/mqtt/<sensor>`; สถิติ `68070131/stats/<sensor>`
- บอร์ดส่ง `/telemetry`; รับ `/command`; ตอบ `/ack` topic ตัวพิมพ์ต้องตรง
- ตัวอย่างคำสั่ง `{"text":"Hello LCD","interval_ms":2500,"reset":false}`
- text ต้อง string; interval_ms เป็นจำนวนเต็ม 2500–60000; reset เป็น boolean และ true ล้างสถิติทั้งสองช่อง; ข้อมูลผิดไม่เปลี่ยนค่าตั้ง
- Serial แสดง payload เต็ม; LCD แสดง `WEB RX` และ text 16 ตัวแรก 5 วินาที แล้วกลับค่าเซนเซอร์ ข้อความใช้ ASCII บน LCD
- ไม่ส่ง retained command; ack เป็นผลรับล่าสุด ไม่มี request ID จึงทดสอบส่งทีละคำสั่ง
- `valid=false,value=null` หมายถึงอ่านไม่สำเร็จ; offline อ่านและแสดงผลต่อแต่ไม่ได้เก็บข้อมูลส่งย้อนหลัง
- DHT ส่ง telemetry สองข้อความ channel อุณหภูมิและ humidity; อย่านำค่าต่างหน่วยมาปนกัน

## สถิติและตัวแปร
สถิติใช้ **20 ค่าที่ valid ล่าสุดต่อช่อง** เริ่มคำนวณได้ตั้งแต่ค่าแรก ไม่ใช่สถิติตั้งแต่เปิดบอร์ด:
`min` ต่ำสุด, `max` สูงสุด, `avg = sum/N`, `median` ค่ากลางเมื่อเรียง (N คู่เฉลี่ยคู่กลาง), `std = sqrt(sum((x-avg)^2)/N)` เป็น population std, `n` จำนวนค่าจริง N≤20
เมื่อ n=1 std=0; ไม่รวม NaN/timeout; reset แล้วค่ารอบถัดไปจะเริ่ม n=1
อ่านชื่อ/หน้าที่ตัวแปรในคอมเมนต์แต่ละ .ino และ Statistics.h; แยกฟังก์ชันรับคำสั่ง, ต่อเน็ต, อ่านเซนเซอร์, ส่งข้อมูลและ LCD ชัดเจน
Keypad สถิติจาก ASCII ของปุ่มใหม่ ไม่ใช่อุณหภูมิ; Button/PIR สถิติ 0/1 ค่า avg คือสัดส่วนสถานะ 1; LCD/RGB/LED/7-segment เป็น output จึงไม่มีค่าที่อ่านมาทำ sensor statistics

## แหล่งรายการเซนเซอร์ใน repository
- GEN_Z/04_PIR.txt, 05_POTENTIOMETER.txt, 08_TMP36.txt และ EXERCISES_FROM_SLIDES: PIR/Pot/TMP36/Ultrasonic/Keypad
- Phycom lab EXAM/TMP.txt, thermistor.txt, Ultrasonic + button.txt: TMP36/NTC/Button
- Phycom lab EXAM/Noah_Ark-main/Kt/arduino/W14_2_Temperature_MCP9700: MCP9700
- GEN_Z/WEB/library-and-sensors.html: เพิ่ม LM35/LDR/DHT11/DHT22 ตามเอกสารใน repo
NTC ในชุดนี้กำหนดวงจร NTC→5V, R10k→GND, B3950/R25=10k; TTC05103 ในไฟล์เก่าใช้ R330 และอีกทิศวงจร ต้องดูรุ่น/ค่า Beta จริงและปรับวงจร/สูตรก่อนใช้ ไม่ใช้แทนกันตรง ๆ

## ตรวจการทำงาน
- หมุน Pot / ปิดบัง LDR → value เปลี่ยน; วาง Ultrasonic ไม่มี echo → valid=false ไม่เพิ่ม n
- คงค่าเดิม 20 รอบ → min=max=avg=median, std≈0 (ความคลาด ADC ทำให้ไม่ตรงเป๊ะ)
- JSON ผิดชนิด/interval นอกช่วง → ACK error; ค่าตั้งเดิมไม่เปลี่ยน
- ปิด Wi-Fi แล้วเปิดใหม่ → อ่านต่อ/reconnect/resubscribe; ส่งคำสั่งใหม่ต้องแสดง LCD ได้
- ข้อมูลเริ่มต้น 1,2,3,4 → min=1,max=4,avg=2.5,median=2.5,std≈1.118034

## แหล่ง API
[WiFiS3](https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples/) · [ADC](https://docs.arduino.cc/tutorials/uno-r4-wifi/adc-resolution/) · [MQTTClient](https://github.com/256dpi/arduino-mqtt) · [ArduinoJson 7](https://arduinojson.org/v7/)

โหมดสถิติ: LCD หมุนหน้า min/max/avg/median/std ทีละรอบอ่าน ส่วน Serial/เว็บแสดงสถิติครบใน JSON เดียวกัน

## การทดสอบอัตโนมัติ
```sh
cd MQTT/web
npm ci
npm test
cd ../..
g++ -std=c++11 MQTT/tests/statistics.cpp -o /tmp/statistics-test
/tmp/statistics-test
```
คำสั่ง g++ ให้รันจาก root ของ repository ทดสอบ min/max/avg/median/std, หน้าต่างเต็มและ reset จาก Statistics.h จริง; bridge test ตรวจ HTTP/JSON/เป้าหมายผิด/broker offline โดยไม่ส่งเข้า broker ห้องเรียน
ยังไม่ได้ compile ด้วย Arduino toolchain หรือทดสอบบอร์ด/LCD/broker จริงในสภาพแวดล้อมนี้

ทดสอบ debounce ปุ่มเพิ่มจาก root repo:
```sh
g++ -std=c++11 MQTT/tests/button.cpp -o /tmp/button-test
/tmp/button-test
```
