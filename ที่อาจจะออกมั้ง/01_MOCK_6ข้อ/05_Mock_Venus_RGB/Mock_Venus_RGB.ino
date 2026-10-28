/*
=========================================================================
  Mock 05 — รับอุณหภูมิแล้วเปลี่ยนสี RGB
  ประเภท: แบบฝึกจากข้อสอบ Mock — ห้ามเปลี่ยน payload หลักที่ Mock คาดหวัง
  อุปกรณ์: RGB, LCD
  หน้าที่: อ่าน / ควบคุมอุปกรณ์แล้วแสดงบน LCD และรับส่งค่า MQTT ตาม Topic
  การทำงาน: 19→เขียว, 31→น้ำเงิน, 46→แดง, 55→ดับ
  วิธีต่อสาย:
  - RGB Common Cathode COM→GND
  - RGB R/G/B ผ่านตัวต้านทาน 330Ω แยกสาย → D9/D10/D11
  - LCD I2C 16×2: VCC→5V, GND→GND, SDA→SDA, SCL→SCL
  - LCD Address ตัวอย่าง 0x27; ถ้าจอไม่ขึ้น ให้ใช้ I2C Scanner ตรวจที่อยู่จริง
  Broker: phycom.it.kmitl.ac.th | Port: 1883 | Wi-Fi: EDOK
  ข้อมูลที่คาดหวัง: 19→เขียว, 31→น้ำเงิน, 46→แดง, 55→ดับ
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
const char TOPIC_DATA[]="68070131/guess/05/data";          // <<< แก้ TOPIC สำหรับ Arduino Publish ขึ้นเว็บตรงนี้
const char TOPIC_COMMAND[]="68070131/venus";    // <<< แก้ TOPIC สำหรับ Arduino Subscribe จากเว็บตรงนี้
const char TOPIC_ACK[]="68070131/guess/05/ack"; // <<< แก้ TOPIC แจ้งผลรับคำสั่งตรงนี้

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
String color="OFF";float lastTemperature=0;void showRGB(bool r,bool g,bool b){digitalWrite(9,r?HIGH:LOW);digitalWrite(10,g?HIGH:LOW);digitalWrite(11,b?HIGH:LOW);}

// =====================================================================
// ส่วนที่ 5 — ฟังก์ชันช่วยแสดง LCD / วัด Ultrasonic
// =====================================================================
void lcdLine(byte row,String message){ // พิมพ์ LCD โดยเคลียร์อักษรเก่าทุกครั้ง
  message=message.substring(0,16);    // LCD กว้าง 16 ตัวอักษร
  while(message.length()<16)message+=' '; // เติมช่องว่างป้องกันอักษรเดิมค้าง
  lcd.setCursor(0,row);               // ย้าย Cursor ไปต้นแถวที่ต้องการ
  lcd.print(message);                 // พิมพ์ข้อความแถวที่เลือก
}


// =====================================================================
// ส่วนที่ 6 — รับ MQTT: แก้กฎคำสั่งและการควบคุมอุปกรณ์ตรงนี้
// =====================================================================
bool applyCommand(String p){          // p คือ Payload ที่เว็บ Publish มา
  p.trim();                           // ลบช่องว่างที่ไม่ต้องการ
  if(p.length()>10||p.length()==0)return false;for(unsigned i=0;i<p.length();i++){char ch=p[i];if(!((ch>='0'&&ch<='9')||ch=='.'||ch=='-'))return false;}lastTemperature=p.toFloat();if(lastTemperature>=10&&lastTemperature<=25){color="GREEN";showRGB(false,true,false);}else if(lastTemperature>=26&&lastTemperature<=35){color="BLUE";showRGB(false,false,true);}else if(lastTemperature>=36&&lastTemperature<=50){color="RED";showRGB(true,false,false);}else{color="OFF";showRGB(false,false,false);}lcdLine(0,String("RX TEMP ")+String(lastTemperature,1));lcdLine(1,"RGB "+color);return true;
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
  pinMode(9,OUTPUT);pinMode(10,OUTPUT);pinMode(11,OUTPUT);showRGB(false,false,false);
}
String readHardware(){               // คืนข้อความ Payload ที่ต้อง Publish
  return color;
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
  lcdLine(0,"PHYCOM GUESS 05");      // แสดงชื่อโจทย์บนจอ
  lcdLine(1,"STARTING ...");        // แจ้งว่ากำลังเปิดระบบ
  setupHardware();                   // ตั้งค่าพินเซนเซอร์ตามโจทย์
  byte mac[6]={0};                   // เก็บ MAC Address 6 ไบต์
  WiFi.macAddress(mac);              // อ่าน MAC เพื่อทำ Client ID ไม่ซ้ำกัน
  clientId="phycom-guess-05-";
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
