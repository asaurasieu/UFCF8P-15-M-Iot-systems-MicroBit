#include "MicroBit.h"
#include "DPK.h"
#include "aes.h"

MicroBit uBit; 


void verification(char letter)
{
    uBit.display.print(letter); 
    uBit.sleep(200); 
}


void generate_key(uint8_t salt, uint8_t dpk[32], char* nibble)
{
    uint8_t salt_buf[1] = {salt}; 
    makeKey(salt_buf, 1, dpk); 
    *nibble = 'A' + (dpk[0] & 0x0F); 
}

void encrypt_command(uint8_t commandValue, uint8_t ciphertext[16], uint8_t dpk[32])
{
    uint8_t plaintext[16]; 
    memset(plaintext, 0, 16); 
    plaintext[0] = commandValue; 


    // Initialize AES context with dpk 
    struct AES_ctx ctx; 
    AES_init_ctx(&ctx, dpk); 

    // Encrypt plaintext using AES-128 ECB mode 
    memcpy(ciphertext, plaintext, 16); 
    AES_ECB_encrypt(&ctx, ciphertext); 
}

void send_message(uint8_t salt, uint8_t ciphertext[16])
{
    PacketBuffer b(17); 
    b[0] = salt;  // first byte is the salt

    for (int i = 0; i < 16; i++){
        b[i + 1] = ciphertext[i]; 
   }

   // DEBUG Sent
    uBit.radio.datagram.send(b); 
}

void encrypt(uint8_t commandValue)
{
    uint8_t salt = (uint8_t)uBit.random(256);

    uint8_t dpk[32];
    char nibble;
    generate_key(salt, dpk, &nibble);
    verification(nibble);
    uBit.sleep(1000);

    verification('E');  // Encryption // remove 
    uBit.sleep(1000);
    uint8_t ciphertext[16];
    encrypt_command(commandValue, ciphertext, dpk);

    verification('T');  // Transmit
    uBit.sleep(1000);
    send_message(salt, ciphertext); // send the salt and the ciphertext
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
    uBit.seedRandom(uBit.random(0x7fffffff));
    uBit.radio.enable();
    uBit.radio.setGroup(1); 

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


// Add more visualization to the commands so that it shows when it was recieved, encrypted and decrypted. 


