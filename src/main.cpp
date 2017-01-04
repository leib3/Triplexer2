#include "WProgram.h"
#include "LiquidCrystal.h"
#include "menu.h"
               // RS, E, D4, D5, D6, D7
LiquidCrystal lcd(29, 31, 19, 17, 14, 33);
//static int * raw;
int lcd_key = 0;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5  

void init() {
   Serial.begin(9600);
   Serial.println("Hello from outside Arduino!");
	pinMode(13, OUTPUT);
   digitalWriteFast(13, HIGH);
   lcd.begin(16,2);
   //raw = (int *)malloc(sizeof(int));
}


extern "C" int main() 
{

#ifdef USING_MAKEFILE

	// To use Teensy 3.0 without Arduino, simply put your code here.
	// For example:
   init();
   while (1) {
      browseMenu();
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


