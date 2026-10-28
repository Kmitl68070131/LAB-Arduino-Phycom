# LAB 09 — Potentiometerส่งค่าAnalog

**ส่งข้อมูลขึ้นเว็บ (Publish)** · Arduino UNO R4 WiFi

![ผังวงจร](../../../../assets/วงจร_LAB09.svg)

## การต่อสาย
- Potentiometer ขานอก→5V
- Potentiometer ขากลาง→A0
- Potentiometer ขานอก→GND
- LED บนบอร์ด→D13

อุปกรณ์: Potentiometer 10kΩ และ LED บนบอร์ด

## Topic
- **Publish:**  `68070131/applied/lab09/pub/data`
- **Subscribe:** ไม่ใช้
- **สถานะคำสั่ง:** ไม่ใช้

## วิธีทดสอบ
1. เปิด `Potentiometer_Send_pub.ino` ด้วย Arduino IDE (โฟลเดอร์ชื่อเดียวกับ Sketch)
2. เลือก **Arduino UNO R4 WiFi** ติดตั้ง WiFiS3 และ MQTT (256dpi)
3. ต่อสายตามผัง เปิด Serial 115200; ตรวจ Wi-Fi/MQTT
4. อ่านค่า Publish ที่ Topic /data
5. ผลที่คาดหวัง: **เว็บได้รับค่าที่บอร์ดอ่านหรือสร้างขึ้น**

> โค้ดต้องต่อกับ Broker ที่เข้าถึงได้ และต้องตรวจบนบอร์ดจริง ไม่ใช่ยืนยันว่าทดสอบฮาร์ดแวร์แล้ว
