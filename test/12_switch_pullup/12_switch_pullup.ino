/*
============================================================================
 TEST: Push Button INPUT_PULLUP
 จุดประสงค์: ทดสอบอ่านปุ่ม Pull-up และนับขอบการกดหนึ่งครั้งต่อการกด
 การต่อสายอุปกรณ์ (ทดสอบทีละ Sketch ไม่ใช่ต่อทุกตัวพร้อมกัน):
  - D2 → สวิตช์ NO → GND
  - ไม่ต้องใช้ R ภายนอก; กด = LOW, ปล่อย = HIGH
 คาดหวัง: กดปุ่มแสดง PRESSED แล้วปล่อยแสดง RELEASED
 Serial Monitor: 115200 baud
 ไลบรารี: WiFiS3 (Arduino UNO R4 WiFi Core), MQTT by 256dpi
 หากไม่ต่ออินเทอร์เน็ต ยังต้องเห็นการอ่านค่าที่ Serial Monitor
 ใช้ Broker TCP 1883 สำหรับห้อง LAB เท่านั้น ไม่มี TLS
============================================================================
*/
#include <WiFiS3.h>      // Wi-Fi ในบอร์ด UNO R4 WiFi
#include <MQTTClient.h>  // MQTTClient by 256dpi สำหรับ Publish/Subscribe
#include <math.h>        // สูตรเซนเซอร์และ isnan()


// =============== ส่วน 1: เครือข่าย เปลี่ยนตรงนี้ ============================
const char WIFI_SSID[] = "EDOK";                // <<< แก้ชื่อ Wi-Fi ตรงนี้
const char WIFI_PASS[] = "1234567890";          // <<< แก้รหัส Wi-Fi ตรงนี้
const char MQTT_HOST[] = "phycom.it.kmitl.ac.th"; // <<< แก้ Broker ตรงนี้
const int MQTT_PORT = 1883;                    // <<< แก้ Port ตรงนี้
// =============== ส่วน 2: Topic เปลี่ยนตรงนี้ ================================
// TOPIC_DATA = Arduino Publish ข้อมูล → เว็บ Subscribe รับข้อมูล
// TOPIC_SET = เว็บ Publish คำสั่ง → Arduino Subscribe รับคำสั่ง
// TOPIC_STATUS = Arduino Publish ผลคำสั่ง/ONLINE → เว็บ Subscribe
// เมื่อเปลี่ยน Topic ฝั่งเว็บและ MQTT Client ต้องแก้ให้ตรงทุกตัวอักษร
const char TOPIC_DATA[] = "68070131/test/switch_pullup/data"; // <<< เปลี่ยน Topic ส่งค่าตรงนี้
const char TOPIC_SET[] = "68070131/test/switch_pullup/set"; // <<< เปลี่ยน Topic รับคำสั่งตรงนี้
const char TOPIC_STATUS[] = "68070131/test/switch_pullup/status"; // <<< เปลี่ยน Topic สถานะตรงนี้

// =============== ส่วน 3: ตัวแปรสำหรับ Network และ Sensor ===================
WiFiClient wifiClient;             // TCP Client สำหรับ MQTT
MQTTClient mqtt(512);             // ขนาด Buffer 512 bytes
String clientId;                  // รหัสบอร์ดต้องไม่ชนเครื่องอื่น
unsigned long lastWiFiTry=0;      // เวลาลองเชื่อม Wi-Fi ล่าสุด
unsigned long lastMqttTry=0;      // เวลาลองเชื่อม MQTT ล่าสุด
unsigned long lastRead=0;         // เวลาอ่านเซนเซอร์ล่าสุด
const unsigned long READ_MS=35UL; // <<< ช่วงอ่านค่า ms; DHT ต้อง >=2000
bool pendingAck=false;            // มีข้อความตอบรับที่รอ Publish
String responseText="";           // ACK ตอบกลับเว็บ
bool previous=false;unsigned long countPress=0;

// =============== ส่วน 4: ฟังก์ชันอ่านอุปกรณ์ ================================
void setupDevice(){
  pinMode(2,INPUT_PULLUP); // กำหนด INPUT/OUTPUT และ ADC ตามเซนเซอร์
}
String readDevice(){
  bool down=digitalRead(2)==LOW;if(down&&!previous)countPress++;previous=down;return String(down?"PRESSED":"RELEASED")+",count="+String(countPress); // แปลงค่าที่วัดได้เป็นข้อความสำหรับ Serial + MQTT
}

