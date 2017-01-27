#include "menu.h"
#include "LiquidCrystal.h"
#include "DFRkeypad.h"

unsigned char selected = 1;
extern LiquidCrystal lcd; // lcd object is declared in main.cpp

const char menu_000[] = "[Main Menu]";        // 0
const char menu_001[] = "Config";             // 1
const char menu_002[] = "Calibration";        // 2

const char menu_100[] = "[Config parameter]"; // 3
const char menu_101[] = "X";                  // 4
const char menu_102[] = "Y";                  // 5                       
const char menu_103[] = "TOT";                // 6

const char menu_200[] = "[Config X]";         // 7
const char menu_201[] = "MIDI channel";       // 8
const char menu_202[] = "MIDI CC #";          // 9 
const char menu_203[] = "INV X";              // 10

const char menu_210[] = "[Config Y]";         // 11
const char menu_211[] = "MIDI channel";       // 12
const char menu_212[] = "MIDI CC #";          // 13
const char menu_213[] = "INV Y";              // 14

const char menu_220[] = "[Config TOT]";       // 15
const char menu_221[] = "MIDI channel";       // 16
const char menu_222[] = "MIDI CC #";          // 17
const char menu_223[] = "INV TOT";            // 18

const char menu_300[] = "[X MIDI CHNL #]";    // 19
const char menu_301[] = "[X CC #]";           // 20

// These functions will be used to configure the MIDI channel and CC #s.
// I really want to separate what the user sees (view) from what's
// going on under the hood (MIDI CC stuff) but I'm not sure how 
// to do this. 
//
// When we get the load cells working their input will be the 
// MIDI CC value output from the weight processing module (to be written)
// we can infer which parameter is being configured based on 
// the value of 'selected'. 
//const char* DFRkeypad::sKEY[]=                          { "---",       "Right",   "Up", "Down", "Left", "Select", "???" };
void configMIDICC() {
   static char param;
   switch (selected) {
        case 9:
                param = 'X';
                break;
        case 13:
                param = 'Y';
                break;
        case 17:
                param = 'T';
                break;
   }
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("[MIDI CC #]");
   lcd.setCursor(14,0);
   lcd.print(param);
   while(DFRkeypad::GetKey() != 1);
   // loop until user hits enter
   return;
}

void configMIDIChannel() {
  static char param;
  uint8_t * midiChPtr;
  switch (selected) {
     case 8:
             param = 'X';
             midiChPtr = &tpxSettings.xChannelNum;
             break;
     case 12:
             param = 'Y';
             midiChPtr = &tpxSettings.yChannelNum;
             break;
     case 16:
             param = 'T';
             midiChPtr = &tpxSettings.totChannelNum;
             break;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("[MIDI CHNL #]");
  lcd.setCursor(14,0);
  lcd.print(param);
  lcd.setCursor(1,1);
  lcd.print(*midiChPtr);
  while(DFRkeypad::GetKey() != 1); // loop until user hits enter
   // placeholder
  return;
}

MenuEntry menu[] =
{
   {menu_000, 3, 0, 0, 0, 0, 0}, // [Main Menu]]   0
   {menu_001, 3, 1, 2, 4, 1, 0},                // 1
   {menu_002, 3, 1, 2, 2, 1, 0},                // 2
                                                  
   {menu_100, 4, 0, 0, 0, 0, 0}, // [Config param] 3
   {menu_101, 4, 4, 5, 8, 1, 0},                // 4
   {menu_102, 4, 4, 6, 12, 1, 0},               // 5 
   {menu_103, 4, 5, 6, 16, 1, 0},               // 6
                                                  
   {menu_200, 4, 0, 0, 0, 0, 0}, // [Config X]     7
   {menu_201, 4, 8, 9, 8, 4, configMIDIChannel},           // 8         
   {menu_202, 4, 8, 10, 9, 4, configMIDICC},               // 9
   {menu_203, 4, 9, 10, 10, 4, 0},              // 10
                                                 
   {menu_210, 4, 0, 0, 0, 0, 0}, // [Config Y]  // 11
   {menu_211, 4, 12, 13, 12, 5, configMIDIChannel},             // 12
   {menu_212, 4, 12, 14, 13, 5, configMIDICC},             // 13
   {menu_213, 4, 13, 14, 14, 5, 0},             // 14
                                                 
   {menu_220, 4, 0, 0, 0, 0, 0},  // [Config TOT// 15
   {menu_221, 4, 16, 17, 16, 6, configMIDIChannel},             // 16
   {menu_222, 4, 16, 18, 17, 6, configMIDICC},             // 17
   {menu_223, 4, 17, 18, 18, 6, 0}             // 18
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
   
   //temp = from; temp is used for screens with more than two rows
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
   if (!strcmp(buffer, DFRkeypad::sKEY[4])) { // back (actually LEFT KEY)
      selected = menu[selected].back;
   }
   if (!strcmp(buffer, DFRkeypad::sKEY[1])) { // enter (actually RIGHT KEY)
      if (menu[selected].fp != 0 ) {
         menu[selected].fp();
      }
      selected = menu[selected].enter;
   }
}

