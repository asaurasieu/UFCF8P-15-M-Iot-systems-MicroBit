#include "MicroBit.h"
#include "DPK.h"
#include "aes.h"

MicroBit uBit; 

#define r_freq 440
#define g_freq 880
#define y_freq 1320
#define duration 500 

#define RED_PIN uBit.io.P8 
#define GREEN_PIN uBit.io.P1 
#define Yell_PIN uBit.io.P2 



void verification(char letter)
{
    uBit.display.print(letter); 
    uBit.sleep(200); 
}



void play_tone(int frequency)
{
    
    MicroBitSoundEffect fx(
        frequency, 
        frequency, 
        duration, 
        255, 
        255, 
        Waveform::SINE 
    ); 

    uBit.sound.play(fx);
}

void play_melody(uint8_t cmd)
{
    if (cmd == 1)
    {
        play_tone(440); uBit.sleep(120); 
        play_tone(660); uBit.sleep(120); 
        play_tone(880); 
    }
    else if (cmd == 2)
    {
        play_tone(880); uBit.sleep(120); 
        play_tone(660); uBit.sleep(120); 
        play_tone(440); 
    }
    else if (cmd == 3)
    {
        play_tone(1320); uBit.sleep(120); 
        play_tone(990); uBit.sleep(120); 
        play_tone(660); 
    }
}

void turnOFFLEDs()
{
    RED_PIN.setDigitalValue(0); 
    GREEN_PIN.setDigitalValue(0); 
    Yell_PIN.setDigitalValue(0); 
}

void turnONLED(uint8_t cmd)
{
   if (cmd == 1) RED_PIN.setDigitalValue(1);
   if (cmd == 2) GREEN_PIN.setDigitalValue(1);
   if (cmd == 3) YEL_PIN.setDigitalValue(1);

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
    memcpy(ciphertext, p.getBytes() + 1, 16);

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
        play_tone(r_freq);
        play_melody(1);
    }
    else if (command == 2)
    {
        turnONLED(2);
        verification('G');
        play_tone(g_freq);
        play_melody(2);
    }
    else if (command == 3)
    {
        turnONLED(3);
        verification('Y');
        play_tone(y_freq);
        play_melody(3);
    }
    else
    {
        verification('F');
    }
}

int main()
{
    uBit.init(); 
    uBit.audio.enable(); 
    uBit.radio.setGroup(1); 
    uBit.audio.setVolume(255);

    uBit.radio.enable();
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
   

    turnOFFLEDs(); 

    while (1)
        uBit.sleep(1000);
   
    return 0; 
}