// =============== ส่วน 5: รับคำสั่ง MQTT ====================================
void onMessage(String &topic,String &message){
  if(topic!=TOPIC_SET)return;     // ป้องกันคำสั่งจาก Topic อื่น
  message.trim();                 // เอาช่องว่างหัว/ท้ายออก
  if(message=="PING")responseText="PONG"; // ทดสอบส่งจากเว็บกลับบอร์ด
  else if(message=="READ")responseText="OK:READ"; // รับคำสั่งทดสอบ
  else responseText="ERROR:COMMAND";  // ไม่รับคำสั่งที่ไม่ได้ระบุไว้
  pendingAck=true;                // ค่อย Publish ใน loop ไม่ส่งใน Callback
  Serial.println(String("RX CMD ")+message);
}

// =============== ส่วน 6: ดูแล Wi-Fi / MQTT ================================
void networkTick(){
  unsigned long now=millis();     // เวลาปัจจุบัน ms
  if(WiFi.status()!=WL_CONNECTED){
    if(now-lastWiFiTry>=10000UL){ // ลองต่อใหม่ไม่เกินทุก 10 วินาที
      lastWiFiTry=now;
      WiFi.begin(WIFI_SSID,WIFI_PASS);
    }
    return;                      // ไม่ให้เครือข่ายบล็อกการอ่าน Sensor
  }
  if(!mqtt.connected()){
    if(now-lastMqttTry>=5000UL){
      lastMqttTry=now;
      if(mqtt.connect(clientId.c_str())){
        mqtt.subscribe(TOPIC_SET); // <<< Subscribe คำสั่งบน Topic นี้
        mqtt.publish(TOPIC_STATUS,"ONLINE"); // <<< แจ้งว่าต่อ Broker ได้
        Serial.println("MQTT CONNECTED");
      }
    }
    return;
  }
  mqtt.loop();                    // สำคัญ: รับคำสั่งและรักษา Keepalive
  if(pendingAck&&mqtt.publish(TOPIC_STATUS,responseText)){
    pendingAck=false;             // ล้าง ACK เฉพาะเมื่อส่งสำเร็จ
  }
}

// =============== ส่วน 7: ตั้งค่าตอนเปิดบอร์ด ================================
void setup(){
  Serial.begin(115200);           // ดูค่า Sensor ด้วย Serial Monitor 115200
  setupDevice();                  // เตรียมขาต่อจริง
  WiFi.begin(WIFI_SSID,WIFI_PASS); // เริ่มต่อ Wi-Fi; Serial ยังแสดงค่าได้แม้เน็ตหลุด
  byte mac[6]={0};                // Buffer MAC 6 ไบต์ของ UNO R4
  WiFi.macAddress(mac);           // สร้าง client ID เฉพาะบอร์ด
  clientId="test-switch_pullup-";
  for(byte i=0;i<6;i++){if(mac[i]<16)clientId+="0";clientId+=String(mac[i],HEX);}
  mqtt.begin(MQTT_HOST,MQTT_PORT,wifiClient);
  mqtt.onMessage(onMessage);      // กำหนดฟังก์ชันรับ /set
  Serial.println(String("TEST: Push Button INPUT_PULLUP"));
  Serial.println(String("PUB ")+TOPIC_DATA);
  Serial.println(String("SUB ")+TOPIC_SET);
}

// =============== ส่วน 8: วนอ่านและส่งขึ้นเว็บ ===============================
void loop(){
  networkTick();                  // ดูแล MQTT โดยไม่ต้องรอ Wi-Fi เพื่อวัด Sensor
  if(millis()-lastRead<READ_MS)return;
  lastRead=millis();              // นับช่วงเวลาอ่านค่า
  String value=readDevice();      // อ่านจากขาที่ต่อจริง
  if(value.length()==0)return;   // เซนเซอร์ไม่มีข้อมูลใหม่ ไม่จำเป็นต้องส่ง
  Serial.println(String("DATA switch_pullup: ")+value);
  if(mqtt.connected())mqtt.publish(TOPIC_DATA,value); // <<< Publish ไปเว็บ
}
