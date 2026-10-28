/*
  04 Keypadควบคุม7เซกเมนต์ / รับคำสั่งจากเว็บ (Subscribe)
  Arduino UNO R4 WiFi | Broker phycom.it.kmitl.ac.th:1883
  Topic prefix 68070131/applied/lab04/sub
  รหัส Wi-Fi ตัวอย่างนี้ถูกระบุตามโจทย์; Repository เป็น public ควรเปลี่ยนรหัสหรือย้ายเป็น secrets ก่อนใช้จริง
  วงจร: ดู README.md และ assets/circuit-lab04.svg
  คำสั่งเว็บตัวอย่าง: 5
  ทดสอบที่คาดหวัง: เว็บสั่ง 5 แสดงเลข 5; กด Keypad 2 กลับแสดง 2 และ Publish 2
*/
#include <WiFiS3.h>
#include <MQTTClient.h>
#include <math.h>
const byte ROWS[4]={2,3,4,5},COLS[4]={6,7,8,9};
const char KEYMAP[4][4]={{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};
char previousKey=0;
void initKeypad(){for(byte i=0;i<4;i++){pinMode(ROWS[i],OUTPUT);digitalWrite(ROWS[i],HIGH);pinMode(COLS[i],INPUT_PULLUP);}}
char scanKey(){char hit=0;for(byte r=0;r<4;r++){digitalWrite(ROWS[r],LOW);delayMicroseconds(10);for(byte c=0;c<4;c++)if(digitalRead(COLS[c])==LOW)hit=KEYMAP[r][c];digitalWrite(ROWS[r],HIGH);}if(!hit){previousKey=0;return 0;}if(previousKey==hit)return 0;previousKey=hit;return hit;}
const byte SEG[7]={10,11,12,13,A0,A1,A2};
const byte DIGITS[10][7]={{1,1,1,1,1,1,0},{0,1,1,0,0,0,0},{1,1,0,1,1,0,1},{1,1,1,1,0,0,1},{0,1,1,0,0,1,1},{1,0,1,1,0,1,1},{1,0,1,1,1,1,1},{1,1,1,0,0,0,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}};
int digit=0;
void showDigit(int x){if(x<0||x>9)return;for(byte i=0;i<7;i++)digitalWrite(SEG[i],DIGITS[x][i]?HIGH:LOW);}
const char WIFI_SSID[]="EDOK";
const char WIFI_PASSWORD[]="1234567890";
const char MQTT_HOST[]="phycom.it.kmitl.ac.th";
const unsigned int MQTT_PORT=1883;
const char TOPIC_PREFIX[]="68070131/applied/lab04/sub";
const bool SEND_DATA=false;
const bool RECEIVE_COMMANDS=true;
WiFiClient network;
MQTTClient mqtt(1024);
String topicData,topicSet,topicState,clientId;
unsigned long lastSample=0,lastWiFi=0,lastMqtt=0;
const unsigned long SAMPLE_MS=60UL;
bool remoteActive=false,ackPending=false;
String lastAck="BOOT";

// รับคำสั่งจากเว็บ: ไม่ Publish ภายใน callback
bool applyRemote(String payload){
  if(payload.length()!=1||payload[0]<'0'||payload[0]>'9')return false;digit=payload.toInt();showDigit(digit);remoteActive=true;return true;
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
void setupHardware(){initKeypad();for(byte i=0;i<7;i++)pinMode(SEG[i],OUTPUT);showDigit(0);}
String sampleHardware(){
  char k=scanKey();if(!k)return String("");if(k>='0'&&k<='9'){digit=k-'0';showDigit(digit);}return String(k);
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
  clientId="phycom-lab04-sub-";
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
