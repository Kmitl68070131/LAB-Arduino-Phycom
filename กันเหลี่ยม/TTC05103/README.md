# เทอร์มิสเตอร์ TTC05103 วงจรตัวต้านทาน 330 โอห์ม — สถิติค่าต่ำสุด สูงสุด เฉลี่ย มัธยฐาน และส่วนเบี่ยงเบนมาตรฐาน

[กลับคู่มือส่วนนี้](../README.md) · [หน้าแรก](../../README.md)

## ทำอะไร
อ่านค่าจากเทอร์มิสเตอร์ TTC05103 วงจรตัวต้านทาน 330 โอห์ม หน่วย `C` ส่ง JSON ขึ้นเว็บ และรับคำสั่ง JSON จากเว็บมาแสดง Serial/LCD ทุกชุดใช้ UNO R4 WiFi และอัปโหลดแยกทีละชุด
คำนวณ min/max/avg/median/population std แยกช่อง; DHT มีทั้งอุณหภูมิและความชื้น

## ต่อสาย
5V → R330Ω → A0 → TTC05103 → GND; R25=10kΩ, B4050 ตามไฟล์เดิม ต้องตรวจ datasheet จริง

LCD I2C 16×2: VCC → 5V, GND → GND, SDA → ขา SDA (A4), SCL → ขา SCL (A5) ใช้ address 0x27 หรือแก้เป็น 0x3F ตามโมดูล ปรับ contrast ก่อนใช้งาน ห้ามใช้ A4/A5 ซ้ำกับเซนเซอร์

## วิธีทำ
1. ทำตามการติดตั้งไลบรารีใน [คู่มือ](../README.md)
2. เปิด `TTC05103.ino` พร้อม `Statistics.h` ในโฟลเดอร์เดียวกัน
3. แก้ WIFI_SSID/WIFI_PASS; เลือก UNO R4 WiFi แล้ว Upload
4. เปิด Serial Monitor 115200; เปิดเว็บ bridge ตามคู่มือ เลือกโหมดและ `TTC05103`
5. เว็บ subscribe `68070131/stats/TTC05103/telemetry` และ `/ack`; publish คำสั่งที่ `68070131/stats/TTC05103/command`
6. ส่ง `{"text":"Hello LCD","interval_ms":2500,"reset":false}` จะแสดง WEB RX 5 วินาที พร้อม JSON เต็มใน Serial และ ack บนเว็บ
7. เปลี่ยนค่าเซนเซอร์แล้วดู telemetry; ถ้า offline จะบอก NOT SENT และ retry อัตโนมัติ (ไม่เก็บค้างย้อนหลัง)

## แยกส่วนโค้ดและตัวแปร
- `sensorSetup/readSensor`: ตั้งขา/อ่านค่า; `raw` = ADC 0–4095, `voltage` = V
- `networkTick`: reconnect Wi-Fi/MQTT และ subscribe ใหม่; `received`: ตรวจ JSON และแสดงข้อความ
- `emit`: แสดง Serial/LCD และ publish telemetry; `line`: เติมช่องว่างป้องกันตัวอักษรค้าง
- `WindowStats` ใน Statistics.h: ring buffer, เรียงสำเนาหา median, คำนวณ std; ทุกสมาชิกมีคอมเมนต์
- ค่าตั้งเครือข่าย, ตัวจับเวลา และตัวแปรช่วยมีคอมเมนต์ในไฟล์ .ino

## ความหมายข้อมูล
`value` = ค่าหน่วย `C`; `valid=false/value=null` = อ่านไม่สำเร็จ (ไม่นำเข้า stats)
`stats.n` = จำนวนค่าจริงสูงสุด 20; `min/max` ต่ำสุด/สูงสุด; `avg` ค่าเฉลี่ย; `median` มัธยฐาน; `std` ส่วนเบี่ยงเบนมาตรฐานประชากร
Keypad ใช้ ASCII เช่น '1'=49, 'A'=65, '#'=35; สถิติรหัสปุ่มไม่ใช่ค่ากายภาพ และเพิ่มเฉพาะตอนมีปุ่มใหม่ ส่วน Button/PIR เป็น 0/1, avg คือสัดส่วนสถานะ 1 ในหน้าต่าง Button อ่านแบบ polling ไม่ใช่ตัวนับการกด/ debounce

## ข้อจำกัด
ไม่รวม timeout ในสถิติ; Keypad ไม่มีปุ่มใหม่จะส่ง valid=false; PIR ต้องรอ warm-up
TTC05103 ชุดนี้ใช้ R330 ด้าน 5V และ NTC ด้าน GND สูตร R=330*ADC/(4095-ADC), B4050 ตามไฟล์ thermistor.txt; ตรวจ Beta ตามรุ่นจริง
DHT อ่านสองช่องแยกกัน; LCD แสดงช่องล่าสุด และข้อความเว็บใช้ ASCII (LCD ROM ทั่วไปไม่รองรับไทย)
