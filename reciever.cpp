#include "MicroBit.h"

MicroBit uBit;

void onData(MicroBitEvent)
{
	PacketBuffer p = uBit.radio.datagram.recv(); 
	uint8_t command = p[0];
	
	if (command == 1)
	{
		uBit.io.P0.setLEDValue(1);
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
