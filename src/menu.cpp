#include "menu.h"
#include "LiquidCrystal.h"

const char menu_000[] = "[Main Menu]"; // 0
const char menu_001[] = "Config";        // 1
const char menu_002[] = "Calibration";   // 2

const char menu_100[] = "[Config parameter]"; // 3
const char menu_101[] = "X";                  // 4
const char menu_102[] = "Y";                  // 5                       
const char menu_103[] = "TOT";                // 6
const char menu_104[] = "return";             // 7  

const char menu_110[] = "[Config X]";         // 8
const char menu_111[] = "MIDI channel";       // 9
const char menu_112[] = "MIDI CC #";          // 10 
const char menu_113[] = "return";             // 11  

const char menu_120[] = "[Config Y]";         // 12
const char menu_121[] = "MIDI channel";       // 13
const char menu_122[] = "MIDI CC #";          // 14
const char menu_123[] = "return";             // 15  

const char menu_130[] = "[Config TOT]";       // 16
const char menu_131[] = "MIDI channel";       // 17
const char menu_132[] = "MIDI CC #";          // 18
const char menu_133[] = "return";             // 19 

unsigned char selected = 1;
extern LiquidCrystal lcd;

MenuEntry menu[] =
{
   {menu_000, 3, 0, 0, 0, 0}, // [Main Menu]]
   {menu_001, 3, 1, 2, 4, 0},
   {menu_002, 3, 1, 2, 2, 0},

   {menu_100, 5, 0, 0, 0, 0}, // [Config parameter]
   {menu_101, 5, 4, 5, 8, 0},
   {menu_102, 5, 4, 6, 11, 0},
   {menu_103, 5, 5, 7, 14, 0},
   {menu_104, 5, 6, 7, 1, 0},

   {menu_110, 4, 0, 0, 0, 0}, // [Config X]
   {menu_111, 4, 9, 10, 9, 0},                  
   {menu_112, 4, 9, 11, 10, 0},
   {menu_113, 4, 10, 11, 4, 0},

   {menu_120, 4, 0, 0, 0, 0}, // [Config Y]
   {menu_121, 4, 13, 14, 13, 0}, 
   {menu_122, 4, 13, 15, 14, 0},
   {menu_123, 4, 14, 15, 4, 0},

   {menu_130, 4, 0, 0, 0, 0},  // [Config TOT]
   {menu_131, 4, 17, 18, 17, 0},
   {menu_132, 4, 17, 19, 18, 0},
   {menu_133, 4, 18, 19, 4, 0},
};

void showMenu() {
   unsigned char from = 0;
   unsigned char to = 0;
   //unsigned char temp = 0;
   unsigned char line = 0;

   while (to <= selected) {
      to += menu[to].num_menupoints;
   }
   from = to - menu[selected].num_menupoints;
   --to;
   
   //temp = from;
   
   if ( (selected < (from + 2)) ) {
      to = from + 1;

      for (from; from<=to; ++from) {
         lcd.setCursor(1,line);
         lcd.print(menu[from].text);
         ++line;
      }
      lcd.setCursor(0,1);
      lcd.print("-");
   }
   else if ( (selected >= (from + 2)) ) {
      from = selected - 2;
      to = from + 1;

      for (from; from<=to; ++from) {
         lcd.setCursor(1,line);
         lcd.print(menu[from].text);
         ++line;
      }
      lcd.setCursor(0,1);
      lcd.print("-");
   }
   else {
      lcd.setCursor(0,0);
      lcd.print("ERR: Invalid");
      lcd.setCursor(0,1);
      lcd.print("selection!"); 
   }
}

void browseMenu() {
   

}
