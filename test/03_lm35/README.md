# Test LM35 (Celsius)

**หน้าที่:** LM35 ให้ 10mV/°C ไม่มี Offset 0.5V จึงใช้แรงดัน × 100

## ต่อสาย
- Vs → 5V, Vout → A0, GND → GND ตรวจ pinout ตามรุ่น

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** ค่าใกล้อุณหภูมิห้อง โดยไม่ติดลบหลายสิบองศาจากสูตร TMP36

## MQTT
- Publish: `68070131/test/lm35/data`
- Subscribe: `68070131/test/lm35/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/lm35/status`

ดูไฟล์ [03_lm35.ino](03_lm35.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
