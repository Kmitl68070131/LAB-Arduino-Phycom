# Test TMP36 (Celsius)

**หน้าที่:** TMP36 ใช้ Offset 500 mV และ 10 mV/°C: (แรงดัน - 0.5)×100

## ต่อสาย
- Vs → 5V, Vout → A3, GND → GND
- ตรวจ pinout ตามแพ็กเกจและ Datasheet จริงก่อนจ่ายไฟ

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** อุณหภูมิห้องควรอยู่ในช่วงที่สมเหตุสมผล จับเซนเซอร์ระวังความร้อนแล้วค่าเปลี่ยน

## MQTT
- Publish: `68070131/test/tmp36/data`
- Subscribe: `68070131/test/tmp36/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/tmp36/status`

ดูไฟล์ [02_tmp36.ino](02_tmp36.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
