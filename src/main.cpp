#include "WProgram.h"
#include "menu.h"
#include "ADC.h"
#include "IntervalTimer.h"
#include "LiquidCrystal.h"
#include "trip_osc.h"
#include "trip_adc.h"



//static int * raw;
const int channel = 1;

//display macros
               // RS, E, D4, D5, D6, D7
LiquidCrystal lcd(29, 31, 19, 17, 14, 33);

//DFRkeypad button defines
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5  


void  init() {
   Serial.begin(9600);
   pinMode(13, OUTPUT);
   //pinMode(0, INPUT);
   digitalWriteFast(13, HIGH);
   // commented out menu stuff for now. Put back in with Zach whenever we meet
   pinMode(PIN_ENTER, INPUT_PULLUP); // menu buttons
   pinMode(PIN_UP, INPUT_PULLUP);
   pinMode(PIN_DOWN, INPUT_PULLUP);
   pinMode(PIN_BACK, INPUT_PULLUP);
   pinMode(PIN_SAVE, INPUT_PULLUP);
   
   lcd.begin(16,2);

   showMenu();
}

extern "C" int main() 
{
   init();
  // delayMicroseconds(5000000);
  // adcinit();
  // oscinit();
   debounceInit();
  // timerinit(); //put this last before while loop if possible. Don't want to start adc timer before we're ready for it.
  // adcCalibrate();
   while (1) {
      browseMenu();
      /*if (usbMIDI.read() != 0) {
         Serial.print("Rcvd type ");
         Serial.println(usbMIDI.getType());
         Serial.print("MIDI channel ");
         Serial.println(usbMIDI.getChannel());
      } 
      */
   checkosc();
   } 
}




