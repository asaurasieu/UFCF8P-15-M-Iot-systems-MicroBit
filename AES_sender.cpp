#include "MicroBit.h"
#include "DPK.h"
#include "aes.h"

MicroBit uBit; 


void verification(char letter)
{
    uBit.display.print(letter); 
    uBit.sleep(200); 
}


void generate_key(uint8_t commandValue, uint8_t dpk[32], char* nibble)
{
    uint8_t salt[1] = {commandValue}; 
    makekey(salt, 1, dpk); 
    *nibble = 'A' + (dpk[0] & 0x0F); 
}

void encrypt_command(uint8_t commmandValue, uint8_t ciphertext[16], uint8_t dpk[32])
{
    uint8_t plaintext[16]; 
    memset(plaintext, 0, 16); 
    plaintext[0] = commandValue; 


    // Initialize AES context with dpk 
    struct AES_ctx ctx; 
    AES_init_ctx(&ctx, dpk); 

    // Encrypt plaintext using AES-128 ECB mode 
    uint8_t ciphertext[16]; 
    memcpy(ciphertext, plaintext, 16); 
    AES_ECB_encrypt(&ctx, ciphertext); 
}

void send_message(uint8_t commmandValue, uint8_t ciphertext[16])
{
    PacketBuffer b(17); 
    b[0] = salt[0]; 

    for (int i = 0; i < 16; i++){
        b[i + 1] = ciphertext[i]; 
   }

   // DEBUG Sent
    uBit.radio.datagram.send(b); 
}

void encrypt(uint8_t commandValue)
{
    uint8_t dpk[32];
    char nibble;
    generate_key(commandValue, dpk, &nibble);
    display_letter(nibble);

    display_letter('E');  // Encryption
    uint8_t ciphertext[16];
    encrypt_command(commandValue, ciphertext, dpk);

    display_letter('T');  // Transmit
    send_message(commandValue, ciphertext);
}

void onButtonA(MicroBitEvent e)
{
    if (e.value == MICROBIT_BUTTON_EVT_CLICK)
    {
        
        uBit.sleep(1000);
        encrypt(1);
        uBit.display.print("R");
    }
    else if (e.value == MICROBIT_BUTTON_EVT_LONG_CLICK)
    {
        uBit.sleep(1000);
        encrypt(3);
        uBit.display.print("Y");
    }
}

void onButtonB(MicroBitEvent e)
{
    if (e.value == MICROBIT_BUTTON_EVT_CLICK)
    {
        uBit.sleep(1000);
        encrypt(2);
        uBit.display.print("G");
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