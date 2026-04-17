Hello People,
This project is to demonstrate the basic functionalities of an ESP32C3 through a fun game. 

Requirements:
1. ESP32C3 super-mini
2. Mini OLED Display
3. A teeny-tiny buzzer
4. A touch Pin Switch (B port should'nt be soldered)
5. Wires and Breadboard
6. will to live (optional)

solder the ESP32C3's pins, proceed to place it on the breadboard in any config you like. place the rest of the components as shown in the circuit diagram. 

connect the ESP32C3 to your computer's port (COM). then using your suitable IDE, flash your code to the ESP.

key point: if this is the first time you are using an ESP32C3, you might have trouble flashing the code to the ESP, as no data exists on it already. so to flash, follow these steps:

a. Hold the BOOT button

b. Press and release RESET

c. Release BOOT

d. Start flashing


if you have followed all steps correctly, you should see the gravity dash game on the OLED screen. Use the touch pin as the input to play the game.