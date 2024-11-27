#include <Preferences.h> // ไลบรารีสำหรับ NVS

Preferences preferences; // สร้างออบเจ็กต์ Preferences

String inputString = "";   // เก็บข้อความที่พิมพ์ผ่าน Serial
bool stringComplete = false; // สถานะว่าได้รับข้อความครบหรือยัง

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar != '\r' && inChar != '\n') {
      inputString += inChar; // บันทึกข้อความ
    }
    if (inChar == '\n') {
      stringComplete = true; // เมื่อกด Enter
    }
  }
}

void setup() {
  Serial.begin(115200);

  // เริ่มต้น Preferences (NVS)
  preferences.begin("storage", false); // namespace "storage"

  // ลองโหลดข้อความที่บันทึกไว้
  if (preferences.isKey("savedText")) {
    String savedText = preferences.getString("savedText", "No Data"); // โหลดข้อความ
    Serial.print("Loaded saved text: ");
    Serial.println(savedText);
  } else {
    Serial.println("No saved text found in NVS.");
  }
}

void loop() {
  serialEvent();
  if (stringComplete) {
    Serial.print("You entered: ");
    Serial.println(inputString);

    // บันทึกข้อความลงใน NVS
    preferences.putString("savedText", inputString);
    Serial.println("Text saved in NVS.");

    // เคลียร์ข้อความ
    inputString = "";
    stringComplete = false;
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