#include "menu.h"
#include "settings.h"
#include "LiquidCrystal.h"
#include "DFRkeypad.h"
#include "Bounce2.h"
#include <map>

//#include <string>
/* TODO: 
 * ability to DISABLE any control parameters
 * a STATUS menu that lists mode, status (enabled/disabled), and settings for all three parameters
 */
extern LiquidCrystal lcd; // lcd object is declared and initialized in main.cpp
unsigned char selected = 1;
unsigned char xCfg = 9; // menu initially thinks all parameters are in MIDI mode. May want to change later
unsigned char yCfg = 19; // these are just initializations
unsigned char totCfg = 29;
tpxSettings * Settings = new tpxSettings; // how do I share this instance with the future web front-end?
const char * modemap[3] = {"MIDI/USB","MIDI/UART","OSC"};

const char menu_000[] = "[Main Menu]";        // 0
const char menu_001[] = "Config";             // 1
const char menu_002[] = "Calibration";        // 2
const char menu_003[] = "Status";             // 3

const char menu_100[] = "[Config parameter]"; // 4
const char menu_101[] = "X";                  // 5
const char menu_102[] = "Y";                  // 6                       
const char menu_103[] = "TOT";                // 7

const char menu_200[] = "[Config X]";         // 8
const char menu_201[] = "MIDI channel";       // 9
const char menu_202[] = "MIDI CC #";          // 10
const char menu_203[] = "INV X";              // 11
const char menu_204[] = "MODE";               // 12
const char menu_209[] = "EN/DISABLE X";       // 13

const char menu_205[] = "[Config X OSC]";     // 14
const char menu_206[] = "OPTION #1";          // 15
const char menu_207[] = "MODE";               // 16
//toggle                                              // 17

const char menu_210[] = "[Config Y]";         // 18
const char menu_211[] = "MIDI channel";       // 19
const char menu_212[] = "MIDI CC #";          // 20
const char menu_213[] = "INV Y";              // 21
const char menu_214[] = "MODE";               // 22
const char menu_218[] = "EN/DISABLE Y";       // 23

const char menu_215[] = "[Config Y OSC]";     // 24
const char menu_216[] = "OSC OPTION #1";      // 25
const char menu_217[] = "MODE";               // 26
                                              // 27

const char menu_220[] = "[Config TOT]";       // 28
const char menu_221[] = "MIDI channel";       // 29
const char menu_222[] = "MIDI CC #";          // 30
const char menu_223[] = "INV TOT";            // 31
const char menu_224[] = "MODE";               // 32
const char menu_228[] = "EN/DISABLE TOT";     // 33

const char menu_225[] = "[Config TOT OSC]";   // 34
const char menu_226[] = "OSC OPTION #1";      // 35
const char menu_227[] = "MODE";               // 36
                                              // 37

//const char menu_997[] = "MIDI/UART";          // 32
//const char menu_998[] = "MIDI/USB";           // 33
//const char menu_999[] = "OSC";                // 34
//const char menu_110[] = "[X enabled]";        // 35
//const char menu_111[] = "[X disabled]";       // 36

// we can deduce which parameter is being configured based on 
// the current value of 'selected'. 
//const char* DFRkeypad::sKEY[]=                          { "---",       "Right",   "Up", "Down", "Left", "Select", "???" };
//
static Bounce back = Bounce();
static Bounce up = Bounce();
static Bounce down = Bounce();
static Bounce enter = Bounce();

void debounceInit() {

   back.attach(PIN_BACK);
   back.interval(20);

   up.attach(PIN_UP);
   up.interval(20);

   down.attach(PIN_DOWN);
   down.interval(20);

   enter.attach(PIN_ENTER);
   enter.interval(20); 
   return;
}

