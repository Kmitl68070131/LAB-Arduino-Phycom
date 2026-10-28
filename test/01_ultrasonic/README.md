# Test Ultrasonic HC-SR04

**หน้าที่:** ส่งพัลส์ TRIG 10 µs แล้ววัด ECHO เป็นไมโครวินาที เมื่อ timeout ให้ NO_ECHO ไม่ใช่ 0 cm

## ต่อสาย
- VCC → 5V, GND → GND
- TRIG → D2, ECHO → D3

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** วัตถุห่าง ~20 cm แสดงระยะใกล้ 20 cm และค่าเปลี่ยนเมื่อขยับ

## MQTT
- Publish: `68070131/test/ultrasonic/data`
- Subscribe: `68070131/test/ultrasonic/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/ultrasonic/status`

ดูไฟล์ [01_ultrasonic.ino](01_ultrasonic.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
