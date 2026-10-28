# LAB 02 — 7เซกเมนต์แสดงรหัสนักศึกษา

**รับและส่งข้อมูลสองทาง** · Arduino UNO R4 WiFi

![ผังวงจร](../../../../assets/วงจร_LAB02.svg)

## การต่อสาย
- A→D2 (330Ω)
- B→D3 (330Ω)
- C→D4 (330Ω)
- D→D5 (330Ω)
- E→D6 (330Ω)
- F→D7 (330Ω)
- G→D8 (330Ω)
- COM→GND

อุปกรณ์: 7-Segment Common Cathode + ตัวต้านทาน 330Ω จำนวน 7 ตัว

## Topic
- **Publish:**  `68070131/applied/lab02/both/data`
- **Subscribe:**  `68070131/applied/lab02/both/set`
- **สถานะคำสั่ง:** `68070131/applied/lab02/both/state`

## วิธีทดสอบ
1. เปิด `SevenSeg_StudentID_both.ino` ด้วย Arduino IDE (โฟลเดอร์ชื่อเดียวกับ Sketch)
2. เลือก **Arduino UNO R4 WiFi** ติดตั้ง WiFiS3 และ MQTT (256dpi)
3. ต่อสายตามผัง เปิด Serial 115200; ตรวจ Wi-Fi/MQTT
4. Publish ข้อความ `3` ไปที่ Topic /set
5. ผลที่คาดหวัง: **เลือกตำแหน่ง 3 ของ 68070131 จะแสดงเลข 7** และ Topic /state ตอบ OK:3

> โค้ดต้องต่อกับ Broker ที่เข้าถึงได้ และต้องตรวจบนบอร์ดจริง ไม่ใช่ยืนยันว่าทดสอบฮาร์ดแวร์แล้ว
