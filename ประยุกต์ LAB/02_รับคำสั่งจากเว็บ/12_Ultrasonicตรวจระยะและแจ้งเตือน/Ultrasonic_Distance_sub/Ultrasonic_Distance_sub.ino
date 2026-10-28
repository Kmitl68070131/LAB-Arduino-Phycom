/*
  12 Ultrasonicตรวจระยะและแจ้งเตือน / รับคำสั่งจากเว็บ (Subscribe)
  Arduino UNO R4 WiFi | Broker phycom.it.kmitl.ac.th:1883
  Topic prefix 68070131/applied/lab12/sub
  รหัส Wi-Fi ตัวอย่างนี้ถูกระบุตามโจทย์; Repository เป็น public ควรเปลี่ยนรหัสหรือย้ายเป็น secrets ก่อนใช้จริง
  วงจร: ดู README.md และ assets/circuit-lab12.svg
  คำสั่งเว็บตัวอย่าง: 25
  ทดสอบที่คาดหวัง: ตั้งเกณฑ์ 25 ซม.; วัตถุอยู่ที่ 15 ซม. LED บนบอร์ดติด
*/
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <math.h>
float distanceCm=NAN;int thresholdCm=20;
float readDistance(){digitalWrite(2,LOW);delayMicroseconds(2);digitalWrite(2,HIGH);delayMicroseconds(10);digitalWrite(2,LOW);unsigned long duration=pulseIn(3,HIGH,30000UL);return duration==0?NAN:duration*0.0343f/2.0f;}
const char WIFI_SSID[]="EDOK";
const char WIFI_PASSWORD[]="1234567890";
const char MQTT_HOST[]="phycom.it.kmitl.ac.th";
const unsigned int MQTT_PORT=1883;
const char TOPIC_PREFIX[]="68070131/applied/lab12/sub";
const bool SEND_DATA=false;
const bool RECEIVE_COMMANDS=true;
WiFiClient network;
MQTTClient mqtt(1024);
String topicData,topicSet,topicState,clientId;
unsigned long lastSample=0,lastWiFi=0,lastMqtt=0;
const unsigned long SAMPLE_MS=600UL;
bool remoteActive=false,ackPending=false;
String lastAck="BOOT";

// รับคำสั่งจากเว็บ: ไม่ Publish ภายใน callback
bool applyRemote(String payload){
  if(payload.length()>3)return false;int v=payload.toInt();if(v<1||v>400)return false;thresholdCm=v;remoteActive=true;return true;
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
void setupHardware(){pinMode(2,OUTPUT);pinMode(3,INPUT);pinMode(LED_BUILTIN,OUTPUT);}
String sampleHardware(){
  distanceCm=readDistance();if(isnan(distanceCm)){digitalWrite(LED_BUILTIN,LOW);return "NO_ECHO";}digitalWrite(LED_BUILTIN,distanceCm<thresholdCm?HIGH:LOW);return String(distanceCm,1);
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
  clientId="phycom-lab12-sub-";
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