int getButtonPress() {
   enter.update();
   up.update();
   down.update();
   back.update();

   if (enter.fell()){
      return ENTER;
   }
   else if (up.fell()) {
      return UP;
   }
   else if (down.fell()) {
      return DOWN;
   }
   else if (back.fell()) {
      return BACK;

   }
   else return -1; // no button press detected
}
void configMIDICC() {
   char param;
   uint8_t CCnum;
   int keypress;
   switch (selected) {
        case 10:
           param = 'X';
           CCnum = Settings->getParamSetting('X', 1);
           break;
        case 20:
           param = 'Y';
           CCnum = Settings->getParamSetting('Y', 1);
           break;
        case 30:
           param = 'T';
           CCnum = Settings->getParamSetting('T', 1);
           break;
   }
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("[MIDI CC #]");
   lcd.setCursor(14,0);
   lcd.print(param);
   do {
      keypress = getButtonPress();
      lcd.setCursor(1,1);
      lcd.print(CCnum);
      if (keypress == UP) {
         if (CCnum == 127) { 
            CCnum = 1;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("[MIDI CC #]");
            lcd.setCursor(14,0);
            lcd.print(param);
         }
         else {
            ++CCnum;
         }
      }
      else if (keypress == DOWN) { 
         if (CCnum == 1) {
            CCnum = 127;
         }
         else if (CCnum == 100 || CCnum == 10) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("[MIDI CC #]");
            lcd.setCursor(14, 0);
            lcd.print(param);
            --CCnum;
         }
         else {
            --CCnum;
         }
      }
   } while (keypress != ENTER && keypress != BACK); // loop till user hits enter or back

   if (keypress == BACK) return;

   switch (param) {
      case 'X':
         if (Settings->setParamOption('X', 1, CCnum) != 1) {
            Serial.println("Error setting X CC num");
         }
         break;
      case 'Y':
         if (Settings->setParamOption('Y', 1, CCnum) != 1) {
            Serial.println("Error setting Y CC num");
         }
         break;
      case 'T':
         if (Settings->setParamOption('T', 1, CCnum) != 1) {
            Serial.println("Error setting TOT CC num");
         }
         break;
   }
   lcd.setCursor(1,1);
   lcd.print("CC # ");
   lcd.print(CCnum);
   lcd.print(" OK");
   delay(1000); // allow user time to see confirmation 
   // of midi CC number
   return;
}

