# Test Push Button External PULLDOWN

**หน้าที่:** ทดสอบ Pull-down ภายนอก (UNO R4 ใช้ INPUT + R ภายนอก) และนับการกด

## ต่อสาย
- 5V → สวิตช์ NO → D2
- D2 → R10kΩ → GND; กด = HIGH, ปล่อย = LOW

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** ปล่อยปุ่มอ่าน LOW และกดอ่าน HIGH

## MQTT
- Publish: `68070131/test/switch_pulldown/data`
- Subscribe: `68070131/test/switch_pulldown/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/switch_pulldown/status`

ดูไฟล์ [13_switch_pulldown.ino](13_switch_pulldown.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
