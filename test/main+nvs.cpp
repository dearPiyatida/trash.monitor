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





//  String getValue(String data, char separator, int index)
// {
//   int found = 0;
//   int strIndex[] = {0, -1};
//   int maxIndex = data.length()-1;

//   for(int i=0; i<=maxIndex && found<=index; i++){
//     if(data.charAt(i)==separator || i==maxIndex){
//         found++;
//         strIndex[0] = strIndex[1]+1;
//         strIndex[1] = (i == maxIndex) ? i+1 : i;
//     }
//   }

//   return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
// }

// void setup() {
//   Serial.begin(115200);  
//   Serial.println("Start!!!");
// }

// void processCommand(String cmd, String inputString) {
//   String data1 = getValue(inputString, ',', 1);
//   String data2 = getValue(inputString, ',', 2);
//   int num1 = data1.toInt();
//   int num2 = data2.toInt();
//   float ans = 0; // ใช้ float สำหรับรองรับการหาร

//   if (cmd == "plus") {
//     ans = num1 + num2;
//   } else if (cmd == "minus") {
//     ans = num1 - num2;
//   } else if (cmd == "multiply") {
//     ans = num1 * num2;
//   } else if (cmd == "divide") {
//     if (num2 != 0) { // ตรวจสอบหารด้วยศูนย์
//       ans = (float)num1 / num2;
//     } else {
//       Serial.println("Error: Division by zero!");
//       return; // ออกจากฟังก์ชันหากหารด้วยศูนย์
//     }
//   } else {
//     Serial.println("Unknown command!");
//     return; // ออกจากฟังก์ชันหากคำสั่งไม่ถูกต้อง
//   }

//   Serial.println("Result: " + String(ans));
// }

// void loop() {
//   serialEvent();
//   if (stringComplete) {
//     Serial.println("Input: " + inputString);
//     String cmd = getValue(inputString, ',', 0);
//     Serial.println("Command: " + cmd);

//     // เรียกฟังก์ชันเพื่อประมวลผลคำสั่ง
//     processCommand(cmd, inputString);

//     stringComplete = false; // รีเซ็ตสถานะ
//     inputString = "";       // เคลียร์ข้อความที่รับเข้ามา
//   }
// }


// void loop() {
//   serialEvent();
//   if (stringComplete) {
//     Serial.println("Input: " + inputString);
//     String cmd = getValue(inputString,',',0);
//     Serial.println(cmd);
//     if(cmd == "plus"){
//        String data1 = getValue(inputString,',',1);
//        String data2 = getValue(inputString,',',2);
//        int num1 = data1.toInt();
//        int num2 = data2.toInt();
//        int ans = num1+num2;
//        Serial.println(ans);
//     }else if(cmd == "minus"){
//       String data1 = getValue(inputString,',',1);
//        String data2 = getValue(inputString,',',2);
//        int num1 = data1.toInt();
//        int num2 = data2.toInt();
//        int ans = num1+num2;
//        Serial.println(ans);

//     }
    
    /*
    if (inputString.startsWith("plus")) {
     
      int firstComma = inputString.indexOf(',');
      int secondComma = inputString.indexOf(',', firstComma + 1);

      if (firstComma != -1 && secondComma != -1) {
        String firstNumber = inputString.substring(firstComma + 1, secondComma);
        String secondNumber = inputString.substring(secondComma + 1);

        
        int num1 = firstNumber.toInt();
        int num2 = secondNumber.toInt();
        int result = num1 + num2;

       
        // Serial.print("Result: ");
        Serial.println(result);
      } else {
        Serial.println("Invalid command format. Use: plus,1,1");
      }
    }
    */
    
//     inputString = "";
//     stringComplete = false;
//   }
// }

// void loop() {
//  if (stringComplete) {
//     Serial.print("You said: ");
//     Serial.println(inputString); // แสดงข้อความที่รับมา

//     // ตอบกลับแบบหวาน ๆ
//     Serial.println("Aww, you're the best! ❤️");

//     inputString = ""; // รีเซ็ตข้อความ
//     stringComplete = false;
//   }
// }

// void serialEvent() {
//   while (Serial.available()) {
//     char inChar = (char)Serial.read();
//     inputString += inChar;
//     if (inChar == '\n') {
//       stringComplete = true;
//     }
//   }
// }