# LAB 10 — PWMหรี่ไฟด้วยค่าMQTT

**รับคำสั่งจากเว็บ (Subscribe)** · Arduino UNO R4 WiFi

![ผังวงจร](../../../../assets/วงจร_LAB10.svg)

## การต่อสาย
- LED ขาบวก→D9 ผ่าน 330Ω
- LED ขาลบ→GND

อุปกรณ์: LED + ตัวต้านทาน 330Ω

## Topic
- **Publish:** ไม่ใช้
- **Subscribe:**  `68070131/applied/lab10/sub/set`
- **สถานะคำสั่ง:** ไม่ใช้

## วิธีทดสอบ
1. เปิด `LED_PWM_Control_sub.ino` ด้วย Arduino IDE (โฟลเดอร์ชื่อเดียวกับ Sketch)
2. เลือก **Arduino UNO R4 WiFi** ติดตั้ง WiFiS3 และ MQTT (256dpi)
3. ต่อสายตามผัง เปิด Serial 115200; ตรวจ Wi-Fi/MQTT
4. Publish ข้อความ `200` ไปที่ Topic /set
5. ผลที่คาดหวัง: **LED ที่ D9 สว่างประมาณ 78% (200/255)**

> โค้ดต้องต่อกับ Broker ที่เข้าถึงได้ และต้องตรวจบนบอร์ดจริง ไม่ใช่ยืนยันว่าทดสอบฮาร์ดแวร์แล้ว
