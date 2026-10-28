/*
  05 LCDแสดงรูปหัวใจ / รับและส่งข้อมูลสองทาง
  Arduino UNO R4 WiFi | Broker phycom.it.kmitl.ac.th:1883
  Topic prefix 68070131/applied/lab05/both
  รหัส Wi-Fi ตัวอย่างนี้ถูกระบุตามโจทย์; Repository เป็น public ควรเปลี่ยนรหัสหรือย้ายเป็น secrets ก่อนใช้จริง
  วงจร: ดู README.md และ assets/circuit-lab05.svg
  คำสั่งเว็บตัวอย่าง: OFF
  ทดสอบที่คาดหวัง: LCD ซ่อนรูปหัวใจ; ส่ง ON จะแสดงหัวใจอีกครั้ง
*/
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <math.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
void lcdLine(byte r,String s){s=s.substring(0,16);while(s.length()<16)s+=" ";lcd.setCursor(0,r);lcd.print(s);}
byte heart[8]={0,10,31,31,14,4,0,0};bool visibleHeart=true;unsigned long ticks=0;
void drawHeart(){lcdLine(0,"I   IT 68070131");lcd.setCursor(2,0);if(visibleHeart)lcd.write((uint8_t)0);else lcd.print(' ');lcdLine(1,String("Seconds ")+String(ticks));}
const char WIFI_SSID[]="EDOK";
const char WIFI_PASSWORD[]="1234567890";
const char MQTT_HOST[]="phycom.it.kmitl.ac.th";
const unsigned int MQTT_PORT=1883;
const char TOPIC_PREFIX[]="68070131/applied/lab05/both";
const bool SEND_DATA=true;
const bool RECEIVE_COMMANDS=true;
WiFiClient network;
MQTTClient mqtt(1024);
String topicData,topicSet,topicState,clientId;
unsigned long lastSample=0,lastWiFi=0,lastMqtt=0;
const unsigned long SAMPLE_MS=1000UL;
bool remoteActive=false,ackPending=false;
String lastAck="BOOT";

// รับคำสั่งจากเว็บ: ไม่ Publish ภายใน callback
bool applyRemote(String payload){
  payload.toUpperCase();if(payload!="ON"&&payload!="OFF")return false;visibleHeart=(payload=="ON");drawHeart();remoteActive=true;return true;
}
void onMqttMessage(String &topic,String &payload){
  if(topic!=topicSet)return;
  payload.trim();
  if(payload.length()==0||payload.length()>32){lastAck="ERROR:SIZE";ackPending=true;return;}
  bool accepted=applyRemote(payload);
  lastAck=accepted?String("OK:")+payload:String("ERROR:VALUE");
  ackPending=true;
  Serial.println(String("RX ")+topic+" "+payload+" -> "+lastAck);
}

// โค้ดเซนเซอร์/เอาต์พุตของ LAB
void setupHardware(){Wire.begin();lcd.init();lcd.backlight();lcd.createChar(0,heart);drawHeart();}
String sampleHardware(){
  ticks=millis()/1000;drawHeart();return visibleHeart?"HEART_ON":"HEART_OFF";
}
void maintainNetwork(){
  unsigned long now=millis();
  if(WiFi.status()!=WL_CONNECTED){
    if(now-lastWiFi>=10000UL){lastWiFi=now;WiFi.begin(WIFI_SSID,WIFI_PASSWORD);}
    return;
  }
  if(!mqtt.connected()){
    if(now-lastMqtt>=5000UL){
      lastMqtt=now;
      if(mqtt.connect(clientId.c_str())){
        Serial.println("MQTT connected");
        if(RECEIVE_COMMANDS)mqtt.subscribe(topicSet);
      }else Serial.println("MQTT retry later");
    }
    return;
  }
  mqtt.loop();
  if(RECEIVE_COMMANDS && SEND_DATA && ackPending){
    if(mqtt.publish(topicState,lastAck))ackPending=false;
  }
}
void setup(){
  Serial.begin(115200);
  setupHardware();
  topicData=String(TOPIC_PREFIX)+"/data";
  topicSet=String(TOPIC_PREFIX)+"/set";
  topicState=String(TOPIC_PREFIX)+"/state";
  byte mac[6]={0};
  WiFi.macAddress(mac);
  clientId="phycom-lab05-both-";
  for(byte i=0;i<6;i++){if(mac[i]<16)clientId+="0";clientId+=String(mac[i],HEX);}
  mqtt.begin(MQTT_HOST,MQTT_PORT,network);
  mqtt.onMessage(onMqttMessage);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  Serial.println(String("MQTT prefix: ")+TOPIC_PREFIX);
}
void loop(){
  maintainNetwork();
  if(millis()-lastSample<SAMPLE_MS)return;
  lastSample=millis();
  if(!SEND_DATA)return; // โหมด Subscribe ควบคุมจากเว็บอย่างเดียว
  String measurement=sampleHardware();
  if(measurement.length()==0)return;
  Serial.println(String("LOCAL ")+measurement);
  if(mqtt.connected())mqtt.publish(topicData,measurement);
}
