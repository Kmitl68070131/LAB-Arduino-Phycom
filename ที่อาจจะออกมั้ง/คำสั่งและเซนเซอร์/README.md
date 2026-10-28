# ค้นหาคำสั่ง Arduino และเซนเซอร์

[เปิดเว็บค้นหาคำสั่ง](https://kmitl68070131.github.io/LAB-Arduino-Phycom/ที่อาจจะออกมั้ง/คำสั่งและเซนเซอร์/) · [กลับเว็บค้นหาโจทย์](../web/) · [กลับ README หลัก](../../README.md)

เว็บนี้รวมคำสั่งสำคัญของ Arduino UNO R4 WiFi ที่ใช้ทำ LAB และ Mock ทั้งขา Digital/Analog, เวลา, Serial, LCD I2C, MQTT, Wi-Fi, RGB/LED/7-Segment/LED Matrix, สวิตช์, Keypad, Ultrasonic, TMP36, MCP9700, LM35, NTC, TTC05103, DHT11, DHT22, LDR, PIR และ Potentiometer รวมถึง Array/Statistics

## ใช้ยังไง

1. เปิด [index.html](index.html) ผ่าน GitHub Pages หรือเว็บ Static Server
2. ค้นหา `analogRead`, `mqtt.publish`, `pulseIn`, `อุณหภูมิ` หรือชื่ออุปกรณ์
3. เลือก Checkbox ของเซนเซอร์ได้หลายตัวและเลือกการจับคู่แบบ **อย่างน้อยหนึ่งตัว** / **ทุกตัว**
4. กดขยายคำสั่งเพื่ออ่าน: **ใช้ทำอะไร / ต้องเตรียมอะไร / ตัวอย่างโค้ด / จุดพลาด**
5. กด **เปิด Sketch ตัวอย่างใน GitHub** เพื่อดูไฟล์เต็มของอุปกรณ์นั้น
6. หากเปลี่ยน Topic ที่กำหนดใน Sketch ให้แก้ Topic ฝั่ง Publisher/Subscriber ที่ใช้จริงให้ตรงกัน

## คำสั่งที่ควรจำก่อนสอบ

| เรื่อง | คำสั่งสำคัญ | ใช้ทำอะไร |
|---|---|---|
| Digital | `pinMode`, `digitalRead`, `digitalWrite`, `INPUT_PULLUP` | ปุ่ม/LED |
| Analog | `analogRead`, `analogReadResolution`, `analogWrite`, `map`, `constrain` | TMP36/LDR/Pot/PWM |
| เวลา | `millis`, `delayMicroseconds`, `pulseIn` | Debounce/HC-SR04 |
| MQTT | `mqtt.begin`, `connect`, `subscribe`, `publish`, `loop`, `onMessage` | ส่ง/รับข้อมูลเว็บ |
| Wi-Fi | `WiFi.begin`, `WiFi.status`, `WiFi.localIP` | เข้าเครือข่าย |
| LCD | `Wire.begin`, `lcd.init`, `backlight`, `setCursor`, `print`, `createChar` | แสดงผล |
| DHT11/22 | `DHT`, `dht.begin`, `readTemperature`, `readHumidity`, `isnan` | อุณหภูมิ/ความชื้น |
| Keypad | `makeKeymap`, `getKey`, `setDebounceTime` | อ่านปุ่ม 4×4 |
| Array | `for`, `min`, `max`, `sqrt`, `log` | หลายเซนเซอร์/สูตร |

**ตัวอย่างต่อสาย/สูตรที่สำคัญ:** LCD SDA/SCL, Pull-up กับ Pull-down, 7-Segment Common Cathode/Anode, RGB Common Cathode/Anode, NTC R10k หรือ R330 และ ADC 10/12 bit อธิบายในหน้าเว็บแยกตามหมวด

## MQTT ที่ใช้ใน LAB

- Wi-Fi SSID: `EDOK`
- Broker: `phycom.it.kmitl.ac.th` (MQTT TCP `1883`)
- Topic Prefix: `68070131/`
- ข้อที่ใช้บนเว็บทดสอบต้องระบุให้ตรงว่า **เว็บ Publish/บอร์ด Subscribe** หรือ **บอร์ด Publish/เว็บ Subscribe**
- GitHub Pages เป็น Static Website และ **ไม่สามารถเปิด MQTT TCP 1883 ตรง ๆ** หากต้องการรับส่งจริงให้ใช้เว็บที่รองรับ Broker WebSocket หรือ Node.js Bridge

## คำสั่งตัวอย่าง VS Sketch เต็ม

ตัวอย่างใต้แต่ละคำสั่งเป็น **Code Snippet** ไม่ใช่ทุกอันที่วางลง Arduino IDE แล้ว Compile ได้เดี่ยว ๆ ต้องประกาศตัวแปร/ไลบรารีและวางใน setup() / loop() ให้เหมาะสม ดูตัวอย่าง Sketch เต็มจากลิงก์ของหมวดนั้น

เว็บนี้เป็นคู่มือสำหรับรายการที่พบใน Repository และคำสั่งพื้นฐานที่เกี่ยวข้อง ไม่ใช่รายการฟังก์ชันทุกตัวจากทุก Arduino Library ที่มีอยู่ในโลก

แหล่งอ้างอิง: [Arduino Language Reference](https://docs.arduino.cc/language-reference/), [Arduino UNO R4 WiFi](https://docs.arduino.cc/hardware/uno-r4-wifi/), [MQTTClient by 256dpi](https://github.com/256dpi/arduino-mqtt), [Adafruit DHT Library](https://github.com/adafruit/DHT-sensor-library).
