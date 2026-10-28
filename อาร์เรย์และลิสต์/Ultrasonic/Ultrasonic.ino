// Ultrasonic: HC-SR04 VCC → 5V; GND → GND; TRIG → D2; ECHO → D3
// LCD: VCC=5V, GND=GND, SDA=SDA, SCL=SCL
#define SENSOR "Ultrasonic"
#define UNIT "cm"
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
#include "SensorHistory.h"
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
SensorHistory primaryHistory, humidityHistory; // ประวัติแยกค่าหลักและความชื้น DHT
int selectedIndex = 0; // ลำดับที่เลือกดูบน LCD จากเก่าสุด 0..19
bool showList = false; // false=array, true=linked list
bool historyRequested = false; // รอส่ง snapshot นอก callback
void historyJson(JsonObject target, const SensorHistory &history);
void historyTick();
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
  if (!command["history_index"].isNull() && (!command["history_index"].is<int>() || command["history_index"].as<int>() < 0 || command["history_index"].as<int>() >= SensorHistory::CAPACITY)) {
    ackText = "index 0-19"; ackPending = true; return;
  }
  if (!command["structure"].isNull() && (!command["structure"].is<const char*>() || (command["structure"] != "array" && command["structure"] != "list"))) {
    ackText = "array or list"; ackPending = true; return;
  }
  if ((!command["clear_history"].isNull() && !command["clear_history"].is<bool>()) || (!command["request_history"].isNull() && !command["request_history"].is<bool>())) {
    ackText = "history must bool"; ackPending = true; return;
  }
  if (command["clear_history"] == true) { primaryHistory.clear(); humidityHistory.clear(); }
  if (command["history_index"].is<int>()) selectedIndex = command["history_index"];
  if (command["structure"].is<const char*>()) showList = command["structure"] == "list";
  if (command["request_history"] == true || command["clear_history"] == true) historyRequested = true;
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
  SensorHistory &history = String(channel) == "humidity" ? humidityHistory : primaryHistory; // เลือกประวัติให้ตรงช่อง
  history.append(value); // ไม่เพิ่มเมื่อ NaN/Inf
  historyJson(data["history"].to<JsonObject>(), history);
  data["selected_index"] = selectedIndex;
  data["structure"] = showList ? "list" : "array";
  data["sensor"] = SENSOR; data["channel"] = channel; data["unit"] = unit;
  data["uptime_ms"] = millis(); data["valid"] = isfinite(value);
  if (isfinite(value)) { data["value"] = value; stats.add(value); }
  else data["value"] = nullptr; // timeout/NaN ไม่ใช่ศูนย์และไม่เข้าคำนวณ
  if (USE_STATS) stats.json(data["stats"].to<JsonObject>());
  String encoded; // ข้อมูล JSON ที่พร้อมส่ง
  serializeJson(data, encoded); Serial.println("TX " + encoded);
  if (!mqtt.connected() || !mqtt.publish(baseTopic + "/telemetry", encoded)) Serial.println("NOT SENT: offline/publish failed");
  if (!remoteVisible) {
    float selected = showList ? history.listAt(selectedIndex) : history.arrayAt(selectedIndex); // ค่าที่เว็บเลือกดู
    line(0, String(showList ? "LIST " : "ARRAY ") + String(selectedIndex) + " N=" + String(history.count));
    line(1, isfinite(selected) ? String(selected, 2) + " " + unit : "INDEX NOT READY");
  }
}
void setup() {
  Serial.begin(115200); analogReadResolution(12); // ADC ช่วง 0–4095
  Wire.begin(); lcd.init(); lcd.backlight(); line(0, SENSOR); line(1, "Connecting...");
  sensorSetup();
  baseTopic = String("68070131/collections/") + SENSOR; // โหมดอาร์เรย์และลิสต์
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
  historyTick();
  if (remoteVisible && millis() - remoteUntil >= 5000UL) remoteVisible = false;
  if (millis() - lastSample >= intervalMs) {
    lastSample = millis();
    emit(readSensor(), SENSOR, UNIT, primaryStats);
#if IS_DHT
    emit(dht.readHumidity(), "humidity", "%", humidityStats);
#endif
  }
}

void sensorSetup() { pinMode(2, OUTPUT); pinMode(3, INPUT); }
float readSensor() {
  int raw = analogRead(A0); // ADC 12-bit ที่ A0
  float voltage = raw * 5.0f / 4095.0f; // แรงดัน V; ปรับ 5.0 ตาม Vref ที่วัดจริง
  digitalWrite(2, LOW); delayMicroseconds(2);
  digitalWrite(2, HIGH); delayMicroseconds(10); digitalWrite(2, LOW);
  unsigned long echoUs = pulseIn(3, HIGH, 30000UL); // เวลา echo หน่วย us; 0 = timeout
  return echoUs ? echoUs * 0.0343f / 2 : NAN;
}

void historyJson(JsonObject target, const SensorHistory &history) {
  // target=JSON ปลายทาง; history=ข้อมูลหนึ่งช่อง แสดงสองโครงสร้างเรียงเก่า→ใหม่
  target["count"] = history.count;
  JsonArray arrayValues = target["array"].to<JsonArray>(); // รายการจาก ring array
  JsonArray listValues = target["list"].to<JsonArray>(); // รายการจาก linked list
  for (int index = 0; index < history.count; ++index) { // index=ลำดับค่า
    arrayValues.add(history.arrayAt(index));
  }
  for (const SensorHistory::Node* node = history.head; node; node = node->next) { // node=โหนดที่กำลังส่ง; เดินลิสต์ครั้งเดียว O(N)
    listValues.add(node->value);
  }
}
void historyTick() {
  if (!historyRequested || !mqtt.connected()) return;
  JsonDocument reply; // snapshot ประวัติ ตอบคำสั่งโดยไม่อ่านเซนเซอร์/เพิ่มข้อมูล
  reply["sensor"] = SENSOR; reply["selected_index"] = selectedIndex;
  reply["structure"] = showList ? "list" : "array";
  historyJson(reply["primary"].to<JsonObject>(), primaryHistory);
#if IS_DHT
  historyJson(reply["humidity"].to<JsonObject>(), humidityHistory);
#endif
  String encoded; // JSON พร้อมส่ง
  serializeJson(reply, encoded); Serial.println("HISTORY " + encoded);
  if (mqtt.publish(baseTopic + "/history", encoded)) historyRequested = false;
}
