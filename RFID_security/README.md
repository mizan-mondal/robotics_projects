so you wanna build a door lock that uses RFID authentication ?
perhaps you landed on the right page

Requirements:

1. RFID-RC522 Scanner + RFID cards/tags
2. OLED screen (optional, but highly recommended)
3. an ESP32C3 (this project uses ESP, but you may opt for Arduino as well), or any microcontroller you prefer (this would require you to change the entire code pa`rtially/completely)
4. wires (use thinner wires for better final finished product)
5. a servo motor(preferably MG90S, or you may opt for any suitable motor drive system you prefer)
6. a door (you will require to drill a hole into it)
7. locking mechanism (3D printable files in the folder 'RFID_security/printables')

optionals:

8. 3D printed case (also available in the printables folder)

solder the ESP32C3's pins, proceed to place it on the breadboard in any config you like. place the rest of the components as shown in the circuit diagram. 

connect the ESP32C3 to your computer's port (COM). then using your suitable IDE, flash your code to the ESP.

key point: if this is the first time you are using an ESP32C3, you might have trouble flashing the code to the ESP, as no data exists on it already. so to flash, follow these steps:

a. Hold the BOOT button

b. Press and release RESET

c. Release BOOT

d. Start flashing

assuming you've done everything carefully and correctly, you should have a working security system that uses RFID to allow people who are registered into the system.

keynote: the user access data is hardcoded into the ESP32C3 onto the code itself, so any addition/deletion of any users would require you to add an user in the code itself.

have fun, and don't lock yourself out.

if you want to check the UID of your RFID cards, compile the "check_UID.ino" file onto the ESP, and it'll show the UID on the serial monitor. make sure to set the baud rate to 115200, and go to 'Tools' -> "USB CDC on Boot" to 'Enabled'.