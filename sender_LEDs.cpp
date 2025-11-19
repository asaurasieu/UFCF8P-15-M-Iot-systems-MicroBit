#include "MicroBit.h" 

MicroBit uBit; 

PacketBuffer b(1); 

void onButtonA(MicroBitEvent e)
{
	if (e.value == MICROBIT_BUTTON_EVT_CLICK)
	{
		b[0] = 1; 
		uBit.radio.datagram.send(b); 
		uBit.display.print("R"); 
	}
	else if (e.value == MICROBIT_BUTTON_EVT_LONG_CLICK)
	{
		b[0] = 3; 
		uBit.radio.datagram.send(b); 
		uBit.display.print("Y"); 
	}
}

void onButtonB(MicroBitEvent e)
{
	if (e.value == MICROBIT_BUTTON_EVT_CLICK)
	{
		b[0] = 2; 
		uBit.radio.datagram.send(b); 
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
	
	uBit.display.scroll("Ready"); 
	uBit.display.print("Start"); 

	while(1)
	{
		uBit.sleep(1000); 
	}

}

