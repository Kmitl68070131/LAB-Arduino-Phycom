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
4. Mock Exam: Connect ไป phycom.it.kmitl.ac.th WebSocket 8884 และฟัง topics ตามรหัสนักศึกษา
5. ส่วน Publish ตรวจรูปแบบข้อมูลและแสดงเวลาตั้งแต่รับข้อความล่าสุด, progress 0/4
6. ส่วน Subscribe แสดงการจำลอง RGB และจอ 7-Segment ตามค่าที่ได้รับจริง
7. สวิตช์จำลอง Server จะ Publish ค่าบน 2 topics ทุก 3 วินาที (ปิดไว้เป็นค่าเริ่มต้น)

## Topics — เปลี่ยนตัวอย่าง 68070131 เป็นรหัสที่กรอก

| ฝั่ง Arduino | MQTT Topic | เงื่อนไข |
| --- | --- | --- |
| Publish | 68070131/temp | ค่าตัวเลขอุณหภูมิ เช่น 23.6 |
| Publish | 68070131/light | Potentiometer จำนวนเต็ม 0–1023 |
| Publish | 68070131/food | Ultrasonic > 20 cm = Off, <= 20 cm = On |
| Publish | 68070131/door | Ultrasonic >= 20 cm และกดปุ่ม = On, นอกนั้น Off |
| Subscribe | 68070131/venus | 10–25 เขียว, 26–35 ฟ้า, 36–50 แดง |
| Subscribe | 68070131/prob_stat | ตัวเลข 0–9 |

อ่าน On/Off แบบไม่สนตัวพิมพ์ใหญ่เล็ก

Arduino UNO R4 WiFi ใช้ MQTT TCP host phycom.it.kmitl.ac.th port 1883 ตามโค้ดเดิมใน Phycom lab EXAM/ALL.txt

## ความปลอดภัยและข้อจำกัด

- กรอกรหัสนักศึกษาเท่านั้น ไม่ใช่ KMITL SSO และไม่ได้ตรวจสอบตัวตนกับ Server
- เว็บไซต์ไม่มี Backend หรือฐานข้อมูล ไม่มีการให้คะแนนหรือจัดสอบจริง
- Broker ต้องรองรับ WebSocket/TLS และเข้าถึงได้จาก Browser ของผู้ใช้
- ข้อมูล MQTT อาจถูกอ่านหรือส่งโดยผู้ที่ทราบ Topic หาก Broker ไม่บังคับ Auth
- โหมดจำลองเป็นการส่งข้อมูลจริงจาก Browser ไปยัง Broker เมื่อเปิดใช้งาน จึงควรใช้เพื่อฝึกซ้อมเท่านั้น
- CDN mqtt.js ต้องใช้อินเทอร์เน็ต

ทุกไฟล์ของเว็บไซต์นี้อยู่ภายใน Phycom MOCK Test 2-1-66 เท่านั้น โดยไม่ได้แก้ไขไฟล์ของโฟลเดอร์อื่น