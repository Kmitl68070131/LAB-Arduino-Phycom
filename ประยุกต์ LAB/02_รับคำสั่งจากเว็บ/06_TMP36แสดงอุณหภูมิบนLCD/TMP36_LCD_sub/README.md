# LAB 06 — TMP36แสดงอุณหภูมิบนLCD

**รับคำสั่งจากเว็บ (Subscribe)** · Arduino UNO R4 WiFi

![ผังวงจร](../../../../assets/วงจร_LAB06.svg)

## การต่อสาย
- TMP36 Vs→5V
- TMP36 Vout→A3
- TMP36 GND→GND
- LCD VCC/GND→5V/GND
- LCD SDA/SCL→SDA/SCL
- LED บนบอร์ด→D13

อุปกรณ์: TMP36 (ตรวจ pinout จาก datasheet) + LCD I2C

## Topic
- **Publish:** ไม่ใช้
- **Subscribe:**  `68070131/applied/lab06/sub/set`
- **สถานะคำสั่ง:** ไม่ใช้

## วิธีทดสอบ
1. เปิด `TMP36_LCD_sub.ino` ด้วย Arduino IDE (โฟลเดอร์ชื่อเดียวกับ Sketch)
2. เลือก **Arduino UNO R4 WiFi** ติดตั้ง WiFiS3 และ MQTT (256dpi) และ LiquidCrystal I2C
3. ต่อสายตามผัง เปิด Serial 115200; ตรวจ Wi-Fi/MQTT
4. Publish ข้อความ `30` ไปที่ Topic /set
5. ผลที่คาดหวัง: **ตั้งเกณฑ์ 30°C; เมื่อวัดได้ตั้งแต่ 30°C ไฟ LED บนบอร์ดติด**

> โค้ดต้องต่อกับ Broker ที่เข้าถึงได้ และต้องตรวจบนบอร์ดจริง ไม่ใช่ยืนยันว่าทดสอบฮาร์ดแวร์แล้ว
