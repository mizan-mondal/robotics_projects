#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Pin Definitions ---
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5
#define TOUCH_PIN   2
#define BUZZER_PIN  6

// --- OLED Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// --- Game State Variables ---
int gameState = 0; // 0 = Title, 1 = Playing, 2 = Game Over
int score = 0;
bool lastTouchState = false;

// --- Player (Cube) Variables ---
int playerX = 20;
int playerSize = 10;
bool onCeiling = false;

// --- Obstacle (Spike) Variables ---
float obsX = 128;
int obsWidth = 8;
int obsHeight = 16;
float obsSpeed = 3.0;
bool obsIsOnCeiling = false;

void setup() {
  Serial.begin(115200);

  pinMode(TOUCH_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH); // HIGH = OFF for your active-low buzzer

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;);
  }
}

void loop() {
  // Read touch input with basic debouncing
  bool currentTouch = digitalRead(TOUCH_PIN);
  bool isTapped = (currentTouch == HIGH && lastTouchState == LOW);
  lastTouchState = currentTouch;

  display.clearDisplay();

  // ==========================================
  // STATE 0: TITLE SCREEN
  // ==========================================
  if (gameState == 0) {
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(15, 15);
    display.print("GRAVITY");
    display.setCursor(40, 35);
    display.print("DASH");
    
    // Blinking prompt
    if (millis() % 1000 < 500) {
      display.setTextSize(1);
      display.setCursor(25, 55);
      display.print("Tap to Start!");
    }

    if (isTapped) {
      resetGame();
      gameState = 1;
      beep(100);
    }
  }

  // ==========================================
  // STATE 1: PLAYING
  // ==========================================
  else if (gameState == 1) {
    
    // 1. Handle Input (Flip Gravity)
    if (isTapped) {
      onCeiling = !onCeiling; // Toggle gravity
      beep(15); // Tiny clicking sound for the flip
    }

    // Determine player Y coordinate based on gravity
    int playerY = onCeiling ? 0 : (SCREEN_HEIGHT - playerSize);

    // 2. Move Obstacle
    obsX -= obsSpeed;

    // 3. Reset Obstacle & Score Points
    if (obsX < -obsWidth) {
      obsX = SCREEN_WIDTH;
      score++;
      obsSpeed += 0.2; // Game gets slightly faster every point!
      
      // Randomly decide if next obstacle is on floor or ceiling
      obsIsOnCeiling = random(0, 2); 
    }

    // Determine obstacle Y coordinate
    int currentObsY = obsIsOnCeiling ? 0 : (SCREEN_HEIGHT - obsHeight);

    // 4. Draw Environment
    display.drawLine(0, 0, 128, 0, SSD1306_WHITE); // Ceiling line
    display.drawLine(0, 63, 128, 63, SSD1306_WHITE); // Floor line

    // Draw Score
    display.setTextSize(1);
    display.setCursor(55, 28);
    display.print(score);

    // Draw Player
    display.fillRect(playerX, playerY, playerSize, playerSize, SSD1306_WHITE);

    // Draw Obstacle (looks like a spike)
    display.fillRect((int)obsX, currentObsY, obsWidth, obsHeight, SSD1306_WHITE);

    // 5. Collision Detection (AABB)
    if (playerX < obsX + obsWidth && playerX + playerSize > obsX &&
        playerY < currentObsY + obsHeight && playerY + playerSize > currentObsY) {
      // BOOM! Hit a spike.
      gameState = 2;
      digitalWrite(BUZZER_PIN, LOW); // Turn buzzer ON
      delay(300);                    // Crash sound length
      digitalWrite(BUZZER_PIN, HIGH); // Turn buzzer OFF
    }
  }

  // ==========================================
  // STATE 2: GAME OVER
  // ==========================================
  else if (gameState == 2) {
    display.setTextSize(2);
    display.setCursor(10, 15);
    display.print("CRASHED!");
    
    display.setTextSize(1);
    display.setCursor(35, 40);
    display.print("Score: ");
    display.print(score);
    
    if (millis() % 1000 < 500) {
      display.setCursor(25, 55);
      display.print("Tap to Retry");
    }

    if (isTapped) {
      resetGame();
      gameState = 1;
      beep(100);
    }
  }

  display.display();
}

// ==========================================
// HELPER FUNCTIONS
// ==========================================
void resetGame() {
  score = 0;
  obsX = 128;
  obsSpeed = 3.0;
  onCeiling = false;
  obsIsOnCeiling = false;
}

void beep(int duration) {
  digitalWrite(BUZZER_PIN, LOW);   // ON
  delay(duration);
  digitalWrite(BUZZER_PIN, HIGH);  // OFF
}