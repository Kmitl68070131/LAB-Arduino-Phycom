# Test PIR HC-SR501

**หน้าที่:** ตรวจเหตุการณ์เคลื่อนไหว ไม่สามารถยืนยันว่ามีคนนั่งนิ่งอยู่

## ต่อสาย
- VCC → 5V, GND → GND, OUT → D2
- ตรวจแรงดัน OUT ตามบอร์ด PIR และรอ Warm-up

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** ช่วงเริ่มต้น WARMUP แล้วเมื่อขยับมือใกล้เซนเซอร์แสดง MOTION

## MQTT
- Publish: `68070131/test/pir/data`
- Subscribe: `68070131/test/pir/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/pir/status`

ดูไฟล์ [10_pir.ino](10_pir.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
