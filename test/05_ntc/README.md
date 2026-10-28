# Test NTC 10k B3950

**หน้าที่:** วงจร NTC อยู่ด้านบน: Rntc=10000×(4095/raw−1), สูตร Beta อุณหภูมิ Kelvin

## ต่อสาย
- 5V → NTC 10k → A0 → R 10k → GND
- R25=10kΩ, Beta 3950 เป็นค่าเริ่มต้น ต้องตรวจรุ่นจริง

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** อุณหภูมิค่อย ๆ ลดหรือเพิ่มเมื่อเปลี่ยนอุณหภูมิที่ NTC

## MQTT
- Publish: `68070131/test/ntc/data`
- Subscribe: `68070131/test/ntc/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/ntc/status`

ดูไฟล์ [05_ntc.ino](05_ntc.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
