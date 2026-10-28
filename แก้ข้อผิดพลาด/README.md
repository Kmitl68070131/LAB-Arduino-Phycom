# แก้ข้อผิดพลาด Arduino / MQTT / Sensor

[🌐 เปิดเว็บค้นหา Error](https://kmitl68070131.github.io/LAB-Arduino-Phycom/แก้ข้อผิดพลาด/) · [🏠 กลับหน้าแรก](../index.html) · [📖 คู่มือคำสั่ง](../ที่อาจจะออกมั้ง/คำสั่งและเซนเซอร์/)

รวม **37 ปัญหา** ที่เกี่ยวกับการ Compile, Wi-Fi, MQTT, LCD I2C, HC-SR04, Switch, Keypad, ADC, TMP36, LDR, RGB, 7-Segment, Github Pages และพินชนกัน

## ใช้ยังไง
1. กดเปิดหน้าเว็บแล้วพิมพ์ข้อความ Error จริง เช่น `WiFi.macAddress`, `MQTT rc=-2`, `LCD ไม่ขึ้น`
2. เลือกหมวดปัญหาที่อยู่ด้านซ้ายเพื่อกรอง
3. อ่าน **สาเหตุ**, **วิธีแก้**, **ตัวอย่างโค้ด**
4. ถ้ามีปัญหา LCD หรือ MCP23017 ให้เปิด [I2C Scanner](I2C_Scanner/I2C_Scanner.ino) แล้วดู Serial Monitor ที่ 115200
5. ถ้า Error ตอน Compile ให้แก้ **บรรทัดแรกที่เป็น error** ก่อน ไม่ดูแค่ `exit status 1`

## เช็กลิสต์ก่อนสอบ
- Board Arduino UNO R4 WiFi / Port ถูกต้อง
- ติดตั้ง WiFiS3 (มาพร้อม core), MQTT by 256dpi, LiquidCrystal I2C และไลบรารีอุปกรณ์ตามที่ใช้
- ระบุ SSID, password, MQTT Broker ให้ตรง และ Topic ขึ้นต้น `68070131/`
- MQTT Publish/Subscribe มีทิศทางตรงโจทย์, `mqtt.loop()` ไม่ถูกบล็อก
- เซนเซอร์ที่ต้องใช้ตัวต้านทานต้องต่อให้ครบ และ GND ทุกอุปกรณ์ร่วมกัน
- แยกสถานะ **Compile ผ่าน**, **MQTT เชื่อมผ่าน**, **อุปกรณ์จริงทำงาน** อย่าถือว่าขั้นแรกเท่ากับขั้นสุดท้าย

## เชื่อมต่อเว็บไซต์
หากเว็บไซต์บน GitHub Pages ขึ้น 404 ให้ตรวจ [Settings → Pages](https://github.com/Kmitl68070131/LAB-Arduino-Phycom/settings/pages) เลือก `main / (root)`. การเปิดคู่มือ Markdown ใช้ลิงก์ GitHub โดยตรงได้เสมอ

รายการ Error เป็นคำแนะนำสำหรับแก้ปัญหาเบื้องต้น โค้ดใต้ Error เป็น **Snippet** ไม่ได้หมายความว่า Compile เดี่ยว ๆ ได้ทุกอัน
