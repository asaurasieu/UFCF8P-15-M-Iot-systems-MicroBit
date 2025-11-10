# UFCF8P-15-M-Iot-systems-MicroBit

This program allows you to control 3 different colored LEDs (RED, GREEN, YELLOW) using the MicroBit devices communicating via radio. 

## HARDWARE 

- 2x MicroBit devices 
- 3x LEDs (RED,GREEN,YELLOW) 
- 3x Resistors 
- Jumper wires 
- Breadboard 

## Setting up the LEDs 

- Red LED: 
    - Postive (long leg) -> Pin P0 of MicroBit 
    - Negative (short leg) -> Resistor -> GND 

- Green LED: 
    - Postive (long leg) -> Pin P1 of MicroBit 
    - Negative (short leg) -> Resistor -> GND 

- Yellow LED:
    - Postive (long leg) -> Pin P2 of MicroBit
    - Negative (short leg) -> Resistor -> GND 

## User Interaction 

1. Button A (pressed once) turns Red LED on 
2. Button A (long press 1,5 seconds) turn on Yellow LED on 
3. Button B (pressed once) turn Green LED on 

Only one LED is active at a time, selecting a new color automatically turns off the previous LED. This ensure we have 3 different states and 3 different commands. 

## Instructions 

- On startup message is display for the User to choose a color "Choose an LED color". 

- Shows intructions "A= Red, Long A= Yellow, B = Green" . 

- Waits for radio commands from the sender MicroBit. 

- The reciever MicroBit will control the 3 LEDs based on the recieved command. 

- Displays R,G or Y on the LED matrix of the MicroBit board to indicate which color is active. 


Uses the event driven library programming from Lancaster University, with the MicroBit message bus. 

## Missing the Encryption for the communication of both MicroBits 






