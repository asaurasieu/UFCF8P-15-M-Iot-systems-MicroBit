#include "MicroBit.h"
#include "DPK.h"

MicroBit uBit; 

void turnOFFLEDs()
{
    uBit.io.P0.setDigitalValue(0); 
    uBit.io.P1.setDigitalValue(0); 
    uBit.io.P2.setDigitalValue(0); 
}

void onData(MicroBitEvent)
{
    PacketBuffer p = uBit.radio.datagram.recv(); 
    uint8_t command = p[0]; 

    // Generate the DPK using the recieved command as salt 
    uint8_t salt[1] = {command}; 
    uint8_t key[32]; 
    makeKey(salt, 1, key);

    // Display the first nibble of the dpk for testing 
    char nibbleSymbol = 'A' + (key[0] & 0x0F); 
    uBit.display.print(nibbleSymbol); 


    turnOFFLEDs(); 

    if (command == 1)
    {
        uBit.io.P0.setDigitalValue(1);
        uBit.display.print("R");
    }
    else if (command == 2)
    {
        uBit.io.P1.setDigitalValue(1);
        uBit.display.print("G");
    }
    else if (command == 3)
    {
        uBit.io.P2.setDigitalValue(1);
        uBit.display.print("Y");
    }

}

int main()
{
    uBit.init(); 

    uBit.display.scroll("RECEIVER DPK"); 
    uBit.sleep(500); 

    uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
    uBit.radio.enable();

    turnOFFLEDs(); 

    while (1)
        uBit.sleep(1000);

}

// Instructions 

// When the sender transmits: 

/*

1. RECEIVES SALT (1,3,2)
2. COPUTES SAME DPK 
3. SHOWS THE SAME LETTER 
4. DISPLAYS THE COLOR LETTER 
5. TURNS ON CORRESPONDING LED 

*/