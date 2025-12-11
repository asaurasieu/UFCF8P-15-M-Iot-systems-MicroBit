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

// Encrypts the command value using AES-256 ECB mode
// commandValue: The command to encrypt (1=RED, 2=GREEN, 3=YELLOW)
// ciphertext: Output buffer for the 16-byte encrypted result
// dpk: 32-byte Generated Private Key used for encryption
void encrypt_command(uint8_t commandValue, uint8_t ciphertext[16], uint8_t dpk[32])
{
    uint8_t plaintext[16]; 
    memset(plaintext, 0, 16); 
    plaintext[0] = commandValue; 

    // Initialize AES context with the 32-byte generated key
    struct AES_ctx ctx; 
    AES_init_ctx(&ctx, dpk); 

    // Encrypt plaintext using AES-256 ECB mode (16-byte block)
    memcpy(ciphertext, plaintext, 16); 
    AES_ECB_encrypt(&ctx, ciphertext); 
}

// Sends encrypted message over radio
// Packet format: [salt (1 byte)] + [ciphertext (16 bytes)] = 17 bytes total
// salt: Random salt value (needed by receiver to derive the same key)
// ciphertext: 16-byte AES-256 encrypted command
void send_message(uint8_t salt, uint8_t ciphertext[16])
{
    PacketBuffer b(17); 
    b[0] = salt;  // First byte is the salt 

    // Copy 16-byte ciphertext into bytes 1-16 of the packet
    for (int i = 0; i < 16; i++){
        b[i + 1] = ciphertext[i]; 
   }

   // DEBUG Sent
    uBit.radio.datagram.send(b); 
}

// Main encryption function: generates the salt, derives the key, encrypts the command, and sends it
void encrypt(uint8_t commandValue)
{
    // Generates a random 1-byte salt (0-255) for the message 
    // Each message uses a unique salt to ensure unique encryption keys
    uint8_t salt = (uint8_t)uBit.random(256);

    // Generate the 32-byte AES-256 key from the salt
    uint8_t dpk[32];
    char nibble;
    generate_key(salt, dpk, &nibble);
    
    // Show key generated: scroll "KEY" message
    uBit.display.scroll("KEY");
    uBit.sleep(1000);
    
    // Show the nibble letter (A-P) for verification that both devices generated the same key
    verification(nibble);
    uBit.sleep(1000);
    
    // Encrypt the command using AES-256
    uint8_t ciphertext[16];
    encrypt_command(commandValue, ciphertext, dpk);

    // Show encrypted: scroll "ENCRYPT" message
    uBit.display.scroll("ENCRYPT");
    uBit.sleep(1000);

    // Send the salt (plaintext) and ciphertext over radio
    send_message(salt, ciphertext);
    
    // Show sent: scroll "SEND" message
    uBit.display.scroll("SEND");
    uBit.sleep(1000);
}

// Event handler for Button A
// Short press: Send RED command (1)
// Long press: Send YELLOW command (3)
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

// Event handler for Button B
// Short press: Send GREEN command (2)
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

    // Configure buttons to trigger on all events (click, long click, etc.)
    uBit.buttonA.setEventConfiguration(MICROBIT_BUTTON_ALL_EVENTS);
    uBit.buttonB.setEventConfiguration(MICROBIT_BUTTON_ALL_EVENTS);

    // Register event handlers for button presses
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, onButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_EVT_ANY, onButtonB);

    while (1)
    {
        uBit.sleep(1000);
    }

    return 0; 
}


