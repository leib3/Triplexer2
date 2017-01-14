#include "menu.h"
#include "LiquidCrystal.h"
#include "DFRkeypad.h"

const char menu_000[] = "[Main Menu]";        // 0
const char menu_001[] = "Config";             // 1
const char menu_002[] = "Calibration";        // 2

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
extern LiquidCrystal lcd; // lcd object is declared in main.cpp

void testMe() {
   lcd.clear();
   lcd.print("It works!!!");

   while(DFRkeypad::GetKey() != 5); // loop until user hits enter

   return;
}

MenuEntry menu[] =
{
   {menu_000, 3, 0, 0, 0, 0, 0}, // [Main Menu]]// 0
   {menu_001, 3, 1, 2, 4, 1, 0},                // 1
   {menu_002, 3, 1, 2, 2, 1, 0},                // 2
                                                  
   {menu_100, 4, 0, 0, 0, 0, 0}, // [Config param] // 3
   {menu_101, 4, 4, 5, 8, 1, 0},                // 4
   {menu_102, 4, 4, 6, 11, 1, 0},               // 5 
   {menu_103, 4, 5, 6, 14, 1, 0},               // 6
                                                  
   {menu_110, 3, 0, 0, 0, 0, 0}, // [Config X]     // 7
   {menu_111, 3, 8, 9, 8, 4, testMe},           // 8         
   {menu_112, 3, 8, 9, 9, 4, 0},              // 9
                                                  
   {menu_120, 3, 0, 0, 0, 0, 0}, // [Config Y]  // 10
   {menu_121, 3, 11, 12, 11, 5, 0},             // 11
   {menu_122, 3, 11, 12, 12, 5, 0},             // 12
                                                  
   {menu_130, 3, 0, 0, 0, 0, 0},  // [Config TOT// 13]
   {menu_131, 3, 14, 15, 14, 6, 0},             // 14
   {menu_132, 3, 14, 15, 15, 6, 0},             // 15
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
   lcd.clear(); // the LCD being cleared and redrawn on each
   // iteration of the loop is responsible for the flicker. 
   // I need to do this in a more intelligent way.
   
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
      from = selected - 1;
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
   delay(20);
}

void browseMenu() {
   
   showMenu();
   static char buffer[15];

   uint8_t key = DFRkeypad::GetKey();
   strcpy(buffer, DFRkeypad::KeyName(key));
   Serial.print("*  key: ");
   Serial.println(buffer);

   if (!strcmp(buffer, DFRkeypad::sKEY[2])) { // up
     selected = menu[selected].up;
   } 
   if (!strcmp(buffer, DFRkeypad::sKEY[3])) { // down
      selected = menu[selected].down;
   }
   if (!strcmp(buffer, DFRkeypad::sKEY[5])) { // back (actually LEFT KEY)
      selected = menu[selected].back;
   }
   if (!strcmp(buffer, DFRkeypad::sKEY[1])) { // enter (actually RIGHT KEY)
      if (menu[selected].fp != 0 ) {
         menu[selected].fp();
      }
      selected = menu[selected].enter;
   }
}

