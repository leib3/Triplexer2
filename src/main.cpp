#include "WProgram.h"
#include "menu.h"
#include "i2c_t3.h"

               // RS, E, D4, D5, D6, D7
//LiquidCrystal lcd(29, 31, 19, 17, 14, 33);
//static int * raw;
const int channel = 1;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5  

void ISRtest() {
   digitalWriteFast(13, LOW);
   delay(750);
   digitalWriteFast(13, HIGH);
   delay(750);
   digitalWriteFast(13, LOW);
   delay(750);
   digitalWriteFast(13, HIGH);

   return;
}

void init() {
   Serial.begin(9600);
   Serial.println("Hello from outside Arduino!");
	pinMode(13, OUTPUT);
   digitalWriteFast(13, HIGH);
   //lcd.begin(16,2);
  // attachInterrupt(A8, ISRtest, CHANGE);
  // usbMIDI.sendNoteOn(60, 99, channel);
  // usbMIDI.sendControlChange(5, 50, channel);
   //raw = (int *)malloc(sizeof(int));
}


extern "C" int main() 
{

#ifdef USING_MAKEFILE

	// To use Teensy 3.0 without Arduino, simply put your code here.
	// For example:
   init();
//code to do initial test of I2C interface to display
//startup sequence comes from Hitachi datasheet. Trying to set to 4 pin mode
//check voltage requirements. See page 24 in 21919 datasheet. Appears to require .8 Vdd (4 V) as min VIH. COuld try running at 3 volts with 5 v to backlight.
    delayMicroseconds(1000000);
    uint8_t dispadd = 0x00;  // 0x40? maybe this is wrong? Didn't get ACKs. Looks right, as long as A0, A1, A2 are 0 by default. Maybe reverse bit order?
    int numerrors = 0;
    delayMicroseconds(50000);
    Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 100000); //TODO try 400kHz here. Shouldn't matter, but would be faster
    Wire.setDefaultTimeout(200000); // 200ms
    Wire.beginTransmission(dispadd);
    Wire.write(0x98);  //TODO this number also might be wrong. Check bits in schematic, and desired input in Hitachi datasheet
    Wire.endTransmission();
    if(Wire.getError())
	numerrors++;
    delayMicroseconds(5000);
    Wire.beginTransmission(dispadd);
    Wire.write(0x98);
    Wire.endTransmission();
    if(Wire.getError())
	numerrors++;
    delayMicroseconds(150);
     Wire.beginTransmission(dispadd);
    Wire.write(0x98);
    Wire.endTransmission();
   if(Wire.getError())
	numerrors++;
   while (1) {
    Serial.print(numerrors);
      /*browseMenu();
      if (usbMIDI.read() != 0) {
         Serial.print("Rcvd type ");
         Serial.println(usbMIDI.getType());
         Serial.print("MIDI channel ");
         Serial.println(usbMIDI.getChannel());
      } */
   } 
}

#else 
	// Arduino's main() function just calls setup() and loop()....
	setup();
	while (1) {
		loop();
		yield();
	}
#endif


