#include "MicroBit.h"
#include "DPK.h"
#include "aes.h"

MicroBit uBit; 

#define r_freq 440
#define g_freq 880
#define y_freq 1320
#define duration 500 

void play_tone(uint8_t command)
{
    int frequency; 
    
    if (command == 1){

        frequency = r_freq; 

    } else if (command == 2){
        frequency = g_freq; 

    } else if (command == 3){
        frequency = y_freq; 
    }else {
        return; 
    }

    uBit.io.P0.setAnalogValue(225); 
    uBit.io.P0.setAnalogPeriodUs(1000000 / frequency); 
    uBit.io.P0.setAnalogValue(128); 
    uBit.sleep(duration); 
    uBit.io.P0.setAnalogValue(0); 
}


void encrypt(uint8_t commandValue)
{
    uint8_t salt = {command}; 
    uint8_t dpk; 
    makeKey(salt, 1, dpk); 

    char nibbleSymbol = 'A' + (key[0] & 0x0F);
    uBit.display.print(nibbleSymbol);
    uBit.sleep(500); 

    uint8_t plaintext[16]; 
    memset(plaintext, 0, 16); 
    plaintext[0] = command; 

    // Initialize AES context with dpk 
    struct AES_ctx ctx; 
    AES_init_ctx(&ctx, dpk); 

    // Encrypt plaintext using AES-128 ECB mode 
    uint8_t ciphertext[16]; 
    memcpy(ciphertext, plaintext, 16); 
    AES_ECB_encrypt(&ctx, ciphertext); 

    PacketBuffer b(17); 
    b[0] = salt[0]; 

    for (int i = 0; i < 16; i++){
        buffer[i + 1] = ciphertext[i]; 
   }

    uBit.radio.datagram.send(b); 

    play_tone(command); 
}

void onButtonA(MicroBitEvent e)
{
    if (e.value == MICROBIT_BUTTON_EVT_CLICK)
    {
        
        generateAndShowDPK(1);
        uBit.sleep(1000);
        encrypt(1);
        uBit.display.scroll("R");
    }
    else if (e.value == MICROBIT_BUTTON_EVT_LONG_CLICK)
    {
        generateAndShowDPK(3);
        uBit.sleep(1000);
        encrypt(3);
        uBit.display.scroll("Y");
    }
}

void onButtonB(MicroBitEvent e)
{
    if (e.value == MICROBIT_BUTTON_EVT_CLICK)
    {
        generateAndShowDPK(2);
        uBit.sleep(1000);
        encrypt(2);
        uBit.display.scroll("G");
    }
}

int main()
{
    uBit.init();
    uBit.radio.enable();

    uBit.buttonA.setEventConfiguration(MICROBIT_BUTTON_ALL_EVENTS);
    uBit.buttonB.setEventConfiguration(MICROBIT_BUTTON_ALL_EVENTS);

    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, onButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, onButtonB);

    uBit.display.scroll("SENDER AES");
    uBit.display.print("Ready");

    while (1)
    {
        uBit.sleep(1000);
    }

    return 0; 

}


// ============================================
// FLOW:
// ============================================
// 1. Press Button A (click) -> Command 1 (Red)
// 2. Press Button A (long) -> Command 3 (Yellow)
// 3. Press Button B (click) -> Command 2 (Green)
//
// For each command:
//   1. Generate DPK = SHA256(secret) + MD5(salt)
//   2. Display DPK nibble (A-P)
//   3. Create plaintext: [command, 0x00, 0x00, ..., 0x00]
//   4. Encrypt with AES-128 ECB using DPK[0:16]
//   5. Send: [salt (1)] + [ciphertext (16)] = 17 bytes
//   6. Play distinctive tone (440/880/1320 Hz)
//   7. Display command letter (R/G/Y)
//
// Expected output at receiver:
//   1. Receiver shows SAME DPK nibble (proves key sync)
//   2. Receiver shows command letter
//   3. Receiver turns on corresponding LED
//   4. Receiver plays same tone
// ============================================