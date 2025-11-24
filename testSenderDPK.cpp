#include "MicroBit.h"
#include "DPK.h"

MicroBit uBit; 

PacketBuffer b(1); 

void generateAndShowDPK(uint8_t commandValue)
{

    uint8_t salt[1] = { commandValue };
    uint8_t key[32];

    makeKey(salt, 1, key);

    char nibbleSymbol = 'A' + (key[0] & 0x0F);
    uBit.display.print(nibbleSymbol);

}

void onButtonA(MicroBitEvent e)
{
    if (e.value == MICROBIT_BUTTON_EVT_CLICK)
    {
        b[0] = 1;

        generateAndShowDPK(1);

        uBit.sleep(1000);

        uBit.radio.datagram.send(b);
        uBit.display.scroll("R");
    }
    else if (e.value == MICROBIT_BUTTON_EVT_LONG_CLICK)
    {
        b[0] = 3;

        generateAndShowDPK(3);

        uBit.sleep(1000);

        uBit.radio.datagram.send(b);
        uBit.display.scroll("Y");
    }
}

void onButtonB(MicroBitEvent e)
{
    if (e.value == MICROBIT_BUTTON_EVT_CLICK)
    {
        b[0] = 2;

        generateAndShowDPK(2);
       
        uBit.sleep(1000);

        uBit.radio.datagram.send(b);
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

    uBit.display.scroll("SENDER DPK");
    uBit.display.print("Ready");

    while (1)
    {
        uBit.sleep(1000);
    }

}


// Instructions 

// When you press: 
/* 
1. A click -> command 1 -> salt = {1}
2. A long click -> command 3 -> salt = {3}
3. B click -> command 2 -> salt = {2}

The sender will compute DPK = SHA256(secret) + MD5(salt)
Display a letter A-P (nibble of key[0])
Send the command to the reciever 
Then display the command letter corresponding to the color (R,G,Y)

*/

/*

The outout expected is: 

1. Receiver should show the SAME letter A-P 
2. This provides both devices that come from the same DPK 
3. Once matching -> We can start to implement the AES encryption 

*/