# Test DHT22 Temp + Humidity

**หน้าที่:** DHT22 ตัวนี้ตั้งค่า DHT22 ไม่ใช่ DHT11; อ่านทุก ≥2 วินาที

## ต่อสาย
- VCC → 5V, GND → GND, DATA → D2
- DHT22 แบบตัวเปล่ามักต้อง R pull-up 4.7–10kΩ จาก DATA ถึง VCC

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** ค่า T และ RH% เปลี่ยนเมื่ออุณหภูมิหรือความชื้นเปลี่ยน

## MQTT
- Publish: `68070131/test/dht22/data`
- Subscribe: `68070131/test/dht22/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/dht22/status`

ดูไฟล์ [08_dht22.ino](08_dht22.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
