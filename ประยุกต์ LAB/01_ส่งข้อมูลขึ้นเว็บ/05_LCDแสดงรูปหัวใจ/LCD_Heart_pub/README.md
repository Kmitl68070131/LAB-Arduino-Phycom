# LAB 05 — LCDแสดงรูปหัวใจ

**ส่งข้อมูลขึ้นเว็บ (Publish)** · Arduino UNO R4 WiFi

![ผังวงจร](../../../../assets/วงจร_LAB05.svg)

## การต่อสาย
- LCD VCC→5V
- LCD GND→GND
- LCD SDA→SDA
- LCD SCL→SCL

อุปกรณ์: LCD I2C 16×2 ที่อยู่ 0x27

## Topic
- **Publish:**  `68070131/applied/lab05/pub/data`
- **Subscribe:** ไม่ใช้
- **สถานะคำสั่ง:** ไม่ใช้

## วิธีทดสอบ
1. เปิด `LCD_Heart_pub.ino` ด้วย Arduino IDE (โฟลเดอร์ชื่อเดียวกับ Sketch)
2. เลือก **Arduino UNO R4 WiFi** ติดตั้ง WiFiS3 และ MQTT (256dpi) และ LiquidCrystal I2C
3. ต่อสายตามผัง เปิด Serial 115200; ตรวจ Wi-Fi/MQTT
4. อ่านค่า Publish ที่ Topic /data
5. ผลที่คาดหวัง: **เว็บได้รับค่าที่บอร์ดอ่านหรือสร้างขึ้น**

> โค้ดต้องต่อกับ Broker ที่เข้าถึงได้ และต้องตรวจบนบอร์ดจริง ไม่ใช่ยืนยันว่าทดสอบฮาร์ดแวร์แล้ว
