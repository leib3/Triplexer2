#include "WProgram.h"
#include "LiquidCrystal.h"
               // RS, E, D4, D5, D6, D7

extern LiquidCrystal lcd(29, 31, 19, 17, 14, 33);
static int * raw;
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
   lcd.begin(16, 2);
   lcd.setCursor(0, 0);
   lcd.print("hello, world!");
   raw = (int *)malloc(sizeof(int));
}

int read_buttons(int * raw) {
   (*raw) = analogRead(A8);
   if (((*raw) > 950) && ((*raw) < 1150)) return btnNONE;
   if ((*raw) < 30*4) return btnRIGHT;
   if ((*raw) < 120*4) return btnUP;
   if ((*raw) < 180*4) return btnDOWN;
   if ((*raw) < 210*4) return btnLEFT;
   if ((*raw) < 240*4) return btnSELECT;
}

extern "C" int main() 
{

#ifdef USING_MAKEFILE

	// To use Teensy 3.0 without Arduino, simply put your code here.
	// For example:
   init();
   while (1) {
      Serial.print(".");
      lcd_key = read_buttons(raw);
      lcd.setCursor(9,1); // syntax: (x, y)
      lcd.print(*raw);
      lcd.setCursor(0,1);
      
      switch (lcd_key) {
         
         case btnRIGHT: {
            lcd.print("RIGHT ");
            break;
         }
         case btnNONE: {
            lcd.print("NONE ");
            break;
         }
         case btnUP: {
            lcd.print("UP  ");
            break;
         }
         case btnDOWN: {
            lcd.print("DOWN  ");
            break;
         }
         case btnSELECT: {
            lcd.print("SELECT");
            break;
         }
      }
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


