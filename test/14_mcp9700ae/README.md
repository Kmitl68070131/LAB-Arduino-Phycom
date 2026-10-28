# MCP9700AE — LCD + Wi-Fi + MQTT Both

## การต่อสาย
- MCP9700AE **VDD → 5V, VOUT → A0, GND → GND**
- **ห้ามเดาตำแหน่งขา** เพราะมีหลายแพ็กเกจ ให้ตรวจ Datasheet และลำดับขาของอุปกรณ์ที่ถือจริง
- LCD I2C: VCC → 5V, GND → GND, SDA → SDA, SCL → SCL
- LCD Address ค่าเริ่มต้น `0x27` ถ้าไม่ขึ้นให้สแกนที่อยู่จริง
- หากเซนเซอร์ร้อนผิดปกติให้ตัดไฟทันที

## สูตรที่ใช้
MCP9700A ให้ 500mV ที่ 0°C และมีความชัน 10mV/°C ตาม Datasheet

```cpp
analogReadResolution(12);
float volts = analogRead(A0) * 5.0f / 4095.0f;
float tempC = (volts - 0.5f) / 0.010f;
```

ค่า `5.0f` เป็นตัวอย่างค่าแรงดันอ้างอิง ADC ต้องปรับตามระบบจริง

## MQTT
- SSID: `EDOK`, Password ตามโจทย์อยู่ใน Sketch
- Host: `phycom.it.kmitl.ac.th`; TCP `1883`
- PUBLISH: `68070131/test/mcp9700ae/data` ข้อมูล JSON อุณหภูมิ
- SUBSCRIBE: `68070131/test/mcp9700ae/set` คำสั่งเว็บ
- PUBLISH: `68070131/test/mcp9700ae/status` ตอบ ACK และ ONLINE
- คำสั่งที่ลองส่งได้: `PING`, `READ`, `LIMIT:30`, `LCD:HELLO`

### ตัวอย่าง JSON ที่คาดหวัง
```json
{"temperature_c":25.50,"adc":618,"voltage_v":0.7550,"limit_c":35.0}
```

ตัวเลขเป็นตัวอย่างเชิงคำนวณ ไม่ใช่ค่าจากการวัดจริง

## วิธีทดสอบ
1. เปิด [MCP9700AE.ino](MCP9700AE/MCP9700AE.ino) ด้วย Arduino IDE เลือก UNO R4 WiFi
2. ติดตั้ง Library MQTT by 256dpi, LiquidCrystal I2C และ Board Package UNO R4
3. เปิด Serial Monitor 115200: ต้องเห็น ADC, Voltage, Temperature
4. LCD แสดง TEMP และ NORMAL/ALERT
5. Subscribe MQTT Topic `68070131/test/mcp9700ae/#` จาก MQTT Client แล้วลอง Publish คำสั่งไป `.../set`
6. เมื่อส่ง `PING` ต้องได้รับ `PONG` ใน `.../status`

**สถานะ:** เป็น Sketch ตัวอย่าง ยังต้องตรวจ Compile และฮาร์ดแวร์จริงหลังติดตั้งไลบรารีที่ตรงรุ่น
