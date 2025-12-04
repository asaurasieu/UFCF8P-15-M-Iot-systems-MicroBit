#include "MicroBit.h"
#include "DPK.h"
#include "aes.h"

MicroBit uBit; 

#define r_freq 440
#define g_freq 880
#define y_freq 1320
#define duration 500 



void verification(char letter)
{
    uBit.display.print(letter); 
    uBit.sleep(200); 
}



void play_tone(uint8_t command)
{
    if (command == 1){
        uBit.speaker.playTone(r_freq, duration);
    } else if (command == 2){
        uBit.speaker.playTone(g_freq, duration);
    } else if (command == 3){
        uBit.speaker.playTone(y_freq, duration);
    }
}

void turnOFFLEDs()
{
    uBit.io.P0.setDigitalValue(0); 
    uBit.io.P1.setDigitalValue(0); 
    uBit.io.P2.setDigitalValue(0); 
}

void turnONLED(uint8_t command)
{
    if (command == 1){
        uBit.io.P0.setDigitalValue(1);
    } else if (command == 2){
        uBit.io.P1.setDigitalValue(1);
    } else if (command == 3){
        uBit.io.P2.setDigitalValue(1);
    }
}

void generate_key(uint8_t salt_value, uint8_t key[32], char* nibble)
{
    uint8_t salt_buf[1] = {salt_value}; 
    makeKey(salt_buf, 1, key);
    *nibble = 'A' + (key[0] & 0x0F);
}

void decrypt_command(uint8_t ciphertext[16], uint8_t key[32], uint8_t plaintext[16])
{
    struct AES_ctx ctx; 
    AES_init_ctx(&ctx, key); 
    memcpy(plaintext, ciphertext, 16); 
    AES_ECB_decrypt(&ctx, plaintext);
}


void onData(MicroBitEvent)
{
    // Recieve encrypted message salt (1) + ciphertext(16)
    PacketBuffer p = uBit.radio.datagram.recv(); 
    verification('R');

    uint8_t salt = p[0]; 
    uint8_t ciphertext[16]; 

    for (int i = 0; i < 16; i++){
        ciphertext[i] = p[i + 1]; 
    }

    uint8_t key[32];
    char nibble;
    generate_key(salt, key, &nibble);
    verification(nibble);

    verification('D');
    uint8_t plaintext[16]; 
    decrypt_command(ciphertext, key, plaintext);

    uint8_t command = plaintext[0];

    turnOFFLEDs(); 
    if (command == 1)
    {
        turnONLED(1);
        verification('R');
        play_tone(1);
    }
    else if (command == 2)
    {
        turnONLED(2);
        verification('G');
        play_tone(2); 
    }
    else if (command == 3)
    {
        turnONLED(3);
        verification('Y');
        play_tone(3); 
    }
    else
    {
        verification('F');
    }
}

int main()
{
    uBit.init(); 

    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();

    turnOFFLEDs(); 

    while (1)
        uBit.sleep(1000);
   
    return 0; 
}

// ============================================
// FLOW:
// ============================================
// 1. Receives encrypted message: [salt (1)] + [ciphertext (16)]
// 2. Extracts salt from first byte
// 3. Regenerates DPK using same salt (same as sender)
// 4. Displays DPK nibble (A-P) - should match sender display!
// 5. Initializes AES context with regenerated DPK
// 6. Decrypts ciphertext to recover plaintext
// 7. Extracts command from plaintext[0]
// 8. Executes command:
//    - Command 1: Turn on LED P0, display "R", play 440 Hz tone
//    - Command 2: Turn on LED P1, display "G", play 880 Hz tone
//    - Command 3: Turn on LED P2, display "Y", play 1320 Hz tone
//
// Security verification:
//   1. DPK nibbles match sender -> Proves both have same key
//   2. Message decrypts correctly -> Proves encryption is working
//   3. Command executes -> Proves authentication succeeded
// ============================================