/*
=========================================================================
  Ultrasonic 2 ตัวตรวจที่นั่งและแสดง LCD
  ประเภท: โจทย์ประยุกต์คาดการณ์เพื่อฝึกทักษะ ไม่ใช่ข้อสอบที่ยืนยันว่าจะออก
  อุปกรณ์: Ultrasonic, LCD
  หน้าที่: อ่าน / ควบคุมอุปกรณ์แล้วแสดงบน LCD และรับส่งค่า MQTT ตาม Topic
  การทำงาน: S1=40 cm/S2=120 cm → BUSY/FREE
  วิธีต่อสาย:
  - S1 HC-SR04 VCC/GND→5V/GND TRIG→D2 ECHO→D3
  - S2 HC-SR04 VCC/GND→5V/GND TRIG→D4 ECHO→D5
  - LCD I2C 16×2: VCC→5V, GND→GND, SDA→SDA, SCL→SCL
  - LCD Address ตัวอย่าง 0x27; ถ้าจอไม่ขึ้น ให้ใช้ I2C Scanner ตรวจที่อยู่จริง
  Broker: phycom.it.kmitl.ac.th | Port: 1883 | Wi-Fi: EDOK
  ข้อมูลที่คาดหวัง: S1=40 cm/S2=120 cm → BUSY/FREE
  Serial Monitor: 115200 baud
  สาย GND ทุกอุปกรณ์ต้องร่วมกัน และตรวจขาจริงก่อนจ่ายไฟ
  GitHub Public: อย่าใช้รหัส Wi-Fi นี้กับระบบที่ต้องการปกปิดรหัส
=========================================================================
*/
#include <WiFiS3.h>              // Wi-Fi ของ Arduino UNO R4 WiFi
#include <MQTTClient.h>          // MQTT by 256dpi สำหรับ Publish/Subscribe
#include <Wire.h>                // ติดต่อจอผ่าน I2C
#include <LiquidCrystal_I2C.h>   // ไลบรารีจอ LCD 16×2
#include <math.h>                // สำหรับสมการ NTC / log() และคณิตศาสตร์

// =====================================================================
// ส่วนที่ 1 — ตั้งค่าเครือข่าย: เปลี่ยน SSID/PASSWORD และ Broker ตรงนี้
// =====================================================================
const char WIFI_SSID[]="EDOK";                // <<< เปลี่ยนชื่อ Wi-Fi ตรงนี้
const char WIFI_PASS[]="1234567890";          // <<< เปลี่ยนรหัส Wi-Fi ตรงนี้
const char MQTT_HOST[]="phycom.it.kmitl.ac.th"; // <<< เปลี่ยน MQTT Host ตรงนี้
const int MQTT_PORT=1883;                     // <<< เปลี่ยน MQTT Port ตรงนี้

// =====================================================================
// ส่วนที่ 2 — MQTT TOPIC: เปลี่ยนข้อความภายใน "..." 3 บรรทัดนี้
// ห้ามสลับทิศทาง Topic: DATA=บอร์ดส่ง; COMMAND=เว็บส่ง; ACK=บอร์ดตอบ
// ชื่อ Topic ต้องตรงกับที่เว็บใช้ทุกตัวอักษร (รวมตัวพิมพ์เล็กใหญ่)
// หากทำข้อสอบ Mock: Topic ด้านล่างตรงตาม Mock แล้ว อย่าเปลี่ยนโดยไม่จำเป็น
// ถ้าเปลี่ยน Topic ของโค้ด ต้องเปลี่ยนเว็บไซต์ catalog / control ด้วย
// =====================================================================
const char TOPIC_DATA[]="68070131/guess/15/data";          // <<< แก้ TOPIC สำหรับ Arduino Publish ขึ้นเว็บตรงนี้
const char TOPIC_COMMAND[]="68070131/guess/15/set";    // <<< แก้ TOPIC สำหรับ Arduino Subscribe จากเว็บตรงนี้
const char TOPIC_ACK[]="68070131/guess/15/ack"; // <<< แก้ TOPIC แจ้งผลรับคำสั่งตรงนี้

// =====================================================================
// ส่วนที่ 3 — ตัวแปรและค่าเวลา: เปลี่ยนช่วงเวลาอ่านค่าได้ที่ SAMPLE_MS
// =====================================================================
WiFiClient network;                       // ช่องสื่อสาร TCP ที่ MQTT ใช้ผ่าน Wi-Fi
MQTTClient mqtt(512);                    // บัฟเฟอร์ข้อความ MQTT 512 bytes
LiquidCrystal_I2C lcd(0x27,16,2);       // <<< Address LCD 0x27 / 0x3F แก้ตรงนี้
const unsigned long SAMPLE_MS=950UL; // <<< ปรับช่วงอ่านอุปกรณ์/ส่งข้อมูล มิลลิวินาที
const unsigned long WIFI_RETRY_MS=10000UL; // ลอง Wi-Fi ใหม่ทุก 10 วินาที
const unsigned long MQTT_RETRY_MS=5000UL;  // ลอง MQTT ใหม่ทุก 5 วินาที
String clientId;                          // MQTT Client ID ไม่ให้ชนกับบอร์ดอื่น
unsigned long lastSample=0;               // เวลาอ่านอุปกรณ์ครั้งล่าสุด
unsigned long lastWifiRetry=0;            // เวลาลองต่อ Wi-Fi ครั้งล่าสุด
unsigned long lastMqttRetry=0;            // เวลาลองต่อ MQTT ครั้งล่าสุด
bool ackPending=false;                    // true = มี ACK รอส่งกลับเว็บไซต์
String ackText="BOOT";                    // ข้อความผลตอบรับคำสั่งจากเว็บ

