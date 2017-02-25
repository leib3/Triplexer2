#include <OSCMessage.h>
#include <SLIPEncodedUSBSerial.h>
#include <SLIPEncodedUSBSerial.h>

/*
Make an OSC message and send it over serial
 */



void oscinit();
void oscsend1(unsigned short value); //sends 16 bit unsigned value
void oscsend4(unsigned short ul, unsigned short ur, unsigned short ll, unsigned short lr);
