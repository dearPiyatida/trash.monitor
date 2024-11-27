#include <Arduino.h>
#include <HX711.h>
#include <Preferences.h> // ไลบรารีสำหรับ NVS

Preferences preferences; // สร้างออบเจ็กต์ Preferences

#define DT 32  // ขา DT ของ HX711
#define SCK 33 // ขา SCK ของ HX711

HX711 scale;
bool status = false; // ใช้ตรวจสอบสถานะว่าอยู่ในโหมดปกติหรือ Calibration
String inputString = "";  
bool stringComplete = false;
float tolerance = 0.05; 

// ฟังก์ชันสำหรับ Calibration
void calibrateScale() {
  Serial.println("Calibration Mode: Place a 1 kg weight on the scale...");
  delay(5000); // รอ 5 วินาทีเพื่อให้ผู้ใช้งานวางน้ำหนัก 1 กิโลกรัม

  float knownWeight = 1.0; // น้ำหนักมาตรฐาน: 1 กิโลกรัม
  float calibrationFactor = 1.0; // เริ่มต้น Calibration Factor
  float step = 1000.0;           // ขนาดการปรับค่าเริ่มต้น
  bool positiveDirection = true; // ทิศทางการปรับค่าเริ่มต้น

  // ทดสอบว่าควรใช้ค่าบวกหรือลบ
  scale.set_scale(1000.0);
  float measuredWeightPositive = scale.get_units(10);

  scale.set_scale(-1000.0);
  float measuredWeightNegative = scale.get_units(10);

  if (fabs(measuredWeightPositive) < fabs(measuredWeightNegative)) {
    positiveDirection = true;
    calibrationFactor = 1000.0; // เริ่มต้นด้วยค่าบวก
    Serial.println("Choosing positive direction.");
  } else {
    positiveDirection = false;
    calibrationFactor = -1000.0; // เริ่มต้นด้วยค่าลบ
    Serial.println("Choosing negative direction.");
  }

  scale.set_scale(calibrationFactor);

  // เริ่มต้นการวนลูปปรับค่า
  while (true) {
    float measuredWeight = scale.get_units(10); // อ่านค่าน้ำหนัก
    Serial.print("Measured Weight: ");
    Serial.println(measuredWeight);

    // ตรวจสอบว่าค่าที่วัดได้ใกล้เคียง 1 กิโลกรัมหรือไม่
    if (fabs(measuredWeight - knownWeight) <= tolerance) {
      Serial.println("Calibration successful!");
      break;
    }

    // ปรับค่าขึ้นหรือลง
    if (measuredWeight < knownWeight) {
      calibrationFactor += step; // เพิ่ม Calibration Factor
    } else {
      calibrationFactor -= step; // ลด Calibration Factor
    }

    // อัปเดต Calibration Factor
    scale.set_scale(calibrationFactor);
    Serial.print("Adjusting Calibration Factor to: ");
    Serial.println(calibrationFactor);

    // ลดขนาด step หาก Calibration Factor ใกล้ถึงค่าที่เหมาะสม
    if (fabs(measuredWeight) < (10 * knownWeight) && step == 1000.0) {
      step = 100.0;
      Serial.println("Reducing step size to 100.");
    } else if (fabs(measuredWeight) < (2 * knownWeight) && step == 100.0) {
      step = 10.0;
      Serial.println("Reducing step size to 10.");
    } else if (fabs(measuredWeight) < (1.5 * knownWeight) && step == 10.0) {
      step = 1.0;
      Serial.println("Reducing step size to 1.");
    }

    delay(500); // หน่วงเวลาเล็กน้อยเพื่ออ่านค่าใหม่
  }

  Serial.println("Final Calibration Factor: ");
  Serial.println(calibrationFactor);
  Serial.println("Calibration complete. Returning to normal mode...");
}

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
  preferences.begin("storage", false); // namespace "storage"
  Serial.println("Starting HX711 Test...");

  scale.begin(DT, SCK); // เริ่มต้น HX711

  if (preferences.isKey("cal")) {
    float getcal = preferences.getFloat("cal", -43820.00 ); // โหลดข้อความ
    Serial.print("Get data from NVS: ");
    Serial.println(getcal);
    scale.set_scale(getcal);
  } else {
    Serial.println("No saved text found in NVS.");
    scale.set_scale(-43820.00);
  }

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
      calibrateScale();
    } else if (inputString == "read") {
      status = false; // กลับสู่โหมดอ่านค่าน้ำหนักปกติ
    }
    stringComplete = false; // รีเซ็ตสถานะ
    inputString = "";       // เคลียร์ข้อความที่รับเข้ามา
  }
  // โหมดอ่านค่าน้ำหนักปกติ
  if (scale.is_ready()) {
    float weight = scale.get_units(10); // อ่านค่าน้ำหนัก
    Serial.print("Weight: ");
    Serial.print(weight, 2); // แสดงผล 2 ตำแหน่งทศนิยม
    Serial.println(" kg");
  } else {
    Serial.println("HX711 not ready.");
  }

}

