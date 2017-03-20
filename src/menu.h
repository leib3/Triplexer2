#include "stdint.h"
#include <map>
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
// see defines above... MIDIUSB = 0, MIDIUART = 1, OSC = 2
//
// trying to think ahead here: MIDIUSB/MIDIUART have the same settings. When switching
// a parameter from one of these two to the other, should we clear the hash table? I want to say no.
// If we switch from either MIDIUSB or MIDIUART to OSC, we shouldn't have to erase it IFF
// OSC options are stored at different indices from the MIDI options.
// How much control do we give the user wrt OSC here?
// Couldn't there be any number of possible options, given the open nature of OSC?
class ControlModeAndSettings {
   public:
      uint8_t mode; // 0, 1, or 2
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
      std::map<uint8_t, uint8_t> settings; // each parameter gets its own hash table, which is how
      // user options are stored internally
};

class param { // making this barebones class allows us to refer to each parameter directly by name
   ControlModeAndSettings active;
};
class tpxSettings {
   public:

      bool isValid(uint8_t XMode, uint8_t YMode, uint8_t TOTMode) // check if valid combination of modes is assigned
      // to parameters (X, Y, TOT)
      // most likely done after user hits enter on mode selection screen
      {
         uint8_t temp[3] = {XMode, YMode, TOTMode};
         uint8_t OSCflag = 0;
         uint8_t MIDIUSBflag = 0;
         for (int i = 0; i < 3; ++i) {
            if (temp[i] == MIDIUSB) {
               MIDIUSBflag = 1;
            }
            else if (temp[i] == OSC) {
               OSCflag = 1;
            }
            else continue;
         }
         if (MIDIUSBflag && OSCflag) return false;
         else return true;
      }

      // These functions must report to the LCD menu logic if a mode combination is invalid. 
      // Try catch?
      uint8_t setXMode(uint8_t modeToSet)
      {
         // test for valid combination of modes with prospective mode change
         if (isValid(modeToSet, Y.active.mode, TOT.active.mode)) {
            X.active.mode = modeToSet;
            return 1;
         }
         else {
            Serial.println("invalid mode combination. Only one protocol that uses USB can be
                  active at a time");
            return -1;
         }
         // rely on isValid() to print problem to Serial. Eventually, lcd prints problem
      }
      uint8_t setYMode(uint8_t modeToSet)
      {
         if (isValid(X.active.mode, modeToSet, TOT.active.mode)) {
            Y.active.mode = modeToSet;
            return 1;
         }
         else {
            Serial.println("invalid mode combination. Only one protocol that uses USB can be
                  active at a time");
            return -1;
         }
      }
      uint8_t setTOTMode(uint8_t modeToSet) 
      {
         if (isValid(X.active.mode, Y.active.mode, modeToSet)) {
            TOT.active.mode = modeToSet;
            return 1;
         }
         else {
            Serial.println("invalid mode combination. Only one protocol that uses USB can be
                  active at a time");
           return -1;
         }
      }
      // unfortunately if we want to make a generic interface that
      // the web front-end can work with, we cannot use "selected" from
      // the menu logic to infer which parameter we are configuring.
      // this means we need three separate functions to update individual
      // options -- one for each parameter
      // For error handling purposes, our very simple LCD screen/buttons
      // doesn't allow the user to input invalid values for ex: a CC number.
      // Problem is the web front-end will. LUCKILY, IF WE'RE GOING TO DO IT RIGHT,
      // error checking can happen on the front-end itself!! Not in our program here!
      uint8_t updateXOptions(uint8_t option, uint8_t value) 
      {
        X.active.update(option, value);

        return 1; // need to add error handling
      }
      uint8_t updateYOptions(uint8_t option, uint8_t value) 
      {
        Y.active.update(option, value);

        return 1; // need to add error handling
      }
      uint8_t updateTOTOptions(uint8_t option, uint8_t value)
      {
        TOT.active.update(option, value);

        return 1; // need to add error handling
      }

    private:
      param X,Y,TOT;
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
