#include "OSCMessage.h"
#include "OSCBundle.h"
#include "trip_osc.h"
#include "SLIPEncodedUSBSerial.h"
#include "SLIPEncodedSerial.h"
#include "WProgram.h"

/*
Make an OSC message and send it over serial
 */


//SLIPEncodedUSBSerial SLIPSerial;

SLIPEncodedUSBSerial SLIPSerial( Serial ); //global SLIP thing
//SLIPEncodedUSBSerial( thisBoardsSerialUSB );

// SlipEncodedUSBSerial test;

static OSCBundle globalBundle;


void oscadd1(char * target, int value){
   globalBundle.add(target).add((int32_t)value);
}

void oscsend(){
    SLIPSerial.beginPacket();
    globalBundle.send(SLIPSerial); // send the bytes to the SLIP stream
    SLIPSerial.endPacket();        // mark the end of the OSC Packet
    globalBundle.empty();          // empty the bundle to free room for a new one
}

void oscinit() {
  SLIPSerial.begin(9600);   // set this as high as you can reliably run on your platform
}


void oscsend1(unsigned short value){
  //the message wants an OSC address as first argument
  OSCMessage msg("/teensy/helloworld");
  msg.add(value);
  SLIPSerial.beginPacket();  
    msg.send(SLIPSerial); // send the bytes to the SLIP stream
  SLIPSerial.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message
}


void oscsend3(unsigned short x, unsigned short y, unsigned short t){
    OSCBundle myBundle;
    //OSCBundle's add' returns the OSCMessage so the message's 'add' can be composed together
    myBundle.add("/teensy/x").add((int32_t)x);
    myBundle.add("/teensy/y").add((int32_t)y);
    myBundle.add("/teensy/t").add((int32_t)t);

    SLIPSerial.beginPacket();
        myBundle.send(SLIPSerial); // send the bytes to the SLIP stream
    SLIPSerial.endPacket(); // mark the end of the OSC Packet
    myBundle.empty(); // empty the bundle to free room for a new one
}


void oscsend4(unsigned short ul, unsigned short ur, unsigned short ll, unsigned short lr){
    OSCBundle myBundle;
    //BOSCBundle's add' returns the OSCMessage so the message's 'add' can be composed together
    myBundle.add("/teensy/ul").add((int32_t)ul);
    myBundle.add("/teensy/ur").add((int32_t)ur);
    myBundle.add("/teensy/ll").add((int32_t)ll);
    myBundle.add("/teensy/lr").add((int32_t)lr);

    SLIPSerial.beginPacket();
        myBundle.send(SLIPSerial); // send the bytes to the SLIP stream
    SLIPSerial.endPacket(); // mark the end of the OSC Packet
    myBundle.empty(); // empty the bundle to free room for a new one
};

//Functions for receive OSC, and doing stuff with it

void osc_cc(OSCMessage &msg){
   int cc;
   char param;
   char buffer[16];
   cc=msg.getInt(0);
   Serial.print("got cc: ");
   Serial.println(cc);
   if(msg.getAddress((char*)buffer, 0)  > 9){
     Serial.print("for param: ");
     Serial.println(buffer[9]);
   }
}

void oscreceive(){
   OSCMessage inMessage;
   int size;
   while(!SLIPSerial.endofPacket())
    if( (size =SLIPSerial.available()) > 0)
    {
       while(size--)
          inMessage.fill(SLIPSerial.read());
     }
  
  if(!inMessage.hasError()){
   inMessage.dispatch("/teensy/xcc", osc_cc);
   inMessage.dispatch("/teensy/ycc", osc_cc);
   inMessage.dispatch("/teensy/tcc", osc_cc);

  }
}

void checkosc(){
   oscreceive();
}
