# GEN_Z — Arduino Sensor Lab

ตัวอย่าง Arduino UNO R4 WiFi และ MQTT สำหรับฝึก Physical Computing

- Wi-Fi SSID: EDOK
- Wi-Fi Password: **ตั้งค่าใน Arduino IDE ก่อนอัปโหลด (อย่าเผยแพร่รหัสผ่านใน GitHub สาธารณะ)**
- MQTT host: phycom.it.kmitl.ac.th
- MQTT port: 1883
- MQTT Topic prefix: 68070131/

## ตัวอย่างที่อัปโหลดแล้ว

- [PIR](04_PIR.txt) — 68070131/pir
- [Potentiometer](05_POTENTIOMETER.txt) — 68070131/pot
- [TMP36](08_TMP36.txt) — 68070131/tmp36
- [7-Segment](12_7_SEGMENT.txt) — 68070131/sevenseg

## วิธีใช้

1. เปิดไฟล์ .txt ด้วย Notepad แล้วคัดลอกไป Arduino IDE
2. แก้ WIFI_SSID และ WIFI_PASS ให้ตรงกับเครือข่ายที่ใช้
3. ตรวจสายและ Library ตามคอมเมนต์ในแต่ละไฟล์
4. เลือก Arduino UNO R4 WiFi แล้ว Upload
5. เปิด Serial Monitor ที่ 115200 baud

## เว็บ

[เปิดคู่มือแบบเลือกแท็บ](../index.html) · [ไฟล์ LAB เดิม](../Phycom%20lab%20EXAM/)

หมายเหตุ: การเชื่อมต่อจากเว็บเบราว์เซอร์ต้องใช้ MQTT over WebSocket หรือ Bridge ที่รันบน Server เพราะ MQTT TCP 1883 ไม่สามารถต่อจาก JavaScript ในเบราว์เซอร์โดยตรงได้


## 🖼️ ดูรูปและเปิดเว็บตัวอย่าง
- [เว็บคู่มือหลัก](https://kmitl68070131.github.io/LAB-Arduino-Phycom/)
- [แกลเลอรีพร้อมรูปอุปกรณ์ทั้ง 12 ข้อ](https://kmitl68070131.github.io/LAB-Arduino-Phycom/GEN_Z/WEB/example-gallery.html)
- [เว็บ MQTT Exercises](https://kmitl68070131.github.io/LAB-Arduino-Phycom/GEN_Z/WEB/exercises.html)

**หมายเหตุ:** ลิงก์ GitHub ที่ลงท้าย `.html` จะเปิดไฟล์โค้ด ไม่ใช่เว็บไซต์ ต้องเปิดผ่าน `github.io` ตามลิงก์ด้านบน
