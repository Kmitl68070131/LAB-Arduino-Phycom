#include <WiFiS3.h>
#include <PubSubClient.h>
// ตั้งค่า Wi-Fi และ MQTT ของห้องปฏิบัติการ
const char* WIFI_SSID="EDOK";
const char* WIFI_PASS="1234567890";
const char* BROKER="phycom.it.kmitl.ac.th";
WiFiClient network;
PubSubClient mqtt(network);
unsigned long lastReconnect=0;
void connectNetwork(){
  if(WiFi.status()!=WL_CONNECTED){WiFi.begin(WIFI_SSID,WIFI_PASS);}
  mqtt.setServer(BROKER,1883);
}
void maintainNetwork(){
  if(WiFi.status()!=WL_CONNECTED){if(millis()-lastReconnect>5000){lastReconnect=millis();WiFi.begin(WIFI_SSID,WIFI_PASS);}return;}
  if(!mqtt.connected() && millis()-lastReconnect>3000){
    lastReconnect=millis();
    String id="uno-r4-"+String(millis());
    mqtt.connect(id.c_str());
  }
  mqtt.loop();
}

// 7-segment แบบ Common Cathode: ขา A B C D E F G
// ต้องใส่ตัวต้านทานจำกัดกระแสให้แต่ละ segment
const byte PINS[7]={2,3,4,5,6,7,8};
const byte DIGITS[10][7]={
{1,1,1,1,1,1,0},{0,1,1,0,0,0,0},{1,1,0,1,1,0,1},
{1,1,1,1,0,0,1},{0,1,1,0,0,1,1},{1,0,1,1,0,1,1},
{1,0,1,1,1,1,1},{1,1,1,0,0,0,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}};
int displayed=0;
void showDigit(int n){
  if(n<0||n>9)return;
  displayed=n;
  for(byte i=0;i<7;i++)digitalWrite(PINS[i],DIGITS[n][i]?HIGH:LOW);
}
// Subscribe รับตัวเลข 0-9 จากเว็บ แล้วแสดงบน 7-segment
void receive(char* topic,byte* payload,unsigned int len){
  if(String(topic)!="68070131/7seg/set"||len!=1)return;
  if(payload[0]>='0'&&payload[0]<='9'){
    showDigit(payload[0]-'0');
    // Publish ยืนยันค่าที่แสดงจริงกลับไปยังเว็บ
    char value[2]={(char)payload[0],0};
    mqtt.publish("68070131/7seg/state",value);
  }
}
void setup(){
  Serial.begin(115200);
  for(byte i=0;i<7;i++)pinMode(PINS[i],OUTPUT);
  showDigit(0);
  connectNetwork();
  mqtt.setCallback(receive);
}
bool subscribed=false;
void loop(){
  maintainNetwork();
  if(!mqtt.connected()){subscribed=false;return;}
  if(!subscribed){subscribed=mqtt.subscribe("68070131/7seg/set");}
}