// =====================================================================
// ส่วนที่ 4 — ตัวแปรของโจทย์: ปรับค่าเกณฑ์และพินจากโค้ดข้างล่าง
// =====================================================================
float limitCm=80;

// =====================================================================
// ส่วนที่ 5 — ฟังก์ชันช่วยแสดง LCD / วัด Ultrasonic
// =====================================================================
void lcdLine(byte row,String message){ // พิมพ์ LCD โดยเคลียร์อักษรเก่าทุกครั้ง
  message=message.substring(0,16);    // LCD กว้าง 16 ตัวอักษร
  while(message.length()<16)message+=' '; // เติมช่องว่างป้องกันอักษรเดิมค้าง
  lcd.setCursor(0,row);               // ย้าย Cursor ไปต้นแถวที่ต้องการ
  lcd.print(message);                 // พิมพ์ข้อความแถวที่เลือก
}
float sonar(byte trigPin,byte echoPin) { // วัดระยะ Ultrasonic หน่วยเซนติเมตร
  digitalWrite(trigPin,LOW);delayMicroseconds(2); // เคลียร์ Trigger ก่อนวัด
  digitalWrite(trigPin,HIGH);delayMicroseconds(10); // ส่งพัลส์ Trigger 10 us
  digitalWrite(trigPin,LOW); // ปิด Trigger ให้เซนเซอร์เริ่มวัด
  unsigned long duration=pulseIn(echoPin,HIGH,30000UL); // ถ้าไม่มี Echo ใน 30ms จบการรอ
  return duration==0?-1.0f:duration*0.0343f/2.0f; // ถ้าไม่พบ Echo คืน -1 ไม่ใช่ 0 cm
}

// =====================================================================
// ส่วนที่ 6 — รับ MQTT: แก้กฎคำสั่งและการควบคุมอุปกรณ์ตรงนี้
// =====================================================================
bool applyCommand(String p){          // p คือ Payload ที่เว็บ Publish มา
  p.trim();                           // ลบช่องว่างที่ไม่ต้องการ
  if(p.startsWith("LIMIT:")){float v=p.substring(6).toFloat();if(v>0&&v<=300){limitCm=v;return true;}}return false;
}
void onMqtt(String &topic,String &message){ // เรียกเมื่อ Broker ส่งข้อความมา
  if(topic!=TOPIC_COMMAND)return;     // รับเฉพาะ TOPIC_COMMAND ไม่รับข้อความของตัวเอง
  if(message.length()>40){ackText="ERROR:TOO_LONG";ackPending=true;return;} // จำกัดความยาว
  bool ok=applyCommand(message);      // ประมวลผลคำสั่งเฉพาะโจทย์
  ackText=ok?"OK:"+message:"ERROR:INVALID"; // บันทึกผลตอบกลับ
  ackPending=true;                    // จะ Publish ACK ใน loop ไม่ทำใน Callback
  Serial.println(String("MQTT RX: ")+message);
}

// =====================================================================
// ส่วนที่ 7 — อ่านเซนเซอร์และแสดงผล: แก้ตรรกะโจทย์ตรงนี้
// =====================================================================
void setupHardware(){                 // กำหนดพิน INPUT/OUTPUT เฉพาะอุปกรณ์
  pinMode(2,OUTPUT);pinMode(3,INPUT);pinMode(4,OUTPUT);pinMode(5,INPUT);
}
String readHardware(){               // คืนข้อความ Payload ที่ต้อง Publish
  float a=sonar(2,3),b=sonar(4,5);bool occupiedA=a>=0&&a<limitCm,occupiedB=b>=0&&b<limitCm;lcdLine(0,String("S1 ")+(a<0?"NO ECHO":occupiedA?"BUSY":"FREE"));lcdLine(1,String("S2 ")+(b<0?"NO ECHO":occupiedB?"BUSY":"FREE"));return String("{\"s1_cm\":")+String(a,1)+",\"s2_cm\":"+String(b,1)+",\"busy1\":"+(occupiedA?"true":"false")+",\"busy2\":"+(occupiedB?"true":"false")+"}";
}

