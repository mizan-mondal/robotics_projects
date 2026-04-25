#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

//rfid pins
#define RST_PIN 3
#define SS_PIN 7   
#define SCK_PIN 4
#define MISO_PIN 5
#define MOSI_PIN 6

//oled
#define OLED_SDA 8
#define OLED_SCL 9
#define SERVO_PIN 10 //prettyselfexplanatory

//touch pins (thiis one uses three pins, one is placed inside the gate, which is used to unlock the gate from inside, as locking/unlocking from outside shouldnt requre any credential/auth)
#define INSIDE_BTN_PIN 1
//two seperate touch pins are placed outside which are sort of like a binary input, which can be used to unlock the gate from outside, in case you forget/misplace the RFID cards
#define OUTSIDE_BTN_1  20
#define OUTSIDE_BTN_0  21

//sum shi jus write this, nonchalant screen settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MFRC522 mfrc522(SS_PIN, RST_PIN); 
Servo lockServo;

//tag ids (use the check_UID.ino to find out what is the UID of your RFID cards)
String tagmiz = "XX XX XX XX";
String tagtaj = "XX XX XX XX"; //incase you got a roomie, comment it out if you live alone :(

bool isUnlocked = false; 

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 300; 

//emergency binary password
String targetCode = "XXXXXXXXX"; //replace with your preffered binary password
String currentCode = "";
unsigned long lastCodeTime = 0;
const unsigned long codeTimeout = 2000; 

//incase we want a BMS with a cell attached, we dont wanna draw too much power out of the system. so do this
unsigned long lastActivityTime = 0;
const unsigned long screenTimeout = 10000; 
bool screenIsAsleep = false;

void setup() {
  Serial.begin(115200);
  
  pinMode(INSIDE_BTN_PIN, INPUT_PULLDOWN);
  pinMode(OUTSIDE_BTN_1, INPUT_PULLDOWN);
  pinMode(OUTSIDE_BTN_0, INPUT_PULLDOWN);
  
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED failed")); //hope you never have to see this
    for(;;); 
  }
  
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  mfrc522.PCD_Init(); 
  
  ESP32PWM::allocateTimer(0);
  lockServo.setPeriodHertz(50);
  lockServo.attach(SERVO_PIN, 500, 2400); 
  
  //System boots up in the LOCKED state, for security purposes
  lockServo.write(180); 
  isUnlocked = false;

  wakeScreen();
  showHome(); 
}

void loop() {
  unsigned long currentMillis = millis();

  //power mgmt
  if (!screenIsAsleep && (currentMillis - lastActivityTime > screenTimeout)) {
    display.ssd1306_command(SSD1306_DISPLAYOFF); 
    screenIsAsleep = true;
  }

  //emergency 0 1 logic
  if (currentCode.length() > 0 && (currentMillis - lastCodeTime > codeTimeout)) {
    currentCode = ""; 
    wakeScreen();
    showHome();
  }

  if (digitalRead(OUTSIDE_BTN_1) == HIGH && (currentMillis - lastDebounceTime > debounceDelay)) {
    wakeScreen();
    currentCode += "1";
    lastCodeTime = currentMillis;
    lastDebounceTime = currentMillis;
    displayInputStatus("Input: 1");
  }
  
  if (digitalRead(OUTSIDE_BTN_0) == HIGH && (currentMillis - lastDebounceTime > debounceDelay)) {
    wakeScreen();
    currentCode += "0";
    lastCodeTime = currentMillis;
    lastDebounceTime = currentMillis;
    displayInputStatus("Input: 0");
  }

  if (currentCode == targetCode) {
    currentCode = ""; 
    if (!isUnlocked) {
      displayMessage("Emergency", "Unlock");
      triggerServoUnlock();
      delay(2000);
      showHome();
    } else {
      displayMessage("Already", "Unlocked");
      delay(2000);
      showHome();
    }
  } else if (currentCode.length() >= targetCode.length()) {
    displayMessage("Invalid", "Code");
    currentCode = "";
    delay(2000);
    showHome();
  }

  //for the pin inside
  if (digitalRead(INSIDE_BTN_PIN) == HIGH && (currentMillis - lastDebounceTime > debounceDelay)) {
    wakeScreen();
    lastDebounceTime = currentMillis;
    
    if (isUnlocked) {
      displayMessage("Locked", "");
      triggerServoLock();
    } else {
      displayMessage("Unlocked", "");
      triggerServoUnlock();
    }
    delay(2000);
    showHome();
  }

  //rfid logic
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    wakeScreen();
    
    String scannedUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      scannedUID += (mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      scannedUID += String(mfrc522.uid.uidByte[i], HEX);
    }
    scannedUID.toUpperCase(); 
    scannedUID.trim(); 

    if (scannedUID == tagmiz || scannedUID == tagtaj) {
      String userName = (scannedUID == tagmiz) ? "Mizan" : "Tajul"; //switch conditional statement to if else logic if you need more users

      if (!isUnlocked) {
        displayMessage("Welcome", userName);
        triggerServoUnlock();
      } else {
        displayMessage("Goodbye", userName);
        triggerServoLock();
      }
    } else {
      displayMessage("SYBAU !!!", ""); 
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1(); 
    delay(2000);
    showHome();
  }
}

//helper func

void wakeScreen() {
  if (screenIsAsleep) {
    display.ssd1306_command(SSD1306_DISPLAYON);
    screenIsAsleep = false;
  }
  lastActivityTime = millis();
}

void showHome() {
  display.clearDisplay();
  display.setTextSize(3); 
  display.setTextColor(SSD1306_WHITE);
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

void displayInputStatus(String input) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(10, 25);
  display.println(input);
  
  display.setCursor(0, 50);
  for(int i=0; i<currentCode.length(); i++) display.print(".");
  display.display();
}

//dual state for the servo
void triggerServoUnlock() {
  lockServo.write(0); // Move to Unlocked position(0 degrees)
  isUnlocked = true;
}

void triggerServoLock() {
  lockServo.write(180); // Move to Locked position(180 degrees)
  isUnlocked = false;
}

