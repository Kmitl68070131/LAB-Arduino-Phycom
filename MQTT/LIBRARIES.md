# 📚 MQTT — ไลบรารีและคู่มือใช้งาน

[🏠 หน้าแรก](../README.md) › [📡 เมนู MQTT](README.md) › Library & Protocol

คู่มือสำหรับตัวอย่าง **UNO R4 WiFi** ใน repo นี้ รวมสิ่งที่ต้องใช้และหัวข้อ MQTT ที่เกี่ยวข้อง พร้อมลิงก์เอกสารเต็มของแต่ละไลบรารี

## 🧭 เลือกหัวข้อ

- [1 · ไลบรารีที่ต้องติดตั้ง](#ไลบรารีที่ต้องติดตั้ง)
- [2 · เส้นทางข้อมูล](#เส้นทางข้อมูล)
- [3 · ตั้งค่าเครือข่ายและบัญชี](#ตั้งค่าเครือข่ายและบัญชี)
- [4 · Topic และ JSON](#topic-และ-json)
- [5 · คำสั่ง MQTTClient](#คำสั่ง-mqttclient)
- [6 · QoS / Retain / Session / Will](#qos--retain--session--will)
- [7 · เปิดเว็บและเครื่องมือทดสอบ](#เปิดเว็บและเครื่องมือทดสอบ)
- [8 · ตรวจปัญหา](#ตรวจปัญหา)
- [9 · แหล่งอ้างอิง](#แหล่งอ้างอิง)

## ไลบรารีที่ต้องติดตั้ง

### Arduino IDE

| ติดตั้งอะไร | ใช้กับส่วนไหน | Header / หน้าที่ |
|---|---|---|
| **Arduino UNO R4 Boards** ผ่าน Boards Manager | ทุกชุด | board core + **WiFiS3**, **Wire** |
| **MQTT by Joel Gaehwiler / 256dpi** ผ่าน Library Manager | ทุกชุด | `MQTTClient.h` ต่อ broker, publish, subscribe |
| **ArduinoJson 7** by Benoit Blanchon | ทุกชุด | `ArduinoJson.h` อ่าน/สร้าง JSON; ตัวอย่างใช้ API v7 |
| **LiquidCrystal I2C** by Frank de Brabander | ทุกชุดที่มี LCD | `LiquidCrystal_I2C.h`, `init()`, `backlight()` |
| **Keypad** by Mark Stanley / Alexander Brevig | Keypad | `Keypad.h` scan ปุ่มเมทริกซ์ |
| **DHT sensor library** by Adafruit | DHT11 / DHT22 | `DHT.h` อ่านอุณหภูมิ/ความชื้น |
| **Adafruit Unified Sensor** | dependency ของ DHT | ติดตั้งเมื่อ IDE เสนอ Install all |
| `math.h`, `initializer_list` | สูตร / Statistics.h | มากับ C/C++ toolchain ไม่ต้องโหลด zip |
| `Statistics.h`, `ButtonDebounce.h` | ไฟล์ท้องถิ่นใน sketch | มากับ repo ไม่ได้ติดตั้งใน Library Manager |

เปิด Sketch → Include Library → Manage Libraries ค้น **ชื่อและผู้สร้าง** ให้ตรง อย่าเลือก PubSubClient แล้วคาดว่าจะใช้ `MQTTClient` API เดียวกันได้ ทุกชุดมีไฟล์ .h ที่ต้องอยู่ข้าง .ino; ชื่อโฟลเดอร์ต้องตรงชื่อ sketch
ไม่มีการคัดลอก source ไลบรารีภายนอกเข้ามาใน repo ให้ติดตั้งจาก Library Manager เพื่อคง dependencies และ license ของผู้สร้าง

### เว็บ / Node.js

| รายการ | วิธีติดตั้ง | หน้าที่ |
|---|---|---|
| Node.js 20 ขึ้นไป + npm | ติดตั้งจาก nodejs.org | รัน bridge |
| **mqtt (MQTT.js)** | `npm ci` ใน MQTT/web | ต่อ broker TCP จาก Node |
| http, fs | built-in Node | HTTP server และอ่าน HTML |
| fetch, EventSource | built-in browser | ส่งคำสั่ง HTTP / รับ SSE |

`package.json` ระบุ dependency และ `package-lock.json` ล็อก dependency tree แล้ว ไม่ต้องติดตั้ง mqtt แบบ global

## เส้นทางข้อมูล

1. บอร์ดอ่านเซนเซอร์หรือปุ่ม → สร้าง JSON → MQTT broker
2. Node bridge subscribe telemetry/ack → ส่ง SSE ให้ browser
3. Browser POST คำสั่ง → Node publish command → บอร์ด subscribe → Serial/LCD
4. บอร์ดตอบ ack → broker → Node → browser

Broker เป็นตัวกลางส่งข้อความตาม topic; **publish**=ส่ง, **subscribe**=ลงทะเบียนรับ, **payload**=เนื้อหา, **client ID**=ชื่อ connection ต้องไม่ชนกัน
HTTP bridge ในชุดนี้ใช้ localhost และ SSE ไม่ต้องสมมติว่า broker ห้องเรียนเปิด WebSocket อยู่ Browser ใช้ TCP port 1883 โดยตรงไม่ได้

## ตั้งค่าเครือข่ายและบัญชี

| ตัวแปร | อยู่ที่ไหน | ต้องใส่อะไร |
|---|---|---|
| WIFI_SSID / WIFI_PASS | แต่ละ .ino | Wi-Fi ที่บอร์ดเข้าถึงได้ |
| BROKER / PORT | แต่ละ .ino | host ไม่มี mqtt://, TCP port ค่าเริ่ม 1883 |
| clientId | sketch สร้างจาก MAC | แต่ละบอร์ดต้องไม่ซ้ำ; repo นี้ใช้ lab-ตาม MAC |
| MQTT_URL | environment ฝั่ง Node | เช่น mqtt://phycom.it.kmitl.ac.th:1883 |
| PORT | environment ฝั่ง Node | HTTP port ค่าเริ่ม 3000 (คนละ PORT กับ broker) |
| baseTopic | sketch | 68070131/mqtt/ชื่ออุปกรณ์ หรือ 68070131/stats/ชื่ออุปกรณ์ |

ตัวอย่างบน Linux/macOS เปลี่ยน broker ของ bridge:
```sh
MQTT_URL=mqtt://your-broker:1883 npm start
```
PowerShell:
```powershell
$env:MQTT_URL="mqtt://your-broker:1883"
npm start
```

หาก broker ต้อง username/password ต้องตั้ง **ทั้งบอร์ดและ bridge**:
```cpp
// ตัวอย่างปรับ networkTick; ใส่ข้อมูลจาก broker ของตนเอง
mqtt.connect(clientId.c_str(), "YOUR_USERNAME", "YOUR_PASSWORD");
```
MQTT.js รองรับ options `{username, password}` ใน `mqtt.connect(url, options)`; อ่านจาก environment แทนใส่รหัสจริงในไฟล์ ตัวอย่างหลักของ repo ยังใช้ broker แบบไม่ระบุบัญชี

TCP 1883 ของ lab ไม่เข้ารหัส หากต้องการ TLS ใช้ transport SSL ที่ UNO R4 WiFi รองรับและตั้ง certificate/เวลา/hostname ตาม broker จริง ฝั่ง Node ใช้ mqtts:// และ CA ที่เชื่อถือได้ ห้ามปิดตรวจ certificate เพื่อแก้ปัญหา MQTT WebSocket ใช้ ws:// หรือ wss:// เฉพาะ endpoint/port/path ที่ผู้ดูแล broker ระบุ ไม่เดาจาก TCP port

## Topic และ JSON

| Topic | ผู้ส่ง → ผู้รับ | ข้อมูล |
|---|---|---|
| 68070131/mqtt/ButtonEvents/telemetry | บอร์ด → เว็บ | pressed/released/snapshot + จำนวนกด |
| 68070131/mqtt/ButtonEvents/command | เว็บ → บอร์ด | text, interval_ms, reset |
| 68070131/mqtt/ButtonEvents/ack | บอร์ด → เว็บ | result และ text |
| 68070131/stats/TMP36/telemetry | บอร์ด → เว็บ | value และ stats |

หัวข้อ case-sensitive: `ButtonEvents` ต่างจาก `buttonevents`; broker ไม่ได้สร้างเว็บให้เอง ต้องมี subscriber
`+` แทนหนึ่งระดับ เช่น `68070131/mqtt/+/telemetry`; `#` แทนหลายระดับและต้องอยู่ท้าย เช่น `68070131/mqtt/#` ใช้ wildcard ตอน subscribe ไม่ใช้ตอน publish

```json
{"text":"Hello LCD","interval_ms":2500,"reset":false}
```

- JSON ต้องใช้ double quotes; ห้าม trailing comma; true/false ไม่ใส่เครื่องหมายคำพูด
- `text` string, `interval_ms` unsigned integer 2500–60000, `reset` boolean
- คำสั่งเกิน 512 bytes ถูกปฏิเสธ; firmware MQTT buffer 2048 bytes รวมข้อจำกัด packet/topic ด้วย
- อ่าน `deserializeJson()` แล้วตรวจชนิดก่อนใช้; `serializeJson()` สร้างข้อความ ไม่ต่อ JSON ด้วย string เอง
- invalid sensor reading ส่ง null/valid=false; ไม่ส่ง NaN ซึ่งไม่ใช่ JSON number
- ACK ของชุดนี้ไม่มี request ID จึงส่งทีละคำสั่ง; ผล HTTP 202 หมายถึง publish แล้ว ต้องรอ ACK จึงทราบว่าบอร์ดประมวลผล

## คำสั่ง MQTTClient

| API | ทำอะไร / ใช้เมื่อใด |
|---|---|
| `WiFi.begin(ssid, pass)` | เริ่มเชื่อม Wi-Fi ก่อน MQTT |
| `WiFi.status() == WL_CONNECTED` | ตรวจเครือข่าย |
| `mqtt.begin(host, port, transport)` | ตั้ง broker และ TCP transport |
| `mqtt.onMessage(received)` | ลงทะเบียน callback รับ topic/payload |
| `mqtt.connect(clientId)` | เปิด MQTT session |
| `mqtt.connected()` | ตรวจ connection ปัจจุบัน |
| `mqtt.subscribe(topic)` | เริ่มรับข้อความ; ชุดนี้ subscribe ใหม่หลัง reconnect |
| `mqtt.publish(topic, payload)` | ส่งข้อความ ค่าเริ่ม non-retained QoS 0 |
| `mqtt.publish(topic, payload, retained, qos)` | ระบุ retain/QoS เอง |
| `mqtt.loop()` | เรียกต่อเนื่องเพื่อรับข้อความ/keepalive |
| `mqtt.unsubscribe(topic)` | ยกเลิกการรับหัวข้อ |
| `mqtt.disconnect()` | ปิด connection อย่างเป็นระเบียบ |
| `mqtt.setTimeout(1000)` | timeout ของ MQTT operation หน่วย ms ไม่ใช่การรับประกันว่า Wi-Fi connect ไม่บล็อก |
| `mqtt.setKeepAlive(30)` | กำหนดช่วง MQTT keepalive หน่วยวินาที |
| `mqtt.setCleanSession(true)` | เริ่ม session ใหม่เมื่อ connect |
| `mqtt.setWill(topic, payload, retained, qos)` | ตั้งข้อความให้ broker ส่งเมื่อ connection ขาดผิดปกติ; ตั้งก่อน connect |
| `mqtt.lastError()` / `mqtt.returnCode()` | error ของ client / connect return code เพื่อวิเคราะห์ |

ใน callback ให้ตรวจ JSON/อัปเดต state/ตั้ง flag แล้ว publish ACK ใน loop: upstream ของไลบรารีเตือนเรื่องเรียก publish/subscribe ซ้อนใน callback เพราะอาจเกิดการประมวลผล packet ซ้อน หลีกเลี่ยง delay ยาวใน loop

## QoS / Retain / Session / Will

| เรื่อง | ความหมาย | ชุดนี้ใช้อย่างไร |
|---|---|---|
| QoS 0 | ส่งได้มากสุดหนึ่งครั้ง อาจหาย | telemetry/commands ของตัวอย่าง; เหมาะทดลอง |
| QoS 1 | อย่างน้อยหนึ่งครั้ง อาจซ้ำ | ถ้าใช้กับคำสั่งนับต้องเพิ่ม event ID/deduplication |
| QoS 2 | protocol ลดการส่งซ้ำตาม session | ไม่ทดแทน transaction/ยืนยันผลทางกายภาพ และไม่ทำให้แอปทุกส่วน exactly once |
| Retain | broker เก็บค่าล่าสุดของ topic ให้ subscriber ใหม่ | ตัวอย่างไม่ retain command ป้องกันคำสั่งเก่าถูกเรียกซ้ำตอน subscribe |
| Clean session | ควบคุมการคง session/subscriptions ตาม broker | ชุดนี้ subscribe ใหม่ทุก connection |
| Last Will | broker ประกาศ offline เมื่อ connection ขาดผิดปกติ | ชุดหลักยังไม่ได้เปิด; เพิ่มได้ตามตัวอย่างด้านล่าง |
| Reconnect | ต่อ Wi-Fi/MQTT ใหม่หลังหลุด | retry ตาม millis; ไม่เก็บ queue เหตุการณ์ offline |

ตัวอย่างเพิ่มสถานะออนไลน์ (เป็น snippet เสริม ต้องเพิ่ม subscriber ฝั่งเว็บเอง):
```cpp
// setup หลัง baseTopic พร้อม แต่ก่อน connect
String statusTopic = baseTopic + "/status";
mqtt.setWill(statusTopic.c_str(), "offline", true, 0);
// หลัง connect สำเร็จใน networkTick
mqtt.publish(baseTopic + "/status", "online", true, 0);
```
Will ไม่ใช่แจ้งทันทีทุกกรณี broker ต้องตรวจพบ connection ขาดก่อน; graceful disconnect ไม่เรียก Will หากปิดปกติให้ publish offline เองก่อน disconnect; retained status อาจเป็นค่าเก่าเมื่อระบบผิดปกติ ควรใช้ timestamp/heartbeat ร่วมด้วย

## เปิดเว็บและเครื่องมือทดสอบ

```sh
cd MQTT/web
npm ci
npm start
```

เปิด http://127.0.0.1:3000 แล้วเลือก mode/sensor ให้ตรงกับ sketch ตัวอย่าง [ปุ่ม 2 ขา](ButtonEvents/README.md) ทำตามได้ตั้งแต่ต่อสาย
Node bind localhost; เครื่องอื่นเปิด IP เครื่องนี้ไม่ได้ในค่าเริ่มต้น การเปิดภายนอกต้องจัด backend, auth, HTTPS และ broker ACL เพิ่ม ไม่ใช้ GitHub Pages แทน backend

หากติดตั้ง Mosquitto client ไว้แล้ว สามารถตรวจโดยไม่ผ่านเว็บ (เปลี่ยน broker ตามจริง):
```sh
mosquitto_sub -h phycom.it.kmitl.ac.th -p 1883 -t '68070131/mqtt/ButtonEvents/#' -v
mosquitto_pub -h phycom.it.kmitl.ac.th -p 1883 -t '68070131/mqtt/ButtonEvents/command' -m '{"text":"Hello LCD"}'
```

คำสั่งข้างบนใช้ quoting แบบ bash; PowerShell ใช้เว็บหรือ MQTT Explorer หากติดเรื่อง quoting; MQTT Explorer ต้องใส่ host/port/auth ของ broker เดียวกัน ไม่มีการส่งคำสั่งเหล่านี้ให้โดยอัตโนมัติ

## ตรวจปัญหา

| อาการ | ตรวจอะไร |
|---|---|
| MQTTClient.h not found | ติดตั้ง MQTT by Joel Gaehwiler ไม่ใช่ไลบรารีชื่อคล้าย |
| JsonDocument/API ไม่ตรง | ติดตั้ง ArduinoJson major 7 |
| LCD init ไม่พบ | ตรวจผู้สร้าง/เวอร์ชัน LiquidCrystal I2C และ API |
| WiFiS3.h not found | เลือก UNO R4 WiFi และลง board package |
| MQTT ไม่ connect | ตรวจ Wi-Fi, host/port, VPN/เครือข่ายห้องเรียน, client ID และ username/password |
| ต่อได้แล้วหลุดบ่อย | client ID ชน, mqtt.loop ไม่ทัน, delay ยาว, signal อ่อน |
| Node ได้ Broker offline (503) | ดู terminal ของ bridge, ตรวจ MQTT_URL และเครือข่าย |
| TX มีแต่เว็บเงียบ | ดู NOT SENT; mode/sensor/topic ต้องตรง; bridge ต้องรันจริง |
| ได้ HTTP 202 ไม่มี ACK | บอร์ด offline/subscribe ไม่สำเร็จ/topic ผิด; 202 ไม่ยืนยันบอร์ดรับ |
| Publish ซ้ำตอน subscribe | ตรวจ retained message ที่ broker; อย่า retain คำสั่ง |
| payload ถูกตัด | ตรวจขนาด UTF-8 bytes, MQTT buffer, topic length และ JSON |
| กดปุ่มเร็วไม่ขึ้น | เลือก ButtonEvents; กดนิ่งเกิน 30 ms; reconnect อาจบล็อก |
| LCD ไทยเพี้ยน | ROM ทั่วไปไม่รองรับภาษาไทย ใช้ ASCII |

## แหล่งอ้างอิง

- [MQTTClient / 256dpi: API และข้อควรระวัง callback](https://github.com/256dpi/arduino-mqtt)
- [ArduinoJson 7 API](https://arduinojson.org/v7/api/) และ [deserializeJson](https://arduinojson.org/v7/api/json/deserializejson/)
- [UNO R4 WiFi / WiFiS3](https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples/)
- [Arduino debounce example](https://docs.arduino.cc/built-in-examples/digital/Debounce/)
- [MQTT.js](https://github.com/mqttjs/MQTT.js)
- [MQTT specification 3.1.1](https://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html)

[⬆ เมนูหัวข้อ](#-เลือกหัวข้อ) · [📡 กลับ MQTT](README.md)
