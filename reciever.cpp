#include "MicroBit.h"

MicroBit uBit;

void onData(MicroBitEvent e)
{
	ManagedString s = uBit.radio.datagram.recv();
	
	if (s == "1")
	{
		uBit.io.P0.setServoValue(0);
		uBit.display.print("A");
	}
	
	if (s == "2")
	{
		uBit.io.P0.setServoVAlue(180);
		uBit.display.print("B");
	}
}

int main()
{
	uBit.init();
	uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
	uBit.radio.enable();
	
	while(1)
		uBit.sleep(1000);
}
