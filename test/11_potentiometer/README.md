# Test Potentiometer 10k

**หน้าที่:** หมุนปุ่มให้ ADC เปลี่ยนต่อเนื่อง 0–1023

## ต่อสาย
- ขานอกสองข้าง → 5V และ GND
- ขากลาง (wiper) → A0

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** หมุนจนสุดได้ค่าใกล้ 0–1023 และตรงกลางใกล้ 512

## MQTT
- Publish: `68070131/test/potentiometer/data`
- Subscribe: `68070131/test/potentiometer/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/potentiometer/status`

ดูไฟล์ [11_potentiometer.ino](11_potentiometer.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
