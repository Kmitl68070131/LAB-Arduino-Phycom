# Phycom MOCK Test 2-1-66

เว็บฝึกสอบ Physical Computing ปีการศึกษา 2566 สร้างตามภาพตัวอย่าง (ไม่ใช่เว็บสอบอย่างเป็นทางการ)

## เปิดเว็บ

เปิด index.html ด้วย Browser หรือ GitHub Pages ตาม URL:

https://kmitl68070131.github.io/LAB-Arduino-Phycom/Phycom%20MOCK%20Test%202-1-66/

หมายเหตุ: URL ใช้งานได้เมื่อเปิด GitHub Pages สำหรับ Repository นี้ (main / root) แล้วเท่านั้น

## ระบบ

1. กรอกรหัสนักศึกษาเป็นตัวเลข 8 หลัก ระบบบันทึกใน sessionStorage เฉพาะแท็บปัจจุบัน
2. หน้าแรกแสดงคำแนะนำและเมนู
3. MQTT Client: Connect/Disconnect, WSS หรือ WS, Path, Username/Password, Last Will, Publish QoS/Retain, Subscribe/Unsubscribe, Messages 500 รายการ
4. Mock Exam: Connect ไป phycom.it.kmitl.ac.th ผ่าน WSS 8884 โดยใช้ MQTT Topic ที่ขึ้นต้นด้วยรหัสนักศึกษา
5. 4 การ์ดด้านบนเป็นฝั่งบอร์ด Publish (`temp`, `light`, `food`, `door`) เว็บ Subscribe ค่าจริงแล้วแสดงสถานะและ progress 0/4
6. 2 การ์ดด้านล่างเป็นฝั่งบอร์ด Subscribe (`venus`, `prob_stat`) **เว็บ Publish ให้บอร์ดอัตโนมัติทุก 3 วินาที** หลังจาก Mock Exam เชื่อมต่อ
7. เว็บส่งค่า 19, 31, 46 °C วนซ้ำให้ RGB แสดงเขียว/ฟ้า/แดง และตัวเลข 0–9 วนซ้ำให้ 7-segment; การ์ดแสดงค่าที่ Publish สำเร็จ
8. Disconnect, ปิดแท็บ, หรือ MQTT ขาดการเชื่อมต่อ จะหยุดส่ง; เมื่อเชื่อมต่อใหม่จะเริ่มส่งอีกครั้ง

## Topics — เปลี่ยนตัวอย่าง 68070131 เป็นรหัสที่กรอก

| ฝั่ง Arduino | MQTT Topic | เงื่อนไข |
| --- | --- | --- |
| Publish | 68070131/temp | ค่าตัวเลขอุณหภูมิ เช่น 23.6 |
| Publish | 68070131/light | Potentiometer จำนวนเต็ม 0–1023 |
| Publish | 68070131/food | Ultrasonic > 20 cm = Off, <= 20 cm = On |
| Publish | 68070131/door | Ultrasonic >= 20 cm และกดปุ่ม = On, นอกนั้น Off |
| Subscribe (บอร์ดรับ, เว็บส่ง) | 68070131/venus | เว็บ Publish 19, 31, 46 °C วนทุก 3 วินาที; 10–25 เขียว, 26–35 ฟ้า, 36–50 แดง |
| Subscribe (บอร์ดรับ, เว็บส่ง) | 68070131/prob_stat | เว็บ Publish ตัวเลข 0–9 วนทุก 3 วินาที |

อ่าน On/Off แบบไม่สนตัวพิมพ์ใหญ่เล็ก

Arduino UNO R4 WiFi ใช้ MQTT TCP host phycom.it.kmitl.ac.th port 1883 ตามโค้ดเดิมใน Phycom lab EXAM/ALL.txt

## การแสดงผลของการ์ดแต่ละข้อ

4 ข้อแรกแสดงค่าล่าสุดที่เว็บรับจากบอร์ด (MQTT Subscribe) ในช่องด้านขวา: อุณหภูมิ, ค่า 0–1023 พร้อมแถบ, Ultrasonic On/Off และประตู Opened/Closed
2 ข้อล่างแสดงค่าที่เว็บ Publish สำเร็จ: สี RGB ที่บอร์ดควรแสดงพร้อมค่าอุณหภูมิ และตัวเลข 7-Segment ที่เรนเดอร์เป็นเส้นติดตามเลข 0–9; ทุกการ์ดแสดง Payload ล่าสุดพร้อมทิศทางข้อมูล
หากไม่ได้ Connect หรือยังไม่มีข้อความจริง ช่องจะแสดงสถานะรอรับ/รอส่ง ไม่สร้างค่าปลอม; ตัวเลขในฝั่งส่งไม่ได้ยืนยันว่าบอร์ดแสดงสำเร็จ

## ความปลอดภัยและข้อจำกัด

- กรอกรหัสนักศึกษาเท่านั้น ไม่ใช่ KMITL SSO และไม่ได้ตรวจสอบตัวตนกับ Server
- เว็บไซต์ไม่มี Backend หรือฐานข้อมูล ไม่มีการให้คะแนนหรือจัดสอบจริง
- Broker ต้องรองรับ WebSocket/TLS และเข้าถึงได้จาก Browser ของผู้ใช้
- ข้อมูล MQTT อาจถูกอ่านหรือส่งโดยผู้ที่ทราบ Topic หาก Broker ไม่บังคับ Auth
- เมื่อกด Connect ใน Mock Exam เว็บจะ Publish ข้อมูล 2 topics จริงไปยัง Broker โดยอัตโนมัติ ใช้กับบอร์ดฝึกซ้อมเท่านั้น
- GitHub Pages เป็นเว็บ Static: **ต้องเปิดเว็บค้างไว้** และ Browser ไม่ถูกระงับการทำงานจึงจะส่งค่าต่อเนื่องได้; ไม่ใช่เซิร์ฟเวอร์เบื้องหลังตลอด 24 ชั่วโมง
- CDN mqtt.js ต้องใช้อินเทอร์เน็ต

ทุกไฟล์ของเว็บไซต์นี้อยู่ภายใน Phycom MOCK Test 2-1-66 เท่านั้น โดยไม่ได้แก้ไขไฟล์ของโฟลเดอร์อื่น