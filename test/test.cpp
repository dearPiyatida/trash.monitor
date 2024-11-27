#include <Arduino.h>
#include <HX711.h>
#include <Preferences.h> // ไลบรารีสำหรับจัดเก็บข้อมูลใน NVS

#define DT 32  // ขา DT ของ HX711
#define SCK 33 // ขา SCK ของ HX711

HX711 scale;
Preferences preferences; // สร้างออบเจ็กต์ Preferences
bool status = false; // ใช้ตรวจสอบสถานะว่าอยู่ในโหมดปกติหรือ Calibration
String inputString = "";  
bool stringComplete = false;

float calibrationFactor = -43820.00; // ค่าเริ่มต้นของ Calibration Factor

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

  preferences.begin("loadcell", false); // เริ่มต้น Preferences

  // โหลดค่าที่เคยบันทึกไว้
  if (preferences.isKey("calFactor")) {
    calibrationFactor = preferences.getFloat("calFactor", -43820.00);
    Serial.println("Loaded Calibration Factor from memory:");
  } else {
    Serial.println("No Calibration Factor found in memory, using default.");
  }
  Serial.print("Calibration Factor: ");
  Serial.println(calibrationFactor);

  scale.begin(DT, SCK); // เริ่มต้น HX711
  scale.set_scale(calibrationFactor);  // ตั้งค่าจาก Calibration Factor ที่โหลดมา
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
    calibrationFactor = rawValue / knownWeight;

    // ตั้งค่า Calibration Factor
    scale.set_scale(calibrationFactor);
    Serial.print("Calibration Factor set to: ");
    Serial.println(calibrationFactor);

    // บันทึกค่า Calibration Factor ลงใน NVS
    preferences.putFloat("calFactor", calibrationFactor);
    Serial.println("Calibration Factor saved to memory.");

    Serial.println("Calibration complete. Returning to normal mode...");
    status = false; // กลับสู่โหมดปกติ
  }
}



