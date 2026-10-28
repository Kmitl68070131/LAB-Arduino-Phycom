# Mock 01 — TMP36 ส่งอุณหภูมิขึ้นเว็บ

**ประเภท:** โจทย์จาก Mock 6 ข้อ · **อุปกรณ์:** TMP36 + LCD

## วงจร / วิธีต่อสาย

- TMP36 Vs → 5V, Vout → A3, GND → GND (ตรวจ datasheet ก่อนต่อ)
- LCD I2C 16×2: VCC→5V, GND→GND, SDA→SDA, SCL→SCL
- LCD Address ตัวอย่าง 0x27; ถ้าจอไม่ขึ้น ให้ใช้ I2C Scanner ตรวจที่อยู่จริง

![ภาพผังสาย](../../diagrams/01.svg)

## ใช้ทำอะไรและทำงานอย่างไร

Mock 01 — TMP36 ส่งอุณหภูมิขึ้นเว็บ เชื่อมต่อ Wi-Fi, MQTT และ LCD; เมื่ออ่านข้อมูลได้ Arduino ส่งข้อมูลผ่าน Publish ส่วนคำสั่งจากเว็บเข้าทาง Subscribe ตาม Topic ด้านล่าง แสดงผลบน LCD และตอบกลับ ACK อัตโนมัติ

## จุดแก้ MQTT Topic

- `TOPIC_DATA="68070131/temp"` : **บอร์ด Publish → เว็บรับ**
- `TOPIC_COMMAND="68070131/guess/01/set"` : **เว็บ Publish → บอร์ด Subscribe**
- `TOPIC_ACK="68070131/guess/01/ack"` : **บอร์ดตอบผลคำสั่ง**
- หากเปลี่ยนชื่อ Topic ให้แก้ในไฟล์ `Mock_TMP36.ino` ส่วนที่ 2 และแก้ `catalog.json` / หน้าเว็บให้ตรงกัน

## การทดสอบที่คาดหวัง

26.5°C → Publish ข้อความ 26.5

คำสั่งตัวอย่าง: `LIMIT:30` ส่งไป `68070131/guess/01/set`. เปิด Serial Monitor 115200, เลือก Board Arduino UNO R4 WiFi และตรวจ LCD จะแสดงข้อมูล

**หมายเหตุ:** ข้อมูลนี้เป็นผลคาดหวัง ยังต้องตรวจ Compile และอุปกรณ์จริง; ตัวอย่างอื่นใน Folder เป็น Sketch คนละชุด ห้ามรวมหลาย setup() / loop() เข้าด้วยกัน
