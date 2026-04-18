#include <SPI.h>
#include <MFRC522.h>

// --- PIN DEFINITIONS FOR ESP32-C3 ---
#define RST_PIN   3
#define SS_PIN    7   // SDA on RC522
#define SCK_PIN   4
#define MISO_PIN  5
#define MOSI_PIN  6

// Create the MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN); 

void setup() {
  // Start the serial monitor
  Serial.begin(115200); 
  
  // Wait a moment for the Serial bus to stabilize
  delay(1000); 

  // Force the SPI bus to use our specific C3 pins
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  
  // Initialize the MFRC522 reader
  mfrc522.PCD_Init(); 
  
  // A short delay helps some RC522 boards boot up properly
  delay(10); 

  Serial.println(F("======================================="));
  Serial.println(F("       RFID SCANNER READY              "));
  Serial.println(F("======================================="));
  Serial.println(F("Tap a card or keychain to the reader..."));
}

void loop() {
  // Look for new cards. If none are present, restart the loop.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // If a card is present, select it so we can read it.
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Print the UID to the Serial Monitor
  Serial.print(F("SUCCESS! Tag UID:"));
  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // Add a leading zero if the hex value is single-digit for clean formatting
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Put the card to sleep so it doesn't spam the serial monitor with the same read
  mfrc522.PICC_HaltA();
  
  // Stop encryption on PCD (important for clean subsequent reads)
  mfrc522.PCD_StopCrypto1(); 
}