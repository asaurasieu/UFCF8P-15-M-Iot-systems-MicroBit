#include "MicroBit.h"

MicroBit uBit:

//simple statistics 
int totalPackets = 0;
int suspiciousPackets = 0;
int floodEvents = 0;

//for flood detection (packets per 1-second window)
int packetsInWindow = 0;
uint64_t windowStartMs = 0;

//tune these to whatever you like 
const int EXPECTED_LEN = 17;         //salt(1) + ciphertext(16)
const int FLOOD_THRESHOLD = 10;      //packets per second

void ShowNormal()
{
  //Small dot to show normal packet seen
  uBit.display.print(".");
  uBit.sleep(80);
  uBit.display.clear();
}

void showSuspicious()
{
  //flash '!' to indicate anomaly
  for (int i = 0; i < 2; i++)
    {
      uBit.display.print("!");
      uBit.sleep(150);
      uBit.display.clear();
      uBit.sleep(100);
    }
}

void showFloodAlert()
{
  //stronger Flood alert pattern
  for (int i = 0; i < 3; i++)
    {
      uBit.display.print("F");
      uBit.sleep(200);
      uBit.display.clear();
      uBit.sleep(100);
    }
}

//called whenever a radio datagram arrives
void onData(MicroBitEvent)
{
  PacketBuffer p = uBit.radio.datagram.recv();
  int len = p.length();
  totalPackets++;

  //sliding 1-second window for flood detection --
  uint64_t now = system_timer_current_time();   //ms since boot

  if (windowstartMs == 0)
  {
    windowStartMs = now;
  }


if (now - windowStartMs >= 1000)
{
  //New 1-sec window
  packetsInWindow = 0;
  windowStartMs = now;
}

packetsInWindow++;

bool isSuspicious = flase;

//1) Length anomaly 
if (len != EXPECTED_LEN)
{
  isSuspicious = true:
    suspiciousPackets++;
}

//2) flood anomaly 
if (packetsInWindow > FLOOD_THRESHOLD)
{
  isSuspicious = true;
  floodEvents++;
  showFloodAlert();
}

// If not flood, but suspicious length to smaller alert
if (isSuspicious && packetsInWindow <= FLOOD_THRESHOLD)
{
  showSuspicious();
}

else if (!isSuspicious)
{
  showNormal();
}

// Optional: log basic info over serial to PC
// can open a serial terminal to view this

uBit.serial.printf("pkt=%d len=%d sus=%d flood=%d\r\n",
  totalPackets, len, suspiciousPackets, floodEvents);
}

int main()
{
    uBit.init();

    // Enable radio and join SAME group as your AES sender/receiver
    uBit.radio.enable();
    uBit.radio.setGroup(1);

    uBit.display.scroll("IDS");

    // Listen for all datagrams
    uBit.messageBus.listen(MICROBIT_ID_RADIO,
                           MICROBIT_RADIO_EVT_DATAGRAM,
                           onData);

    // Keep running forever
    while (1)
    {
        uBit.sleep(1000);
    }

    return 0;
}
