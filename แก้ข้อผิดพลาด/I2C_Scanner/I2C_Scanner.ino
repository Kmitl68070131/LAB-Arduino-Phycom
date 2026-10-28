/*
  I2C Scanner ทดสอบ LCD และ MCP23017 บน Arduino UNO R4 WiFi
  สาย: VCC→5V, GND→GND, SDA→SDA, SCL→SCL
  LCD มักเป็น 0x27 หรือ 0x3F | MCP23017 เมื่อ Address A0/A1/A2→GND เป็น 0x20
  Serial Monitor 115200
*/
#include <Wire.h>                  // ติดต่ออุปกรณ์ผ่าน I2C
void setup(){
  Serial.begin(115200);            // แสดงรายการอุปกรณ์ที่ค้นพบ
  Wire.begin();                    // เปิด I2C Bus บนขา SDA/SCL
}
void loop(){
  Serial.println("Scanning I2C..."); // เริ่มรอบสแกน
  int count=0;                     // จำนวน Address ที่ตอบกลับ
  for(byte address=8;address<120;address++){
    Wire.beginTransmission(address); // ทดสอบว่า Address มีอุปกรณ์หรือไม่
    byte error=Wire.endTransmission();// 0 = อุปกรณ์ตอบ ACK
    if(error==0){
      Serial.print("Found 0x");   // พิมพ์ Address ในฐาน 16
      if(address<16)Serial.print("0");
      Serial.println(address,HEX);
      count++;
    }
  }
  if(count==0)Serial.println("NO I2C DEVICES - check wiring!");
  delay(2500);                     // เว้นเวลารอบต่อไป
}