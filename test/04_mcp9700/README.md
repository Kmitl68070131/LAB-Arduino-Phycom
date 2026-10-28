# Test MCP9700 (Celsius)

**หน้าที่:** MCP9700 รุ่น 10mV/°C และ 500mV ที่ 0°C ใช้ (V-0.5)×100

## ต่อสาย
- VDD → 5V, VOUT → A0, GND → GND ตรวจ Datasheet รุ่นจริง

**Serial Monitor:** 115200 baud

**ผลที่คาดหวัง:** ค่าใกล้อุณหภูมิห้องเมื่อแรงดันอ้างอิงตรง

## MQTT
- Publish: `68070131/test/mcp9700/data`
- Subscribe: `68070131/test/mcp9700/set` (ทดสอบส่ง `PING`, `READ`)
- Status: `68070131/test/mcp9700/status`

ดูไฟล์ [04_mcp9700.ino](04_mcp9700.ino) คอมเมนต์ด้านบนสำหรับพิน และเปลี่ยนค่าคงที่ `TOPIC_DATA`, `TOPIC_SET`, `TOPIC_STATUS` ในส่วนที่ 2 เท่านั้น

> โค้ดทดสอบต้องตรวจฮาร์ดแวร์จริง ส่วน MQTT ถ้า Broker ไม่ออนไลน์จะเห็นผลเฉพาะ Serial Monitor
