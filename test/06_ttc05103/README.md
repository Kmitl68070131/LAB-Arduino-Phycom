# Test TTC05103 (Thermistor)

**หน้าที่:** NTC อยู่ล่าง ใช้ Rntc = 330×raw/(4095-raw), Beta equation

## ต่อสาย
- 5V → R 330Ω → A0 → TTC05103 → GND
- ตัวอย่าง R25=10kΩ, Beta 4050 ต้องเทียบ Datasheet ชิ้นจริง

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** ค่าอุณหภูมิสมเหตุสมผล ไม่เป็น NaN/INF; ตรวจ R และ Beta หากคลาดเคลื่อน

## MQTT
- Publish: `68070131/test/ttc05103/data`
- Subscribe: `68070131/test/ttc05103/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/ttc05103/status`

ดูไฟล์ [06_ttc05103.ino](06_ttc05103.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
