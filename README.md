# UFCF8P-15-M-Iot-systems-MicroBit

This project sends encrypted radio commands between two MicroBits to control three LEDs (RED, GREEN, YELLOW). It also plays a short melody for each command so you get audible feedback in addition to the LED state.

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

1. Button A (press): send RED command.
2. Button A (long press ~1.5s): send YELLOW command.
3. Button B (press): send GREEN command.

Only one LED is active at a time on the receiver. Each command also triggers a distinct melody on the receiver speaker.

## Instructions 

- On startup the receiver waits for radio commands from the sender MicroBit. 
- The receiver controls the LEDs based on the decrypted command and displays R, G, or Y on the LED matrix to indicate which color is active. 
- Each command plays a melody:
  - RED: 440 → 660 → 880 Hz
  - GREEN: 880 → 660 → 440 Hz
  - YELLOW: 660 → 880 → 1320 Hz

Uses the event-driven micro:bit runtime (Lancaster University) and the MicroBit message bus.

## Encryption overview

- Algorithm: AES-128 ECB (tiny-AES-c).
- Key derivation: a 1-byte random salt is generated per message on the sender; `makeKey` derives a 32-byte derived private key (DPK) from that salt. The receiver uses the same salt to derive the identical key.
- Payload: the command byte is placed in `plaintext[0]`, padded with zeros to 16 bytes, then encrypted.
- Packet format (17 bytes total): byte 0 = salt (plaintext); bytes 1–16 = AES ciphertext.
- Flow:
  1) Sender picks a random salt (`uBit.random(256)`), derives key, encrypts command, and transmits salt + ciphertext.  
  2) Receiver reads salt from byte 0, derives the same key, decrypts ciphertext, and executes the command (LED + melody).

This design ensures each message uses a fresh key (random salt) while keeping the command itself encrypted over the air.

## Libraries used

- tiny-AES-c (`tiny-AES-c/`): lightweight AES-128 implementation used for encrypting and decrypting the 16-byte command block (ECB mode).
- Light MD5 (`Light-MD5/`): small MD5 library used for generating the derived private key (DPK) in `DPK.cpp`.
- PicoSha2 (`PicoSHA2/`): header-only SHA-256 implementation, used in `makeKey` to compute the SHA-256 hash as the first stage of key derivation.
