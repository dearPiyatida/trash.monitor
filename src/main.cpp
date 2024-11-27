#include <Arduino.h>
#include <HX711.h>


#define DT 32  // ขา DT ของ HX711
#define SCK 33 // ขา SCK ของ HX711

HX711 scale;
bool status = false; // ใช้ตรวจสอบสถานะว่าอยู่ในโหมดปกติหรือ Calibration
String inputString = "";  
bool stringComplete = false;

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar != '\r' && inChar != '\t' && inChar != '\n') {
      inputString += inChar;
    }
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting HX711 Test...");

  scale.begin(DT, SCK); // เริ่มต้น HX711

  scale.set_scale(-43820.00);  // ค่าเริ่มต้นชั่วคราว
  scale.tare();          // ตั้งค่าน้ำหนักเริ่มต้นเป็น 0
  Serial.println("Taring... Ensure no weight on the scale.");
  delay(2000);
  scale.tare();
  Serial.println("Tare complete.");
}

void loop() {
  serialEvent();
  if (stringComplete) {
    Serial.println("Input: " + inputString);
    if (inputString == "cal") {
      status = true;  // เข้าสู่โหมด Calibration
    } else if (inputString == "read") {
      status = false; // กลับสู่โหมดอ่านค่าน้ำหนักปกติ
    }
    stringComplete = false; // รีเซ็ตสถานะ
    inputString = "";       // เคลียร์ข้อความที่รับเข้ามา
  }

  if (status == false) {
    // โหมดอ่านค่าน้ำหนักปกติ
    if (scale.is_ready()) {
      float weight = scale.get_units(10); // อ่านค่าน้ำหนัก
      Serial.print("Weight: ");
      Serial.print(weight, 2); // แสดงผล 2 ตำแหน่งทศนิยม
      Serial.println(" kg");
    } else {
      Serial.println("HX711 not ready.");
    }
    delay(500); // อ่านค่าทุก 500 มิลลิวินาที
  } else {
    // โหมด Calibration
    Serial.println("Calibration Mode: Place a 1 kg weight on the scale...");
    delay(5000); // รอ 5 วินาทีเพื่อให้ผู้ใช้งานวางน้ำหนัก 1 กิโลกรัม

    float knownWeight = 1.0;  // น้ำหนักมาตรฐาน: 1 กิโลกรัม
    float rawValue = scale.get_units(10); // อ่านค่าดิบเฉลี่ยจาก 10 ครั้ง
    Serial.print("Raw Value: ");
    Serial.println(rawValue);

    // คำนวณ Calibration Factor
    float calibrationFactor = rawValue / knownWeight;

    // ตั้งค่า Calibration Factor
    scale.set_scale(calibrationFactor);
    Serial.print("Calibration Factor set to: ");
    Serial.println(calibrationFactor);

    Serial.println("Calibration complete. Returning to normal mode...");
    status = false; // กลับสู่โหมดปกติ
  }
}
