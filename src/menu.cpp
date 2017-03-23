#include "menu.h"
#include "settings.h"
#include "LiquidCrystal.h"
#include "DFRkeypad.h"
#include "Bounce2.h"
/* TODO: Symbol or character to represent currently active mode FOR EACH PARAMETER 
 * in corner of screen.
 * ADDITIONALLY: ability to DISABLE any control parameters
 * FINALLY: perhaps if we can't fit a symbol or character in the corner of the menu,
 * a STATUS menu that shows all currently active parameters and the modes assigned to them
 */
extern LiquidCrystal lcd; // lcd object is declared and initialized in main.cpp
unsigned char selected = 1;
unsigned char xCfg = 9; // menu initially thinks all parameters are in MIDI mode. May want to change later
unsigned char yCfg = 17; // these are just initializations
unsigned char totCfg = 25;
tpxSettings * Settings = new tpxSettings; // how do I share this instance with the future web front-end?
// so far I'm relying on the compiler to generate constructors for all classes in settings.h. This may need to change.

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

const char menu_205[] = "[Config X OSC]";     // 13
const char menu_206[] = "OPTION #1";          // 14
const char menu_207[] = "OPTION #2";          // 15

const char menu_210[] = "[Config Y]";         // 16
const char menu_211[] = "MIDI channel";       // 17
const char menu_212[] = "MIDI CC #";          // 18
const char menu_213[] = "INV Y";              // 19
const char menu_214[] = "MODE";               // 20

const char menu_215[] = "[Config Y OSC]";     // 21
const char menu_216[] = "OSC OPTION #1";      // 22
const char menu_217[] = "OSC OPTION #2";      // 23

const char menu_220[] = "[Config TOT]";       // 24
const char menu_221[] = "MIDI channel";       // 25
const char menu_222[] = "MIDI CC #";          // 26
const char menu_223[] = "INV TOT";            // 27
const char menu_224[] = "MODE";               // 28

const char menu_225[] = "[Config TOT OSC]";   // 29
const char menu_226[] = "OSC OPTION #1";      // 30
const char menu_227[] = "OSC OPTION #2";      // 31

//const char menu_300[] = "[X MIDI CHNL #]";    // 19
//const char menu_301[] = "[X CC #]";           // 20

// we can infer which parameter is being configured based on 
// the value of 'selected'. 
//const char* DFRkeypad::sKEY[]=                          { "---",       "Right",   "Up", "Down", "Left", "Select", "???" };
//
static Bounce back = Bounce();
static Bounce up = Bounce();
static Bounce down = Bounce();
static Bounce enter = Bounce();

