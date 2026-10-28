# Test Push Button INPUT_PULLUP

**หน้าที่:** ทดสอบอ่านปุ่ม Pull-up และนับขอบการกดหนึ่งครั้งต่อการกด

## ต่อสาย
- D2 → สวิตช์ NO → GND
- ไม่ต้องใช้ R ภายนอก; กด = LOW, ปล่อย = HIGH

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** กดปุ่มแสดง PRESSED แล้วปล่อยแสดง RELEASED

## MQTT
- Publish: `68070131/test/switch_pullup/data`
- Subscribe: `68070131/test/switch_pullup/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/switch_pullup/status`

ดูไฟล์ [12_switch_pullup.ino](12_switch_pullup.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