// =====================================================================
// ส่วนที่ 8 — Wi-Fi / MQTT: reconnect อัตโนมัติและ Subscribe หลังเชื่อม
// =====================================================================
void keepConnected(){                // ฟังก์ชันดูแลการเชื่อมต่อ
  unsigned long now=millis();        // เวลาเดินของบอร์ด หน่วย ms
  if(WiFi.status()!=WL_CONNECTED){   // ถ้า Wi-Fi หลุด
    if(now-lastWifiRetry>=WIFI_RETRY_MS){
      lastWifiRetry=now;             // รีเซ็ตเวลา retry
      WiFi.begin(WIFI_SSID,WIFI_PASS); // ขอเชื่อม Wi-Fi ใหม่
    }
    return;                          // ยังไม่พยายามต่อ MQTT จนกว่า Wi-Fi จะมา
  }
  if(!mqtt.connected()){             // ถ้ายังไม่ได้เชื่อม Broker
    if(now-lastMqttRetry>=MQTT_RETRY_MS){
      lastMqttRetry=now;
      if(mqtt.connect(clientId.c_str())){ // Connect ด้วย Client ID ไม่ซ้ำ
        mqtt.subscribe(TOPIC_COMMAND);   // <<< สมัครรับ Topic คำสั่งจากเว็บตรงนี้
        mqtt.publish(TOPIC_ACK,"ONLINE"); // <<< แจ้งเว็บว่าบอร์ดกลับมาออนไลน์
        Serial.println("MQTT CONNECTED");
      }
    }
    return;
  }
  mqtt.loop();                       // สำคัญ: ต้องเรียกบ่อยเพื่อรับคำสั่งและรักษา MQTT
  if(ackPending&&mqtt.publish(TOPIC_ACK,ackText)){ // <<< Publish ACK ไป Topic สถานะ
    ackPending=false;                // ส่งสำเร็จแล้วจึงล้างธง
  }
}

// =====================================================================
// ส่วนที่ 9 — setup() รันตอนเริ่มต้นเพียงครั้งเดียว
// =====================================================================
void setup(){
  Serial.begin(115200);              // เปิด Serial Monitor 115200 baud
  Wire.begin();                      // เริ่มระบบ I2C ต่อจอ LCD
  lcd.init();                        // เริ่มใช้งาน LCD
  lcd.backlight();                   // เปิดไฟ Backlight ของจอ
  lcdLine(0,"PHYCOM GUESS 15");      // แสดงชื่อโจทย์บนจอ
  lcdLine(1,"STARTING ...");        // แจ้งว่ากำลังเปิดระบบ
  setupHardware();                   // ตั้งค่าพินเซนเซอร์ตามโจทย์
  byte mac[6]={0};                   // เก็บ MAC Address 6 ไบต์
  WiFi.macAddress(mac);              // อ่าน MAC เพื่อทำ Client ID ไม่ซ้ำกัน
  clientId="phycom-guess-15-";
  for(byte i=0;i<6;i++){            // แปลง MAC เป็นตัวอักษรฐาน 16
    if(mac[i]<16)clientId+="0";      // เติมเลข 0 ถ้าไบต์สั้น
    clientId+=String(mac[i],HEX);    // ต่อท้าย clientId
  }
  mqtt.begin(MQTT_HOST,MQTT_PORT,network); // ตั้งค่า Broker port และ TCP Client
  mqtt.onMessage(onMqtt);            // ตั้งฟังก์ชันจัดการข้อความจากเว็บ
  WiFi.begin(WIFI_SSID,WIFI_PASS);   // เริ่มเชื่อม Wi-Fi ตามค่าด้านบน
  Serial.println(String("PUB: ")+TOPIC_DATA);     // <<< ดู Topic ส่งข้อมูลได้ใน Serial
  Serial.println(String("SUB: ")+TOPIC_COMMAND);  // <<< ดู Topic รับคำสั่งได้ใน Serial
  Serial.println(String("ACK: ")+TOPIC_ACK);      // <<< ดู Topic สถานะได้ใน Serial
}

// =====================================================================
// ส่วนที่ 10 — loop() ทำงานวนซ้ำตลอดเวลา
// =====================================================================
void loop(){
  keepConnected();                   // คงการเชื่อมและอ่านคำสั่งจาก MQTT
  if(millis()-lastSample<SAMPLE_MS)return; // ยังไม่ถึงเวลาอ่านข้อมูล
  lastSample=millis();               // บันทึกเวลาอ่านล่าสุด
  String value=readHardware();       // อ่านค่าตามโจทย์ / ปรับ LCD
  if(value.length()==0)return;       // Keypad ยังไม่กด ไม่ต้อง Publish
  Serial.println(String("DATA: ")+value); // ดูค่าจริงแม้ Wi-Fi ไม่เชื่อม
  if(mqtt.connected())mqtt.publish(TOPIC_DATA,value); // <<< ส่ง Payload ขึ้นเว็บที่ TOPIC_DATA
}
