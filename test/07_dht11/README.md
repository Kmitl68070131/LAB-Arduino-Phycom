# Test DHT11 Temp + Humidity

**หน้าที่:** อ่าน Temperature และ Humidity คู่กัน; ห้ามอ่านถี่เกิน ~2 วินาที

## ต่อสาย
- VCC → 5V, GND → GND, DATA → D2
- โมดูลเปล่าต้องมี R pull-up DATA→VCC ประมาณ 10kΩ; โมดูลสำเร็จอาจมีอยู่แล้ว

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** อุณหภูมิ/ความชื้นเปลี่ยนตามสภาพจริง โดยส่ง `อุณหภูมิ,ความชื้น` เช่น 29.0,62.0

## MQTT
- Publish: `68070131/test/dht11/data`
- Subscribe: `68070131/test/dht11/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/dht11/status`

ดูไฟล์ [07_dht11.ino](07_dht11.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
