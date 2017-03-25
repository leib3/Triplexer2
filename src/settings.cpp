#include "settings.h"
#include "WProgram.h"

// modes defines for param class member mode

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
   void param::update(uint8_t option, uint8_t value)
   {
      settings[option] = value; // menu.cpp already takes care of ensuring all MIDI settings
      // fall in valid range
   }
   uint8_t param::read(uint8_t option) // read current menu option value
   {
      return settings[option];
   }


   bool tpxSettings::isValid(uint8_t XMode, uint8_t YMode, uint8_t TOTMode) // check if valid combination of modes is assigned
      // to parameters (X, Y, TOT)
      // most likely done after user hits enter on mode selection screen
   {
      uint8_t temp[3] = {XMode, YMode, TOTMode};
      uint8_t USBcount = 0;
      for (int i = 0; i < 3; ++i) {
         if (temp[i] == MIDIUSB || temp[i] == OSC) {
            ++USBcount;
         }
         else continue;
      }
      if (USBcount > 1) return false;
      else return true;
   }

      // These functions must report to the LCD menu logic if a mode combination is invalid. 
      // Try catch?
   uint8_t tpxSettings::setXMode(uint8_t modeToSet)
   {
      // test for valid combination of modes with prospective mode change
      if (isValid(modeToSet, Y.mode, TOT.mode)) {
         X.mode = modeToSet;
         return 1;
      }
      else {
         Serial.println("invalid mode combination. Only one protocol that uses USB can be active at a time.");
         return -1;
      }
   }
   uint8_t tpxSettings::setYMode(uint8_t modeToSet)
   {
      if (isValid(X.mode, modeToSet, TOT.mode)) {
         Y.mode = modeToSet;
         return 1;
      }
      else {
         Serial.println("invalid mode combination. Only one protocol that uses USB can be active at a time.");
         return -1;
      }
   }
   uint8_t tpxSettings::setTOTMode(uint8_t modeToSet) 
   {
      if (isValid(X.mode, Y.mode, modeToSet)) {
         TOT.mode = modeToSet;
         return 1;
      }
      else {
         Serial.println("invalid mode combination. Only one protocol that uses USB can be active at a time.");
        return -1;
      }
   }
   uint8_t tpxSettings::getParamMode(char param)
   {
      switch (param) {
         case 'X':
            if (X.mode == MIDIUSB) {
               return MIDIUSB;
            }
            else if (X.mode == MIDIUART) {
               return MIDIUART;
            }
            else if (X.mode == OSC) {
               return OSC;
            }
            else return -1; // error
         case 'Y':
            if (Y.mode == MIDIUSB) {
               return MIDIUSB;
            }
            else if (Y.mode == MIDIUART) {
               return MIDIUART;
            }
            else if (Y.mode == OSC) {
               return OSC;
            }
            else return -1; // error
         case 'T':
            if (TOT.mode == MIDIUSB) {
               return MIDIUSB;
            }
            else if (TOT.mode == MIDIUART) {
               return MIDIUART;
            }
            else if (TOT.mode == OSC) {
               return OSC;
            }
            else return -1; // error

         default:
            return -1; // invalid parameter passed to function
      }
   }
   uint8_t tpxSettings::getParamSetting(char param, uint8_t key)
   {
      switch (param) {
         case 'X':
            return X.read(key);

         case 'Y':
            return Y.read(key);

         case 'T':
            return TOT.read(key);

         default:
            return -1; // invalid parameter passed to function

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
   uint8_t tpxSettings::setXOption(uint8_t option, uint8_t value) 
   {
     X.update(option, value);

     return 1; // need to add error handling
   }
   uint8_t tpxSettings::setYOption(uint8_t option, uint8_t value) 
   {
     Y.update(option, value);

     return 1; // need to add error handling
   }
   uint8_t tpxSettings::setTOTOption(uint8_t option, uint8_t value)
   {
     TOT.update(option, value);

     return 1; // need to add error handling
   }
   uint8_t tpxSettings::enOrDisableParam(char param, bool on)
   {
      switch (param) {
         case 'X':
            if (on == true) {
               X.enabled = true;
            }
            else {
               X.enabled = false;
            }
            break;
         case 'Y':
            if (on == true) {
               Y.enabled = true;
            }
            else {
               Y.enabled = false;
            }
            break;
         case 'T':
            if (on == true) {
               TOT.enabled = true;
            }
            else {
               TOT.enabled = false;
            }
            break;
      }
      return 1;
       // need to add error handling
   }
   bool tpxSettings::isParamEnabled(char param)
   {
      switch (param) {
         case 'X':
            return X.enabled;
            break;
         case 'Y':
            return Y.enabled;
            break;
         case 'T':
            return TOT.enabled;
            break;
       
         default: // error
            return false;
      }
   }


