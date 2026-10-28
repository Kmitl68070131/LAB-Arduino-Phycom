/*
  MCP9700AE (MCP9700A) + LCD I2C + Wi-Fi MQTT Both
  Arduino UNO R4 WiFi
  ===========================================================================
  1) การต่อ MCP9700AE:
     VDD -> 5V, VOUT -> A0, GND -> GND
     สำคัญ: MCP9700AE มีหลายแพ็กเกจ ห้ามเดาตำแหน่งขา
     ต้องเปิด Datasheet ของแพ็กเกจจริงและตรวจเครื่องหมายบนตัวเซนเซอร์
     ถ้าร้อนผิดปกติให้ตัดไฟทันที
  2) LCD I2C 16x2:
     VCC -> 5V, GND -> GND, SDA -> SDA, SCL -> SCL
     ถ้าไม่แสดงข้อความ สแกน Address 0x27 / 0x3F และปรับ contrast
  3) สูตร MCP9700A:
     Vout 0°C=0.500 V, ความชัน=0.010 V/°C
     Voltage = ADC × ADC_REFERENCE_V / 4095
     Temperature = (Voltage - 0.500) / 0.010
     ADC_REFERENCE_V ตั้งตามแรงดันอ้างอิงจริงที่วัดบนบอร์ด
  4) สิ่งที่ต้องเห็น:
     Serial Monitor 115200 แสดง ADC, Voltage, Temp
     LCD แสดงอุณหภูมิและสถานะ Wi-Fi/MQTT
     MQTT Publish JSON ไปที่ /data ทุก 2 วินาที
     รับคำสั่ง PING, READ, LIMIT:35, LCD:HELLO ผ่าน /set
  5) Topic: ดูส่วน TOPIC ด้านล่าง เปลี่ยนได้ตรงค่าคงที่ 3 บรรทัด
*/
#include <WiFiS3.h>                 // Wi-Fi ของ Arduino UNO R4 WiFi
#include <MQTTClient.h>             // MQTT by 256dpi
#include <Wire.h>                   // บัส I2C
#include <LiquidCrystal_I2C.h>      // จอ 16×2 I2C
#include <math.h>                   // isnan(), isfinite()

// ====================== NETWORK แก้ตรงนี้ ===============================
const char WIFI_SSID[]="EDOK";                 // <<< เปลี่ยนชื่อ Wi-Fi
const char WIFI_PASS[]="1234567890";           // <<< เปลี่ยนรหัส Wi-Fi
const char MQTT_HOST[]="phycom.it.kmitl.ac.th"; // <<< เปลี่ยน MQTT Broker
const int MQTT_PORT=1883;                     // <<< MQTT TCP Port

// ====================== TOPIC แก้ตรงนี้ ================================
const char TOPIC_DATA[]="68070131/test/mcp9700ae/data";      // <<< บอร์ด PUBLISH อุณหภูมิ ไปหน้าเว็บ
const char TOPIC_SET[]="68070131/test/mcp9700ae/set";        // <<< บอร์ด SUBSCRIBE คำสั่งจากเว็บ
const char TOPIC_STATUS[]="68070131/test/mcp9700ae/status";  // <<< บอร์ด PUBLISH ACK/ONLINE
// ถ้าเปลี่ยน /data /set /status ต้องแก้เว็บและ MQTT Client ให้ตรงกันทุกตัวอักษร

// ====================== SENSOR และตัวแปร ================================
const byte SENSOR_PIN=A0;         // <<< พิน VOUT ของ MCP9700AE (Analog)
const float ADC_REFERENCE_V=5.0f; // <<< แรงดัน ADC Reference ของระบบจริง (โวลต์)
const float ZERO_C_V=0.500f;      // MCP9700A 0°C ให้ 500mV
const float VOLTS_PER_C=0.010f;   // MCP9700A เพิ่ม 10mV ต่อ 1°C
const unsigned long SAMPLE_MS=2000UL; // อ่านและส่งทุก 2 วินาที
WiFiClient network;               // Socket TCP ของ MQTT
MQTTClient mqtt(512);             // บัฟเฟอร์ MQTT
LiquidCrystal_I2C lcd(0x27,16,2); // <<< หาก I2C Scanner พบ 0x3F แก้ 0x27 ตรงนี้
String clientId;                  // รหัส MQTT แยกตาม MAC Address
unsigned long lastSample=0;       // เวลาอ่านค่าล่าสุด
unsigned long lastWiFi=0;         // เวลาเชื่อม Wi-Fi ล่าสุด
unsigned long lastMQTT=0;         // เวลาเชื่อม Broker ล่าสุด
unsigned long messageAt=0;        // เวลาข้อความ LCD จากเว็บ
float limitC=35.0f;               // <<< อุณหภูมิเตือนเริ่มต้น เปลี่ยนได้จากเว็บ
float latestC=NAN;                // ค่าที่อ่านล่าสุด (ยังไม่มีค่า = NAN)
String lcdMessage="";             // ข้อความสั้นจากเว็บ
String ack="BOOT";                // ผลตอบกลับคำสั่ง
bool ackPending=false;            // คำสั่งรอ Publish ACK
bool forceRead=false;             // เว็บสั่ง READ ให้ส่งค่าทันที

