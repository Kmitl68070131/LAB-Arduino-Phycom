// LM35: Vout → A0; Vs → 5V; GND → GND (ตรวจ pinout รุ่นจริง)
// LCD: VCC=5V, GND=GND, SDA=SDA, SCL=SCL
#define SENSOR "LM35"
#define UNIT "C"
#define USE_STATS 0
#define IS_DHT 0
float readSensor();
void sensorSetup();
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include "Statistics.h"
// แก้ค่าตัวอย่างที่เครื่องตนเองก่อนอัปโหลด; อย่า commit รหัสผ่านจริง
const char WIFI_SSID[] = "YOUR_WIFI"; // ชื่อ Wi-Fi
const char WIFI_PASS[] = "YOUR_PASSWORD"; // รหัสผ่าน Wi-Fi
const char BROKER[] = "phycom.it.kmitl.ac.th"; // MQTT broker
const int PORT = 1883; // TCP port; เครือข่ายทดลอง ไม่มี TLS
WiFiClient transport; // TCP socket ของบอร์ด
MQTTClient mqtt(2048); // buffer รับส่ง MQTT หน่วย byte
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address, จำนวนคอลัมน์, แถว
String baseTopic; // prefix เฉพาะโหมด/อุปกรณ์
String clientId; // client ID สร้างจาก MAC ไม่ให้ชนบอร์ดอื่น
String remoteText = "Waiting web"; // ข้อความล่าสุดจากเว็บ (ASCII บน LCD)
unsigned long remoteUntil = 0; // เวลาเริ่มแสดงข้อความเว็บ
bool remoteVisible = false; // แสดงข้อความเว็บชั่วคราวหรือไม่
unsigned long lastWifi = 0, lastMqtt = 0, lastSample = 0; // เวลา retry Wi-Fi/MQTT/อ่านค่า หน่วย ms
unsigned long intervalMs = 2500; // ช่วงอ่านค่า ไม่ต่ำกว่า 2.5 วินาทีเพื่อ DHT
bool ackPending = false; // รอส่ง ack นอก callback
String ackText; // ผลตรวจคำสั่ง
WindowStats primaryStats, humidityStats; // หน้าต่างสถิติค่าเซนเซอร์และความชื้น
void line(byte row, String value) { // row = แถว LCD; value = ข้อความที่จะตัด/เติมให้ครบ 16
  value = value.substring(0, 16);
  while (value.length() < 16) value += ' ';
  lcd.setCursor(0, row); lcd.print(value);
}
void received(String &topic, String &payload) { // topic/payload คือข้อมูล MQTT ที่เว็บส่งมา
  Serial.println("RX " + topic + " " + payload);
  JsonDocument command; // เอกสาร JSON คำสั่งจากเว็บ
  if (payload.length() > 512 || deserializeJson(command, payload) || !command.is<JsonObject>()) {
    ackText = "Invalid JSON"; ackPending = true; return;
  }
  if (!command["text"].isNull() && !command["text"].is<const char*>()) {
    ackText = "text must string"; ackPending = true; return;
  }
  if (!command["interval_ms"].isNull() && (!command["interval_ms"].is<unsigned long>() || command["interval_ms"].as<unsigned long>() < 2500 || command["interval_ms"].as<unsigned long>() > 60000)) {
    ackText = "interval 2500-60000"; ackPending = true; return;
  }
  if (!command["reset"].isNull() && !command["reset"].is<bool>()) {
    ackText = "reset must bool"; ackPending = true; return;
  }
  if (command["reset"] == true) { primaryStats.clear(); humidityStats.clear(); }
  if (command["interval_ms"].is<unsigned long>()) intervalMs = command["interval_ms"];
  remoteText = command["text"] | "Command OK";
  remoteVisible = true; remoteUntil = millis();
  line(0, "WEB RX"); line(1, remoteText);
  ackText = "ok"; ackPending = true; // ห้าม publish/subscribe ภายใน callback
}
void networkTick() {
  unsigned long now = millis(); // เวลา uptime ปัจจุบัน ms
  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWifi >= 10000UL) { lastWifi = now; WiFi.begin(WIFI_SSID, WIFI_PASS); }
    return;
  }
  if (!mqtt.connected() && now - lastMqtt >= 5000UL) {
    lastMqtt = now;
    if (mqtt.connect(clientId.c_str())) {
      if (!mqtt.subscribe(baseTopic + "/command")) { mqtt.disconnect(); Serial.println("Subscribe failed"); }
    }
  }
  if (mqtt.connected()) mqtt.loop();
  if (mqtt.connected() && ackPending) {
    JsonDocument reply; // ACK ตอบผลคำสั่งกลับเว็บ
    reply["result"] = ackText; reply["text"] = remoteText;
    String encoded; // JSON ที่ serialize แล้ว
    serializeJson(reply, encoded);
    if (mqtt.publish(baseTopic + "/ack", encoded)) ackPending = false;
  }
}
void emit(float value, const char* channel, const char* unit, WindowStats &stats) {
  // value=ค่าที่อ่าน, channel=ชื่อช่อง, unit=หน่วย, stats=หน้าต่างของช่องนั้น
  JsonDocument data; // JSON telemetry ส่งขึ้นเว็บ
  data["sensor"] = SENSOR; data["channel"] = channel; data["unit"] = unit;
  data["uptime_ms"] = millis(); data["valid"] = isfinite(value);
  if (isfinite(value)) { data["value"] = value; stats.add(value); }
  else data["value"] = nullptr; // timeout/NaN ไม่ใช่ศูนย์และไม่เข้าคำนวณ
  if (USE_STATS) stats.json(data["stats"].to<JsonObject>());
  String encoded; // ข้อมูล JSON ที่พร้อมส่ง
  serializeJson(data, encoded); Serial.println("TX " + encoded);
  if (!mqtt.connected() || !mqtt.publish(baseTopic + "/telemetry", encoded)) Serial.println("NOT SENT: offline/publish failed");
  if (!remoteVisible) {
    if (USE_STATS && stats.count) {
      JsonDocument display; // ผลสถิติสำหรับหน้า LCD
      stats.json(display.to<JsonObject>());
      const char* fields[] = {"min", "max", "avg", "median", "std"}; // ชื่อหน้าสถิติ
      static byte page = 0; // หน้าที่กำลังแสดง 0-4
      line(0, String(channel) + " " + fields[page]);
      line(1, String(display[fields[page]].as<float>(), 2) + " " + unit);
      page = (page + 1) % 5;
      return;
    }
    line(0, String(channel) + (mqtt.connected() ? " online" : " offline"));
    line(1, isfinite(value) ? String(value, 2) + " " + unit : "NO VALID SAMPLE");
  }
}
void setup() {
  Serial.begin(115200); analogReadResolution(12); // ADC ช่วง 0–4095
  Wire.begin(); lcd.init(); lcd.backlight(); line(0, SENSOR); line(1, "Connecting...");
  sensorSetup();
  baseTopic = String("68070131/") + (USE_STATS ? "stats/" : "mqtt/") + SENSOR;
  byte mac[6]; // MAC address 6 byte ของบอร์ด
  WiFi.macAddress(mac);
  clientId = "lab-";
  for (byte index = 0; index < 6; ++index) { if (mac[index] < 16) clientId += "0"; clientId += String(mac[index], HEX); } // index=ตำแหน่ง byte
  mqtt.begin(BROKER, PORT, transport); mqtt.setTimeout(1000); mqtt.onMessage(received);
  Serial.println("TOPIC " + baseTopic);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
}
void loop() {
  networkTick();
  if (remoteVisible && millis() - remoteUntil >= 5000UL) remoteVisible = false;
  if (millis() - lastSample >= intervalMs) {
    lastSample = millis();
    emit(readSensor(), SENSOR, UNIT, primaryStats);
#if IS_DHT
    emit(dht.readHumidity(), "humidity", "%", humidityStats);
#endif
  }
}

void sensorSetup() {  }
float readSensor() {
  int raw = analogRead(A0); // ADC 12-bit ที่ A0
  float voltage = raw * 5.0f / 4095.0f; // แรงดัน V; ปรับ 5.0 ตาม Vref ที่วัดจริง
  return voltage * 100;
}
