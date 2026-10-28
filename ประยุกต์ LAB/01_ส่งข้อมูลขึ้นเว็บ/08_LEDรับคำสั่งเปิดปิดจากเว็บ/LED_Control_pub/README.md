# LAB 08 — LEDรับคำสั่งเปิดปิดจากเว็บ

**ส่งข้อมูลขึ้นเว็บ (Publish)** · Arduino UNO R4 WiFi

![ผังวงจร](../../../../assets/วงจร_LAB08.svg)

## การต่อสาย
- ใช้ LED ใน Arduino ที่ D13
- ไม่ต้องต่ออุปกรณ์เพิ่ม

อุปกรณ์: LED Built-in ของ Arduino UNO R4 WiFi

## Topic
- **Publish:**  `68070131/applied/lab08/pub/data`
- **Subscribe:** ไม่ใช้
- **สถานะคำสั่ง:** ไม่ใช้

## วิธีทดสอบ
1. เปิด `LED_Control_pub.ino` ด้วย Arduino IDE (โฟลเดอร์ชื่อเดียวกับ Sketch)
2. เลือก **Arduino UNO R4 WiFi** ติดตั้ง WiFiS3 และ MQTT (256dpi)
3. ต่อสายตามผัง เปิด Serial 115200; ตรวจ Wi-Fi/MQTT
4. อ่านค่า Publish ที่ Topic /data
5. ผลที่คาดหวัง: **เว็บได้รับค่าที่บอร์ดอ่านหรือสร้างขึ้น**

> โค้ดต้องต่อกับ Broker ที่เข้าถึงได้ และต้องตรวจบนบอร์ดจริง ไม่ใช่ยืนยันว่าทดสอบฮาร์ดแวร์แล้ว