void configMIDIChannel() {
  char param;
  uint8_t CHnum;
  int keypress;
  switch (selected) {
     case 9:
       param = 'X';
       CHnum = Settings->getParamSetting('X', 0);
       break;
     case 19:
       param = 'Y';
       CHnum = Settings->getParamSetting('Y', 0);
       break;
     case 29:
       param = 'T';
       CHnum = Settings->getParamSetting('T', 0);
       break;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("[MIDI CHNL #]");
  lcd.setCursor(14,0);
  lcd.print(param);
  do {
     keypress = getButtonPress();
     lcd.setCursor(1,1);
     lcd.print(CHnum);
     if (keypress == UP) { // up
   // accounting for when *CHnum goes from
   // two digits to one digit in length -- second
   // digit persists on LCD unless we redraw the
   // screen entirely. Not elegant, but works for 
   // now
        if (CHnum == 16) {
           CHnum = 1;
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("[MIDI CHNL #]");
           lcd.setCursor(14,0);
           lcd.print(param);
        } 
        else {
          ++CHnum;
        }
     }
     else if (keypress == DOWN) { // down
       if (CHnum == 1) {
         CHnum = 16;
       }
       else if (CHnum == 10) {
           lcd.clear();
           lcd.setCursor(0,0);
           lcd.print("[MIDI CHNL #]");
           lcd.setCursor(14,0);
           lcd.print(param);
           --CHnum;
       }
       else {
         --CHnum;
       }
     }
  }
  while (keypress != ENTER && keypress != BACK); // loop till user hits enter or back
  if (keypress == BACK) return;

   switch (param) {
      case 'X':
         if (Settings->setParamOption('X', 0, CHnum) != 1) {
            Serial.println("Error setting X channel num");
         }
         break;
      case 'Y':
         if (Settings->setParamOption('Y', 0, CHnum) != 1) {
            Serial.println("Error setting Y channel num");
         }
         break;
      case 'T':
         if (Settings->setParamOption('T', 0, CHnum) != 1) {
            Serial.println("Error setting TOT channel num");
         }
         break;
   }
  lcd.setCursor(1,1);
  lcd.print("Channel ");
  lcd.print(CHnum);
  lcd.print(" OK");
  delay(1000); // allow user time to see confirmation
 // of MIDI channel 
  return;
}

void configINV()
{
  uint8_t inv;
  char param;
  int keypress;
  switch (selected) {
     case 11:
          param = 'X';
          inv = Settings->getParamSetting('X', 2);
          break;
     case 21:
          param = 'Y';
          inv = Settings->getParamSetting('Y', 2);
          break;
     case 31:
          param = 'T';
          inv = Settings->getParamSetting('T', 2);
          break;
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("[INVERT PARAM]");
  lcd.setCursor(15,0);
  lcd.print(param);
  do {
    keypress = getButtonPress();
    lcd.setCursor(1,1);
    if (inv == 0) {
       lcd.print("NORMAL");
    }
    else {
      lcd.print("INVERT");
    }
    if ((keypress == UP || keypress == DOWN) && (inv == 0)) {
      inv = 1;
    }
    else if ((keypress == UP || keypress == DOWN) && (inv == 1)) {
      inv = 0;
    }
  }
  while (keypress != ENTER && keypress != BACK); // loop till user hits enter or back
   if (keypress == BACK) return;

   switch (param) {
      case 'X':
         if (Settings->setParamOption('X', 2, inv) != 1) {
            Serial.println("Error setting X channel num");
         }
         break;
      case 'Y':
         if (Settings->setParamOption('Y', 2, inv) != 1) {
            Serial.println("Error setting Y channel num");
         }
         break;
      case 'T':
         if (Settings->setParamOption('T', 2, inv) != 1) {
            Serial.println("Error setting TOT channel num");
         }
         break;
   }
  lcd.setCursor(1,1);
  lcd.print(param);
  lcd.print(" ");
  if (inv == 0) {
    lcd.print("NORMAL");
  }
  else {
    lcd.print("INVERT");
  }
  lcd.print(" OK");
  delay(1000); // give user time to see confirmation of invert parameter setting

  return;
}

void configMode()
{
  uint8_t mode;
  char param;
  int keypress;
  if (selected == 12 || selected == 16) {
       param = 'X';
       mode = Settings->getParamMode(param);
       Serial.println("X mode is ");
       Serial.print(mode);
  }
  else if (selected == 22 || selected == 26) {
       param = 'Y';
       mode = Settings->getParamMode(param);
       Serial.println("Y mode is ");
       Serial.print(mode);
  }
  else if (selected == 32 || selected == 36) {
       param = 'T';
       mode = Settings->getParamMode(param);
       Serial.print(mode);
       Serial.println("T mode is ");
  }
  else {
     lcd.setCursor(0,0);
     lcd.print("ERROR: invalid");
     return;
  }
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("[SET MODE FOR]");
  lcd.setCursor(15,0);
  lcd.print(param);
  do {
    keypress = getButtonPress();
    lcd.setCursor(1,1);
    lcd.print(modemap[(int)mode]);

   if (keypress == UP) {
      if (mode == 2) { 
         mode = 0;
      }
      else {
         ++mode;
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("[SET MODE FOR]");
      lcd.setCursor(15,0);
      lcd.print(param);
   }
   else if (keypress == DOWN) { 
      if (mode == 0) {
         mode = 2;
      }
      else {
         --mode;
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("[SET MODE FOR]");
      lcd.setCursor(15,0);
      lcd.print(param);
   }
   else continue;
  }
  while (keypress != ENTER && keypress != BACK); // loop till user hits enter or back
  if (keypress == BACK) return;

   switch (param) {
      case 'X':
         if (Settings->setParamMode('X', mode) != 1) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("USB in use");
            lcd.setCursor(0,1);
            lcd.print("Resetting X");
            delay(2000);
            return;
         }
         break;
      case 'Y':
         if (Settings->setParamMode('Y', mode) != 1) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("USB in use");
            lcd.setCursor(0,1);
            lcd.print("Resetting Y");
            delay(2000);
            return;
         }
         break;
      case 'T':
         if (Settings->setParamMode('T', mode) != 1) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("USB in use");
            lcd.setCursor(0,1);
            lcd.print("Resetting TOT");
            delay(2000);
            return;
         }
         break;
   }
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print(param);
   lcd.print(" mode ");
   lcd.print(modemap[(int)mode]);
   lcd.setCursor(1,1);
   lcd.print(" OK");
   delay(1000);
} 
void toggleOnOff() {
  char param;
  bool on;
  int keypress;
  if (selected == 13 || selected == 17) {
       param = 'X';
       on = Settings->isParamEnabled(param);
  }
  else if (selected == 23 || selected == 27) {
       param = 'Y';
       on = Settings->isParamEnabled(param);
  }
  else if (selected == 33 || selected == 37) {
       param = 'T';
       on = Settings->isParamEnabled(param);
  }
  else {
     lcd.setCursor(0,0);
     lcd.print("ERROR: invalid");
     return;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("[EN/DISABLE] ");
  lcd.setCursor(15,0);
  lcd.print(param);
  do {
    keypress = getButtonPress();
    lcd.setCursor(1,1);
    if (on == 0) {
       lcd.print("DISABLED");
    }
    else {
      lcd.print("*ENABLED");
    }
    if ((keypress == UP || keypress == DOWN) && (on == false)) {
      on = true;
    }
    else if ((keypress == UP || keypress == DOWN) && (on == true)) {
      on = false;
    }
  }
  while (keypress != ENTER && keypress != BACK); // loop till user hits enter or back
   if (keypress == BACK) return;

  if (Settings->enOrDisableParam(param, on) != 1) {
     lcd.setCursor(1,1);
     lcd.print("ERROR");
     return;
  }

  lcd.setCursor(1,1);
  lcd.print(param);
  lcd.print(" ");
  if (on == false) {
    lcd.print("DISABLED");
  }
  else {
    lcd.print("ENABLED");
  }
  lcd.print(" OK");
  delay(1000); // give user time to see confirmation of invert parameter setting
}
//void showStatus()
//{
//
//}

MenuEntry menu[] =
{
   {menu_000, 4, 0, 0, 0, 0, 0}, // [Main Menu]   0
   {menu_001, 4, 1, 2, 5, 1, 0},                // 1
   {menu_002, 4, 1, 3, 2, 1, 0},                // 2
   {menu_003, 4, 2, 3, 3, 1, 0},                // 3
                                                  
   {menu_100, 4, 0, 0, 0, 0, 0}, // [Config param] // 4
   {menu_101, 4, 5, 6, xCfg, 1, 0},                // 5
   {menu_102, 4, 5, 7, yCfg, 1, 0},                // 6 
   {menu_103, 4, 6, 7, totCfg, 1, 0},              // 7
                                                  
   {menu_200, 6, 0, 0, 0, 0, 0}, // [Config X]        // 8
   {menu_201, 6, 9, 10,  9,  5, configMIDIChannel},   // 9           
   {menu_202, 6, 9, 11, 10,  5, configMIDICC},        // 10
   {menu_203, 6, 10, 12, 11, 5, configINV},           // 11
   {menu_204, 6, 11, 13, 12, 5, configMode},          // 12
   {menu_209, 6, 12, 13, 13, 5, toggleOnOff},         // 13

   {menu_205, 4, 0, 0, 0, 0, 0}, // [Config X OSC]    // 14
   {menu_206, 4, 15, 16, 15, 5, 0},                   // 15
   {menu_207, 4, 15, 17, 16, 5, configMode},          // 16
   {menu_209, 4, 16, 17, 17, 5, toggleOnOff},         // 17
                                                 
   {menu_210, 6, 0, 0, 0, 0, 0}, // [Config Y]        // 18
   {menu_211, 6, 19, 20, 19, 5, configMIDIChannel},   // 19
   {menu_212, 6, 19, 21, 20, 5, configMIDICC},        // 20
   {menu_213, 6, 20, 22, 21, 5, configINV},           // 21
   {menu_214, 6, 21, 23, 22, 5, configMode},          // 22
   {menu_218, 6, 22, 23, 23, 5, toggleOnOff},         // 23

   {menu_215, 4, 0, 0, 0, 0, 0}, // [Config Y OSC]    // 24
   {menu_216, 4, 25, 26, 25, 5, 0},                   // 25
   {menu_217, 4, 25, 27, 26, 5, configMode},          // 26
   {menu_218, 4, 26, 27, 27, 5, toggleOnOff},         // 27
                                                
   {menu_220, 6, 0, 0, 0, 0, 0},  // [Config TOT]     // 28
   {menu_221, 6, 29, 30, 29, 5, configMIDIChannel},   // 29
   {menu_222, 6, 29, 31, 30, 5, configMIDICC},        // 30
   {menu_223, 6, 30, 32, 31, 5, configINV},           // 31
   {menu_224, 6, 31, 33, 32, 5, configMode},          // 32
   {menu_228, 6, 32, 33, 33, 5, toggleOnOff},         // 33

   {menu_225, 4, 0, 0, 0, 0, 0}, // [Config TOT OSC]  // 34
   {menu_226, 4, 35, 36, 35, 5, 0},                   // 35
   {menu_227, 4, 35, 37, 36, 5, configMode},          // 36
   {menu_228, 4, 36, 37, 37, 5, toggleOnOff},         // 37
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
   lcd.clear();
   
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
   delay(50);
}

void menuCfg() {
   if (Settings->getParamMode('X') == OSC) {
      menu[5].enter = 15;
   }
   else {
      menu[5].enter = 9;
   }
   if (Settings->getParamMode('Y') == OSC) {
      menu[6].enter = 25;
   }
   else {
      menu[6].enter = 19;
   }
   if (Settings->getParamMode('T') == OSC) {
      menu[7].enter = 35;
   }
   else {
      menu[7].enter = 29;
   }
}


void browseMenu() {
   menuCfg();

#ifdef DFRKEYPAD
   static char buffer[15];
   uint8_t key = DFRkeypad::GetKey();
   strcpy(buffer, DFRkeypad::KeyName(key));

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
   if (!strcmp(buffer, DFRkeypad::sKEY[0])) {
      return;
   }
   else {
      showMenu(); // only redraw menu if a keypress is detected
  }
#endif // DFRKEYPAD
//   Serial.print("*  key: ");
//   Serial.print(analogRead(A8));
//   Serial.print(" ");
//   Serial.println(buffer);
   int keypress = getButtonPress();

   if (keypress == BACK) {
      Serial.println("detected something from back button");
         selected = menu[selected].back;
   }
   if (keypress == UP) {
      Serial.println("detected something from up button");
         selected = menu[selected].up;
   }
   if (keypress == DOWN) {
      Serial.println("detected something from down button");
         selected = menu[selected].down;
   }
   if (keypress == ENTER) {
      Serial.println("detected something from enter button");
         if (menu[selected].fp != 0 ) {
            menu[selected].fp(); // fp takes care of drawing menu
         }  // may want to change this in the future
         selected = menu[selected].enter;
   }
   if (keypress != -1) { // only redraw menu if keypress is detected
      showMenu();
   }
}

