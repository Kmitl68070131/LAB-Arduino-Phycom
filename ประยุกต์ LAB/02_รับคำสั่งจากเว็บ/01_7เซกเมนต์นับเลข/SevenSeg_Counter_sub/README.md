# LAB 01 — 7เซกเมนต์นับเลข

**รับคำสั่งจากเว็บ (Subscribe)** · Arduino UNO R4 WiFi

![ผังวงจร](../../../../assets/วงจร_LAB01.svg)

## การต่อสาย
- A→D2 (330Ω)
- B→D3 (330Ω)
- C→D4 (330Ω)
- D→D5 (330Ω)
- E→D6 (330Ω)
- F→D7 (330Ω)
- G→D8 (330Ω)
- COM→GND (Common Cathode)

อุปกรณ์: 7-Segment Common Cathode + ตัวต้านทาน 330Ω จำนวน 7 ตัว

## Topic
- **Publish:** ไม่ใช้
- **Subscribe:**  `68070131/applied/lab01/sub/set`
- **สถานะคำสั่ง:** ไม่ใช้

## วิธีทดสอบ
1. เปิด `SevenSeg_Counter_sub.ino` ด้วย Arduino IDE (โฟลเดอร์ชื่อเดียวกับ Sketch)
2. เลือก **Arduino UNO R4 WiFi** ติดตั้ง WiFiS3 และ MQTT (256dpi)
3. ต่อสายตามผัง เปิด Serial 115200; ตรวจ Wi-Fi/MQTT
4. Publish ข้อความ `7` ไปที่ Topic /set
5. ผลที่คาดหวัง: **หน้าจอ 7-Segment แสดงเลข 7**

> โค้ดต้องต่อกับ Broker ที่เข้าถึงได้ และต้องตรวจบนบอร์ดจริง ไม่ใช่ยืนยันว่าทดสอบฮาร์ดแวร์แล้ว
