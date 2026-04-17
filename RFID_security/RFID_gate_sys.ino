#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>
//pins

//RFID
#define RST_PIN   3
#define SS_PIN    7   
#define SCK_PIN   4
#define MISO_PIN  5
#define MOSI_PIN  6
//OLED

#define OLED_SDA  8
#define OLED_SCL  9

//MG90S
#define SERVO_PIN 10

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MFRC522 mfrc522(SS_PIN, RST_PIN); 
Servo lockServo;

void setup() {
  Serial.begin(115200);
  
  //OLED config thyme
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED failed to initialize"));
    for(;;); 
  }
  
  //doin da RFID
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  mfrc522.PCD_Init(); 
  delay(10); 

  //move daa servo, lock dat shi
  ESP32PWM::allocateTimer(0);
  lockServo.setPeriodHertz(50);
  lockServo.attach(SERVO_PIN, 500, 2400); 
  
  //the lock should'nt be left open right ?
  lockServo.write(0);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println(F("Lock System Ready"));
  display.setCursor(0, 30);
  display.println(F("Scan your tag."));
  display.display();
  
  Serial.println(F("Knock Knock, who dis ?"));
}

void loop() {
  //cardsearch
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  //UIDcontainer
  String tagUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    tagUID += (mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    tagUID += String(mfrc522.uid.uidByte[i], HEX);
  }
  tagUID.toUpperCase(); 

  Serial.print(F("Welcome"));
  Serial.println(tagUID);

  //welcometypeshi

  //OLED refresh
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("TAG SCANNED:"));
  display.setTextSize(2); 
  display.setCursor(0, 15);
  display.println(tagUID);
  display.setTextSize(1);
  display.setCursor(0, 45);
  display.println(F(">> UNLOCKED <<"));
  display.display();

  //spin the servo motor - (deg)
  lockServo.write(90);

  //how long does it take one to enter ?
  delay(3000);

  //lock

  lockServo.write(0);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.println(F("Lock System Ready"));
  display.setCursor(0, 30);
  display.println(F("Scan your tag."));
  display.display();

  //resrt
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1(); 
}