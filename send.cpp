#include "MicroBit.h" 

MicroBit uBit; 

int main()
{
	uBit.init(); 
	uBit.radio.enable(); 
	
	PacketBuffer b(1);
	
	while(1)
	{
		
	  if (uBit.buttonA.isPressed())
	      b[0] = 1;  

		  uBit.radio.datagram.send(b); 
		  uBit.sleep(1000);
	  
	 }
}

