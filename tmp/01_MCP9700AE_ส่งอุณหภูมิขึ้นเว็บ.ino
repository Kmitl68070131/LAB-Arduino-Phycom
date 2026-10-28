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

// MCP9700AE: VDD -> 5V, GND -> GND, VOUT -> A3
// ตรวจสอบลำดับขาจาก datasheet ของแพ็กเกจจริงก่อนจ่ายไฟ
const byte TEMP_PIN=A3;
const float VREF=5.0; // UNO R4 ใช้ค่าอ้างอิง ADC ตามวงจรจริง; ปรับตามแรงดันที่วัด
const float OFFSET_C=0.0; // ค่าชดเชยหลังเทียบกับเทอร์โมมิเตอร์
unsigned long lastSample=0;
float readCelsius(){
  long sum=0;
  for(int i=0;i<16;i++)sum+=analogRead(TEMP_PIN);
  float raw=sum/16.0;
  float mv=raw*VREF*1000.0/1023.0; // analogReadResolution(10)
  return (mv-500.0)/10.0+OFFSET_C; // MCP9700AE: 500mV ที่ 0°C, 10mV/°C
}
void setup(){
  Serial.begin(115200);
  analogReadResolution(10);
  pinMode(TEMP_PIN,INPUT);
  connectNetwork();
}
void loop(){
  maintainNetwork();
  if(millis()-lastSample>=2000){
    lastSample=millis();
    float c=readCelsius();
    Serial.print("Temperature C: ");Serial.println(c,2);
    if(mqtt.connected()){
      char msg[20];
      dtostrf(c,1,2,msg);
      mqtt.publish("68070131/tmp/temperature",msg);
    }
  }
}
