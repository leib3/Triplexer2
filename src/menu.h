#include "stdint.h"
typedef const struct MenuStruct
{  const char * text;
   unsigned char num_menupoints;
   unsigned char up;
   unsigned char down;
   unsigned char enter;
   unsigned char back;
   void ( *fp ) ( void );
} MenuEntry;

#define ENTER 0 // digital pins for menu
#define UP 1
#define DOWN 6
#define BACK 3
// modes defines for ControlModeAndSettings class member mode
#define MIDIUSB 0
#define MIDIUART 1
#define OSC 2

// Create global struct or pointer to global struct 
// containing current MIDI channel #s and CC #s for each 
// control parameter, plus other relevant status info for
// debugging purposes. There will be a single instance of this
// struct that is shared by both the menu and the load cell
// input processing module (TODO)

// each parameter (MIDI/USB, MIDI/UART, TOT) should be able to be assigned
// to MIDI/USB, MIDI/UART, or OSC. For now, OSC and MIDI/USB should not
// be in use at the same time. 
// how does the user change the mode assigned to each parameter, and at what
// time is this checked for validity?
//
// TODO: how to disable a parameter?
// TODO: parameter status menu screen OR persistent symbol in corner of LCD
// referring to which modes are active
class ControlModeAndSettings {
   public:
      uint8_t mode;
      void update(uint8_t option, uint8_t value)
      {
         settings[option] = value; // menu.cpp already takes care of ensuring all MIDI settings
         // fall in valid range
      }
      uint8_t read(uint8_t option) // read current menu option value
      {
         return settings[option];
      }
   private:
      std::map<uint8_t, uint8_t> settings; // maybe rename
};

class param { // making this barebones class allows us to refer to each parameter directly by name
   ControlModeAndSettings active;
};
class tpxSettings {
   public:
      param X,Y,TOT;

      bool isValid(uint8_t XMode, uint8_t YMode, uint8_t TOTMode) // check if valid combination of modes is assigned
      // to parameters (X, Y, TOT)
      // most likely done after user hits enter on mode selection screen
      {

      }
      // splitting mode setting into three separate, nearly identical functions is not elegant,
      // but it will work for now. What prevents us from using a single function to do this 
      // is the lack of a clear way to refer to the parameter whose mode we want to set, as an
      // argument to this function. 
      void setXMode(uint8_t modeToSet)
      {
         // test for valid combination of modes with prospective mode change
         if (isValid(modeToSet, Y.active.mode, TOT.active.mode)) {
            X.active.mode = modeToSet;
         }
         // rely on isValid() to print problem to Serial. Eventually, lcd prints problem
         return;
      }
      void setYMode(uint8_t modeToSet)
      {
         if (isValid(X.active.mode, modeToSet, TOT.active.mode)) {
            Y.active.mode = modeToSet;
         }
         return;
      }
      void setTOTMode(uint8_t modeToSet) 
      {
         if (isValid(X.active.mode, Y.active.mode, modeToSet)) {
            TOT.active.mode = modeToSet;
         }
         return;
      }
};

struct settingsStruct
{
   uint8_t xCCNum;
   uint8_t xChannelNum;
   uint8_t yCCNum;
   uint8_t yChannelNum;
   uint8_t totCCNum;
   uint8_t totChannelNum;
   bool xInv;
   bool yInv;
   bool totInv;
};

// initialize settingsStruct with default MIDI settings
// declared static to avoid multiple definition linker error
//static settingsStruct tpxSettings = {
//        .xCCNum = 1, 
//        .xChannelNum = 1, 
//        .yCCNum = 1, 
//        .yChannelNum = 1,
//        .totCCNum = 1, 
//        .totChannelNum = 1,
//        .xInv = false, 
//        .yInv = false,
//        .totInv = false
//   };

void browseMenu();

void showMenu();

void debounceInit();
