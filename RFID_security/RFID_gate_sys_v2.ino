#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

//again wit da pinz
#define RST_PIN   3
#define SS_PIN    7   
#define SCK_PIN   4
#define MISO_PIN  5
#define MOSI_PIN  6

//OLED
#define OLED_SDA  8
#define OLED_SCL  9
#define SERVO_PIN 10

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MFRC522 mfrc522(SS_PIN, RST_PIN); 
Servo lockServo;

//sys vars
//good people right here
String tagmiz = "XX XX XX XX";     //replace the tag number with your RFIF card's tag UID
String tagtaj = "XX XX XX XX";     //replace this as well

//Keep track of whether the door is currently open or closed
bool isUnlocked = false; 

//calibrate this (ms) acc to your servo, make dat perform a 360
//you will likely need to adjust this number up or down to get exactly one rotation.
int spinTime360 = 800; 

void setup() {
  Serial.begin(115200);
  
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED failed"));
    for(;;); 
  }
  
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  mfrc522.PCD_Init(); 
  delay(10); 

  ESP32PWM::allocateTimer(0);
  lockServo.setPeriodHertz(50);
  lockServo.attach(SERVO_PIN, 500, 2400); 
  
  //Send "90" to stop a continuous rotation servo from spinning on boot
  lockServo.write(90);

  showHome(); //helper function to draw the HOME screen (pretty imp tbh)
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  //format the scanned UID into a clean String
  String scannedUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    scannedUID += (mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    scannedUID += String(mfrc522.uid.uidByte[i], HEX);
  }
  scannedUID.toUpperCase(); 
  
  //remove the leaading space we added in the loop
  scannedUID.trim(); 

  Serial.print(F("Scanned Tag: "));
  Serial.println(scannedUID);

  //logic gates (adjust acc to your preferencce)

  if (scannedUID == tagmiz || scannedUID == tagtaj) {
    // Determine whose tag was just scanned
    String userName = (scannedUID == tagmiz) ? "MIZAN" : "TAJUL";

    if (!isUnlocked) {
      // It is locked. Let's unlock it!
      displayMessage("Welcome", userName);
      
      // Spin Forward 360 degrees
      lockServo.write(360);      //full speed forward
     // delay(spinTime360);        //wait for it to spin exactly one circle
     // lockServo.write(90);       //stop the motor

      isUnlocked = true;         //update the system state

    } else {
      // It is already unlocked. Let's lock it!
      displayMessage("Goodbye", userName);
      
      // Spin Backward 360 degrees
      lockServo.write(0);        //full speed backward
      //delay(spinTime360);        //wait for it to spin exactly one circle
      //lockServo.write(90);       //sdtop the motor

      isUnlocked = false;        //update the system state
    }

    //let em read n weep, guys
    delay(2000); 
    showHome();

  } else {
    //me when teh theif strike
    display.setCursor(25, 20);
    displayMessage("SYBAU"); //tellem dat
    
    // Buzzing or waiting logic here to punish them
    delay(3000); 
    
    showHome(); 
  }

  //reset
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1(); 
}

//functs

void showHome() {
  display.clearDisplay();
  display.setTextSize(3); // Huge text
  display.setTextColor(SSD1306_WHITE);
  
  // Center the word "HOME" roughly on the screen
  display.setCursor(25, 20);
  display.println(F("HOME"));
  
  display.display();
}

void displayMessage(String line1, String line2) {
  display.clearDisplay();
  display.setTextSize(2);
  
  display.setCursor(0, 10);
  display.println(line1);
  
  display.setCursor(0, 35);
  display.println(line2);
  
  display.display();
}