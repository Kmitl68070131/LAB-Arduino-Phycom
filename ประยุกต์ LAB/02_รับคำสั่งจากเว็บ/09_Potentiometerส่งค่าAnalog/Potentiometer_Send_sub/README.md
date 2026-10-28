# LAB 09 — Potentiometerส่งค่าAnalog

**รับคำสั่งจากเว็บ (Subscribe)** · Arduino UNO R4 WiFi

![ผังวงจร](../../../../assets/วงจร_LAB09.svg)

## การต่อสาย
- Potentiometer ขานอก→5V
- Potentiometer ขากลาง→A0
- Potentiometer ขานอก→GND
- LED บนบอร์ด→D13

อุปกรณ์: Potentiometer 10kΩ และ LED บนบอร์ด

## Topic
- **Publish:** ไม่ใช้
- **Subscribe:**  `68070131/applied/lab09/sub/set`
- **สถานะคำสั่ง:** ไม่ใช้

## วิธีทดสอบ
1. เปิด `Potentiometer_Send_sub.ino` ด้วย Arduino IDE (โฟลเดอร์ชื่อเดียวกับ Sketch)
2. เลือก **Arduino UNO R4 WiFi** ติดตั้ง WiFiS3 และ MQTT (256dpi)
3. ต่อสายตามผัง เปิด Serial 115200; ตรวจ Wi-Fi/MQTT
4. Publish ข้อความ `75` ไปที่ Topic /set
5. ผลที่คาดหวัง: **ปรับ Potentiometer ได้ 75% เว็บแสดง 75; ตั้งเกณฑ์ 75 LED ติดเมื่อค่า≥75**

> โค้ดต้องต่อกับ Broker ที่เข้าถึงได้ และต้องตรวจบนบอร์ดจริง ไม่ใช่ยืนยันว่าทดสอบฮาร์ดแวร์แล้ว
