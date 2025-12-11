#include "MicroBit.h"
#include "DPK.h"
#include "aes.h"

MicroBit uBit; 

#define R_FREQ 440
#define G_FREQ 880
#define Y_FREQ 1320

// LED pin definitions
#define RED_LED uBit.io.P8 
#define GREEN_LED uBit.io.P1 
#define YELL_LED uBit.io.P2 

// Displays a verification letter on the MicroBit LED matrix
void verification(char letter, int duration = 200)
{
    uBit.display.print(letter); 
    uBit.sleep(duration); 
}



void play_tone(int freq, int duration)
{
    // Validate input parameters
    if(freq <= 0 || duration <= 0)
        return;

    // Calculates period in microseconds (1 second = 1,000,000 microseconds)
    int period = 1000000 / freq; 

    uBit.io.speaker.setAnalogPeriodUs(period);
    uBit.io.speaker.setAnalogValue(512);

    uBit.sleep(duration);

    uBit.io.speaker.setAnalogValue(0);
}

// Plays a distinct melody based on the received command
void play_melody(uint8_t cmd)
{
    if (cmd == 1)
    {
        play_tone(440, 150);
        play_tone(660, 150);
        play_tone(880, 200);
    }
    else if (cmd == 2)
    {
       play_tone(880, 150);
       play_tone(660, 150);
       play_tone(440, 200);
    }
    else if (cmd == 3)
    {
        play_tone(660, 150);
        play_tone(880, 150);
        play_tone(1320, 200);
    }
}

// Turns off all LEDs and stops the speaker
// Called before activating a new LED to ensure only one is on at a time
void turnOFFLEDs()
{
    RED_LED.setDigitalValue(0); 
    GREEN_LED.setDigitalValue(0); 
    YELL_LED.setDigitalValue(0); 

    uBit.io.speaker.setAnalogValue(0); 
}

// Turns on the LED corresponding to the command
void turnONLED(uint8_t cmd)
{
   if (cmd == 1) RED_LED.setDigitalValue(1);
   if (cmd == 2) GREEN_LED.setDigitalValue(1);
   if (cmd == 3) YELL_LED.setDigitalValue(1);

}

// Generates a 32-byte generated Private Key (DPK) from a the salt value
// Also extracts a nibble (4 bits) from the key for visual verification
void generate_key(uint8_t salt, uint8_t dpk[32], char* nibble)
{
    // Convert single byte salt to buffer format for makeKey function
    uint8_t salt_buf[1] = {salt}; 

    // Generate the 32-byte AES-256 key from the salt using the shared secret
    makeKey(salt_buf, 1, dpk);

    // This provides a visual check that key generation worked correctly
    *nibble = 'A' + (dpk[0] & 0x0F);
}

// Decrypts a 16-byte ciphertext using AES-256 ECB mode
// ciphertext: 16-byte encrypted data received from sender
// dpk: 32-byteGenerated Private Key (must match the key used for encryption)
// plaintext: Output buffer for the 16-byte decrypted result

void decrypt_command(uint8_t ciphertext[16], uint8_t dpk[32], uint8_t plaintext[16])
{
    // Initialize AES context with the 32-byte generated key
    struct AES_ctx ctx; 
    AES_init_ctx(&ctx, dpk); 
    
    // Copy ciphertext to plaintext buffer 
    memcpy(plaintext, ciphertext, 16); 
    
    // Decrypt using AES-256 ECB mode
    AES_ECB_decrypt(&ctx, plaintext);
}


// Event handler called when a radio message is received
// Packet format: [salt (1 byte)] + [ciphertext (16 bytes)] = 17 bytes total
void onData(MicroBitEvent)
{
    // Show received: scroll "RECEIVED" message
    uBit.display.scroll("RECEIVED");
    uBit.sleep(1000);

    uint8_t salt = p[0]; 
    uint8_t ciphertext[16]; 
    memcpy(ciphertext, p.getBytes() + 1, 16);

    // Get the 32-byte AES-256 key using the same salt and shared secret as sender
    uint8_t dpk[32];
    char nibble;
    generate_key(salt, dpk, &nibble);

    // Show key generated: scroll "KEY" message
    uBit.display.scroll("KEY");
    uBit.sleep(1000);
    
    // Show the nibble letter (A-P) for verification that both devices generated the same key
    verification(nibble, 1000);

    // Decrypt the ciphertext to get the original command
    uint8_t plaintext[16]; 
    decrypt_command(ciphertext, dpk, plaintext);

    // Show decrypted: scroll "DECRYPT" message
    uBit.display.scroll("DECRYPT");
    uBit.sleep(1000);

    // Extract command from first byte of plaintext
    uint8_t command = plaintext[0];

    turnOFFLEDs(); 
    
    int toneRed    = 600;
    int toneGreen  = 900;
    int toneYellow = 1200;

    if (command == 1)
    {
        turnONLED(1);
        verification('R');
        play_tone(toneRed, 150);
        play_melody(1);
    }
    else if (command == 2)
    {
        turnONLED(2);
        verification('G');
        play_tone(toneGreen, 150);
        play_melody(2);
    }
    else if (command == 3)
    {
        turnONLED(3);
        verification('Y');  
        play_tone(toneYellow, 150);
        play_melody(3); 
    }
    else
    {
        verification('F');  
        uBit.display.print("OK");
        uBit.sleep(300); 
    }
}

int main()
{
    uBit.init(); 
    
    uBit.radio.enable();
    uBit.radio.setGroup(1); 
   
    // Ensures all LEDs and speaker are off at startup
    turnOFFLEDs(); 

    
    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);


    while (1)
        uBit.sleep(1000);
   
    return 0; 
}

