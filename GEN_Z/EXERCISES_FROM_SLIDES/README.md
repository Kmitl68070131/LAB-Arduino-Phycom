# 📚 แบบฝึกหัด Arduino จากสไลด์ + MQTT Website

**Arduino UNO R4 WiFi** · **Wi-Fi SSID: EDOK** · **MQTT: phycom.it.kmitl.ac.th:1883** · **Topic: 68070131/**

> ตั้งค่า `WIFI_PASS` ภายใน Arduino IDE ก่อนอัปโหลด (ไม่ควรเก็บรหัสผ่านจริงใน GitHub สาธารณะ)

## รายการไฟล์ (เปิดด้วย Notepad ได้)
| ข้อ | ชื่อไฟล์ | ใช้ทำอะไร | อุปกรณ์ |
|---|---|---|---|
| 1 | [01_7SEG_0to9_Arduino.txt](01_7SEG_0to9_Arduino.txt) | 7-Segment นับเลข 0–9 | 7-Segment |
| 2 | [02_7SEG_STUDENT_ID_Arduino.txt](02_7SEG_STUDENT_ID_Arduino.txt) | 7-Segment แสดงรหัสทีละคู่ | 7-Segment |
| 3 | [03_LED_FADING_4_Arduino.txt](03_LED_FADING_4_Arduino.txt) | LED PWM Fading สี่รูปแบบ | LED 4 ดวง |
| 4 | [04_KEYPAD_TO_7SEG_Arduino.txt](04_KEYPAD_TO_7SEG_Arduino.txt) | Keypad กดเลขแสดง 7-Segment | Keypad+7-Segment |
| 5 | [05_LCD_HEART_Arduino.txt](05_LCD_HEART_Arduino.txt) | LCD Custom Font หัวใจ | LCD I2C |
| 6 | [06_TMP36_LCD_Arduino.txt](06_TMP36_LCD_Arduino.txt) | TMP36 แสดงอุณหภูมิบน LCD | TMP36+LCD |
| 7 | [07_MQTT_KEYPAD_TX_Arduino.txt](07_MQTT_KEYPAD_TX_Arduino.txt) | MQTT Keypad ส่งปุ่มขึ้นเว็บ | Keypad |
| 8 | [08_MQTT_KEYPAD_RX_Arduino.txt](08_MQTT_KEYPAD_RX_Arduino.txt) | MQTT Keypad รับปุ่มจากเว็บ | LED |
| 9 | [09_MQTT_POT_TX_Arduino.txt](09_MQTT_POT_TX_Arduino.txt) | MQTT Potentiometer ส่ง Analog | Potentiometer |
| 10 | [10_MQTT_LED_FADE_RX_Arduino.txt](10_MQTT_LED_FADE_RX_Arduino.txt) | MQTT LED Fading รับค่า Analog | LED PWM |
| 11 | [11_MQTT_RGB_WEB_Arduino.txt](11_MQTT_RGB_WEB_Arduino.txt) | MQTT RGB ควบคุมสีจากเว็บ | RGB LED |
| 12 | [12_ULTRASONIC_WEB_Arduino.txt](12_ULTRASONIC_WEB_Arduino.txt) | Ultrasonic วัดระยะขึ้นเว็บ | HC-SR04 |

## การใช้งาน
1. เปิดไฟล์ .txt ของแต่ละข้อ คัดลอกไปวางใน Arduino IDE (แยก Sketch)
2. เปลี่ยน `WIFI_PASS` เป็นรหัสของเครือข่าย EDOK
3. ติดตั้งไลบรารี **MQTTClient by 256dpi**, **WiFiS3** และ **LiquidCrystal_I2C** สำหรับข้อที่ใช้ LCD
4. ต่อสายตามคอมเมนต์ด้านบนไฟล์ ตรวจขาจริงของเซนเซอร์และชนิด 7-Segment
5. อัปโหลดแล้วเปิด Serial Monitor 115200 baud
6. เว็บ Dashboard: [GEN_Z/WEB/dashboard.html](../WEB/dashboard.html) ต้องมี MQTT TCP-to-Web Bridge ที่ใช้งานได้ จึงจะดูข้อมูลสดได้

## คู่ MQTT
- Keypad ส่งปุ่ม: `07_MQTT_KEYPAD_TX` และตัวรับ `08_MQTT_KEYPAD_RX`
- Potentiometer ส่ง Analog: `09_MQTT_POT_TX` และตัวรับ LED PWM `10_MQTT_LED_FADE_RX`

**ข้อควรทราบ:** ไฟล์ตัวรับต้องตั้ง `MQTT_TOPIC` ให้ตรงกับตัวส่งก่อนใช้งานคู่กัน; หากใช้บอร์ด 2 ตัวให้ตั้ง Client ID ไม่ซ้ำกัน

## ข้อจำกัดของตัวอย่าง
- แบบฝึกหัด 7-Segment สองหลักจริงต้องมีวงจร Digit Enable และสแกน Multiplex; ตัวอย่างแสดงรหัสทีละคู่ใน MQTT และเลขบนหลักเดียว
- โค้ดบางข้อเป็นตัวอย่างเริ่มต้นที่ต้องปรับตามการต่อสายจริง
- GitHub Pages ไม่รองรับ MQTT TCP port 1883 โดยตรง ต้องใช้ Bridge หรือ MQTT over WebSocket ที่ Broker เปิดใช้งานจริง
