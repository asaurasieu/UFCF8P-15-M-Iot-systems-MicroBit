#include "MicroBit.h"

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

	// Turn off all LEDS so that only one should keep on at a time 
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

	uBit.display.scroll("CHOOSE COLOR"); 
	uBit.sleep(500);

	// Display button choices 
	uBit.display.scroll("A=R B=G AA=Y"); 

	// Message listener for radio communication 
	uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
	uBit.radio.enable();

	// Turn off all LEDS 
	turnOFFLEDs(); 
	
	while(1)
		uBit.sleep(1000);
}
