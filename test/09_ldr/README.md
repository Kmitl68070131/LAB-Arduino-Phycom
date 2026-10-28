# Test LDR Photoresistor

**หน้าที่:** อ่านความสว่างสัมพัทธ์ด้วยวงจรแบ่งแรงดัน ไม่ใช่ Lux

## ต่อสาย
- 5V → LDR → A0 → R10kΩ → GND
- ในวงจรนี้แสงมากให้ค่า ADC สูงขึ้น

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** เอามือบัง LDR ค่า ADC ลดลง

## MQTT
- Publish: `68070131/test/ldr/data`
- Subscribe: `68070131/test/ldr/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/ldr/status`

ดูไฟล์ [09_ldr.ino](09_ldr.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
