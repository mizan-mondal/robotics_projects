TRIGGER WARNING: ***THIS PROJECT WILL REQUIRE YOU TO DRILL A SMALL HOLE THROUGH YOUR DOOR TO PASS THE WIRES***

this folder contains all the necessary files you would need to 3D print in order to make this into a fully furnished product.

let us divide this whole thing into two main parts,
1. The outer part (which will contain the RFID-RC522, the OLED screen, and the two binary input touch pins)
2. The inner part, which houses the lock, the ESP32C3, the inner unauthorized touch pin, the battery and the charging module.

now, starting with the outer part, it has two parts as well, the baseplate, onto which the RFID-RC522 and the OLED screen are mounted onto using screws, and the baseplate itself is mounted to the door using screws. the config is given in this folder (instructions.png).

the top part of the outer part is basically a case to cover up the internal components of the system. the two binary input pins are attachewd to this using glue(no screw holes ig)

now, the inner part also consists of these parts:
1. the locking mechanism; which has three parts. the lock sleeves for both the door and the frame, and the lock bar as well(this one is a simple lock, feel free to design a more complicated lock system if you feel like)
2. the servo mechanism- this basically pulls/pushes the lock bar to locked/unlocked state. this has a gear mechanism for doing so. 
3. the ESP32C3 + battery + charging module.

3D print the locking mechanism, along with the other case(both baseplate and topplate)