void debounceInit() {

   back.attach(BACK);
   back.interval(20);

   up.attach(UP);
   up.interval(20);

   down.attach(DOWN);
   down.interval(20);
   enter.attach(ENTER);
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
        case 9:
           param = 'X';
           CCnum = Settings->getParamSetting('X', 1);
           break;
        case 13:
           param = 'Y';
           CCnum = Settings->getParamSetting('Y', 1);
           break;
        case 17:
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
   }
   while(keypress != ENTER); // loop until user hits enter
   switch (param) {
      case 'X':
         if (Settings->setXOption(1, CCnum) != 1) {
            Serial.println("Error setting X CC num");
         }
         break;
      case 'Y':
         if (Settings->setYOption(1, CCnum) != 1) {
            Serial.println("Error setting Y CC num");
         }
         break;
      case 'T':
         if (Settings->setTOTOption(1, CCnum) != 1) {
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
     case 8:
       param = 'X';
       CHnum = Settings->getParamSetting('X', 0);
       break;
     case 12:
       param = 'Y';
       CHnum = Settings->getParamSetting('Y', 0);
       break;
     case 16:
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
  while(keypress != ENTER); // loop until user hits enter

   switch (param) {
      case 'X':
         if (Settings->setXOption(0, CHnum) != 1) {
            Serial.println("Error setting X channel num");
         }
         break;
      case 'Y':
         if (Settings->setYOption(0, CHnum) != 1) {
            Serial.println("Error setting Y channel num");
         }
         break;
      case 'T':
         if (Settings->setTOTOption(0, CHnum) != 1) {
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
     case 10:
          param = 'X';
          inv = Settings->getParamSetting('X', 2);
          break;
     case 14:
          param = 'Y';
          inv = Settings->getParamSetting('Y', 2);
          break;
     case 18:
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
  while (keypress != 1); // loop till user hits enter
   
   switch (param) {
      case 'X':
         if (Settings->setXOption(2, inv) != 1) {
            Serial.println("Error setting X channel num");
         }
         break;
      case 'Y':
         if (Settings->setYOption(2, inv) != 1) {
            Serial.println("Error setting Y channel num");
         }
         break;
      case 'T':
         if (Settings->setTOTOption(2, inv) != 1) {
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

void configMODE()
{

}

MenuEntry menu[] =
{
   {menu_000, 4, 0, 0, 0, 0, 0}, // [Main Menu]   0
   {menu_001, 4, 1, 2, 4, 1, 0},                // 1
   {menu_002, 4, 1, 2, 2, 1, 0},                // 2
   {menu_003, 4, 2, 3, 3, 1, 0}, // status      // 3
                                                  
   {menu_100, 4, 0, 0, 0, 0, 0}, // [Config param] // 4
   {menu_101, 4, 5, 6, xCfg, 1, 0},                // 5
   {menu_102, 4, 5, 6, yCfg, 1, 0},                // 6 
   {menu_103, 4, 5, 6, totCfg, 1, 0},              // 7
                                                  
   {menu_200, 5, 0, 0, 0, 0, 0}, // [Config X]     // 8
   {menu_201, 5, 9, 9,  8,  5, configMIDIChannel}, // 9           
   {menu_202, 5, 9, 10, 9,  5, configMIDICC},      // 10
   {menu_203, 5, 10, 11, 10, 5, configINV},         // 11
   {menu_204, 5, 11, 12, 12, 5, configMODE},        // 12

   {menu_205, 3, 0, 0, 0, 0, 0}, // [Config X OSC]    // 13
   {menu_206, 3, 14, 15, 14, 0},                      // 14
   {menu_207, 3, 14, 15, 15, 0},                      // 15
                                                 
   {menu_210, 5, 0, 0, 0, 0, 0}, // [Config Y]        // 16
   {menu_211, 5, 17, 18, 17, 5, configMIDIChannel},   // 17
   {menu_212, 5, 17, 19, 18, 5, configMIDICC},        // 18
   {menu_213, 5, 18, 20, 19, 5, configINV},           // 19
   {menu_214, 5, 19, 20, 20, 5, configMODE},          // 20

   {menu_215, 3, 0, 0, 0, 0, 0}, // [Config Y OSC]    // 21
   {menu_216, 3, 22, 23, 22, 5, 0},                   // 22
   {menu_217, 3, 22, 23, 23, 5, 0},                   // 23
                                                
   {menu_220, 5, 0, 0, 0, 0, 0},  // [Config TOT]     // 24
   {menu_221, 5, 25, 26, 25, 5, configMIDIChannel},   // 25
   {menu_222, 5, 25, 27, 26, 5, configMIDICC},        // 26
   {menu_223, 5, 26, 28, 27, 5, configINV},           // 27
   {menu_224, 5, 27, 28, 28, 5, 0},                   // 28

   {menu_225, 3, 0, 0, 0, 0, 0}, // [Config TOT OSC]  // 29
   {menu_226, 3, 30, 31, 30, 5, 0},                   // 30
   {menu_227, 3, 30, 31, 31, 5, 0}                    // 31
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
   delay(20);
}

void menuCfg() {
   if (Settings->getParamMode('X')) {
      xCfg = 14;
   }
   else {
      xCfg = 9;
   }
   if (Settings->getParamMode('Y')) {
      yCfg = 22;
   }
   else {
      yCfg = 17;
   }
   if (Settings->getParamMode('T')) {
      totCfg = 30;
   }
   else {
      totCfg = 25;
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