void showLine(byte row,String message){ // เขียน LCD พร้อมล้างตัวอักษรเดิม
  message=message.substring(0,16); // จำกัด 16 ตัวต่อแถว
  while(message.length()<16)message+=' '; // เติมช่องว่าง
  lcd.setCursor(0,row);lcd.print(message); // พิมพ์ที่แถว 0 หรือ 1
}
void onMessage(String &topic,String &payload){ // รับคำสั่ง MQTT จากเว็บ
  if(topic!=TOPIC_SET)return;      // ไม่รับข้อความจาก Topic อื่น
  payload.trim();                  // ลบช่องว่างหน้า/หลัง
  if(payload.length()>32)ack="ERROR:SIZE";
  else if(payload=="PING")ack="PONG";
  else if(payload=="READ"){forceRead=true;ack="OK:READ";}
  else if(payload.startsWith("LIMIT:")){
    String val=payload.substring(6); // อ่านค่าหลัง LIMIT:
    float c=val.toFloat();
    if(val.length() && c>=-40.0f && c<=125.0f){limitC=c;ack="OK:LIMIT";}
    else ack="ERROR:LIMIT";
  }else if(payload.startsWith("LCD:")&&payload.length()>4&&payload.length()<=20){
    lcdMessage=payload.substring(4); // ข้อความ 1–16 ตัว
    messageAt=millis();ack="OK:LCD";
  }else ack="ERROR:COMMAND";
  ackPending=true;                 // ส่ง ACK ใน loop (ไม่ Publish ใน Callback)
  Serial.println(String("MQTT RX ")+payload+" => "+ack);
}
void connectTick(){                // เครือข่ายทำงานแยกจากการอ่านเซนเซอร์
  unsigned long now=millis();
  if(WiFi.status()!=WL_CONNECTED){
    if(now-lastWiFi>=10000UL){lastWiFi=now;WiFi.begin(WIFI_SSID,WIFI_PASS);}
    return;                        // แม้เน็ตหลุดจอยังแสดงได้
  }
  if(!mqtt.connected()){
    if(now-lastMQTT>=5000UL){
      lastMQTT=now;
      if(mqtt.connect(clientId.c_str())){
        if(mqtt.subscribe(TOPIC_SET))mqtt.publish(TOPIC_STATUS,"ONLINE"); // <<< SUBSCRIBE + STATUS
        else mqtt.disconnect();
      }
    }
    return;
  }
  mqtt.loop();                     // สำคัญ: ใช้รับคำสั่งและ Keepalive
  if(ackPending&&mqtt.publish(TOPIC_STATUS,ack))ackPending=false; // <<< PUBLISH ACK
}
void readAndReport(){             // อ่านค่า ADC และคำนวณ MCP9700AE
  const int raw=analogRead(SENSOR_PIN); // อ่าน ADC 12-bit 0–4095
  const float volts=raw*ADC_REFERENCE_V/4095.0f; // แปลงเป็น Volt
  latestC=(volts-ZERO_C_V)/VOLTS_PER_C; // แปลงเป็น °C
  if(!isfinite(latestC)){Serial.println("ERROR SENSOR");return;}
  Serial.print("ADC=");Serial.print(raw);Serial.print(" V=");
  Serial.print(volts,4);Serial.print(" T=");Serial.println(latestC,2);
  if(lcdMessage.length()&&millis()-messageAt<5000UL){
    showLine(0,"WEB MESSAGE");showLine(1,lcdMessage);
  }else{
    showLine(0,String("TEMP ")+String(latestC,1)+" C");
    showLine(1,String(latestC>=limitC?"ALERT ":"NORMAL ")+(mqtt.connected()?"MQTT":"OFFLINE"));
  }
  String json=String("{\"temperature_c\":")+String(latestC,2)+
    ",\"adc\":"+String(raw)+",\"voltage_v\":"+String(volts,4)+
    ",\"limit_c\":"+String(limitC,1)+"}"; // JSON สำหรับเว็บ
  if(mqtt.connected())mqtt.publish(TOPIC_DATA,json); // <<< PUBLISH /data
}
void setup(){
  Serial.begin(115200);           // Serial Monitor ที่ 115200 baud
  analogReadResolution(12);       // <<< 12-bit ใช้ 4095 ในสูตร ไม่ใช่ 1023
  Wire.begin();lcd.init();lcd.backlight(); // เริ่ม LCD I2C
  showLine(0,"MCP9700AE TEST");showLine(1,"STARTING...");
  byte mac[6]={0};WiFi.macAddress(mac); // สร้าง MQTT ID จาก MAC
  clientId="mcp9700ae-";for(byte i=0;i<6;i++){if(mac[i]<16)clientId+="0";clientId+=String(mac[i],HEX);}
  mqtt.begin(MQTT_HOST,MQTT_PORT,network); // ตั้ง TCP MQTT
  mqtt.onMessage(onMessage);       // ผูกฟังก์ชันรับข้อความเว็บ
  WiFi.begin(WIFI_SSID,WIFI_PASS); // เชื่อม Wi-Fi เริ่มต้น
  Serial.println(String("PUB ")+TOPIC_DATA);
  Serial.println(String("SUB ")+TOPIC_SET);
}
void loop(){
  connectTick();                   // รับส่ง MQTT และ reconnect
  if(forceRead||millis()-lastSample>=SAMPLE_MS){
    lastSample=millis();forceRead=false;readAndReport(); // อ่าน/แสดง/ส่งขึ้นเว็บ
  }
}
