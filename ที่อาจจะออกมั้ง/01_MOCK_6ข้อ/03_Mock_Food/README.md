# Mock 03 — Ultrasonic ตรวจอาหาร On/Off

**ประเภท:** โจทย์จาก Mock 6 ข้อ · **อุปกรณ์:** Ultrasonic + LCD + LED

## วงจร / วิธีต่อสาย

- HC-SR04 VCC→5V, GND→GND, TRIG→D2, ECHO→D3
- LCD I2C 16×2: VCC→5V, GND→GND, SDA→SDA, SCL→SCL
- LCD Address ตัวอย่าง 0x27; ถ้าจอไม่ขึ้น ให้ใช้ I2C Scanner ตรวจที่อยู่จริง

![ภาพผังสาย](../../diagrams/03.svg)

## ใช้ทำอะไรและทำงานอย่างไร

Mock 03 — Ultrasonic ตรวจอาหาร On/Off เชื่อมต่อ Wi-Fi, MQTT และ LCD; เมื่ออ่านข้อมูลได้ Arduino ส่งข้อมูลผ่าน Publish ส่วนคำสั่งจากเว็บเข้าทาง Subscribe ตาม Topic ด้านล่าง แสดงผลบน LCD และตอบกลับ ACK อัตโนมัติ

## จุดแก้ MQTT Topic

- `TOPIC_DATA="68070131/food"` : **บอร์ด Publish → เว็บรับ**
- `TOPIC_COMMAND="68070131/guess/03/set"` : **เว็บ Publish → บอร์ด Subscribe**
- `TOPIC_ACK="68070131/guess/03/ack"` : **บอร์ดตอบผลคำสั่ง**
- หากเปลี่ยนชื่อ Topic ให้แก้ในไฟล์ `Mock_Food.ino` ส่วนที่ 2 และแก้ `catalog.json` / หน้าเว็บให้ตรงกัน

## การทดสอบที่คาดหวัง

ระยะ 15 cm → On, 30 cm → Off

คำสั่งตัวอย่าง: `LIMIT:20` ส่งไป `68070131/guess/03/set`. เปิด Serial Monitor 115200, เลือก Board Arduino UNO R4 WiFi และตรวจ LCD จะแสดงข้อมูล

**หมายเหตุ:** ข้อมูลนี้เป็นผลคาดหวัง ยังต้องตรวจ Compile และอุปกรณ์จริง; ตัวอย่างอื่นใน Folder เป็น Sketch คนละชุด ห้ามรวมหลาย setup() / loop() เข้าด้วยกัน
