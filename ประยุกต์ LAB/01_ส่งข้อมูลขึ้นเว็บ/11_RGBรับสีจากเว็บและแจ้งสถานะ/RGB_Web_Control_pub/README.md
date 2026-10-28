# LAB 11 — RGBรับสีจากเว็บและแจ้งสถานะ

**ส่งข้อมูลขึ้นเว็บ (Publish)** · Arduino UNO R4 WiFi

![ผังวงจร](../../../../assets/วงจร_LAB11.svg)

## การต่อสาย
- RGB Common Cathode→GND
- RGB R→D9 ผ่าน 330Ω
- RGB G→D10 ผ่าน 330Ω
- RGB B→D11 ผ่าน 330Ω

อุปกรณ์: RGB LED แบบ Common Cathode + R 330Ω จำนวน 3 ตัว

## Topic
- **Publish:**  `68070131/applied/lab11/pub/data`
- **Subscribe:** ไม่ใช้
- **สถานะคำสั่ง:** ไม่ใช้

## วิธีทดสอบ
1. เปิด `RGB_Web_Control_pub.ino` ด้วย Arduino IDE (โฟลเดอร์ชื่อเดียวกับ Sketch)
2. เลือก **Arduino UNO R4 WiFi** ติดตั้ง WiFiS3 และ MQTT (256dpi)
3. ต่อสายตามผัง เปิด Serial 115200; ตรวจ Wi-Fi/MQTT
4. อ่านค่า Publish ที่ Topic /data
5. ผลที่คาดหวัง: **เว็บได้รับค่าที่บอร์ดอ่านหรือสร้างขึ้น**

> โค้ดต้องต่อกับ Broker ที่เข้าถึงได้ และต้องตรวจบนบอร์ดจริง ไม่ใช่ยืนยันว่าทดสอบฮาร์ดแวร์แล้ว
