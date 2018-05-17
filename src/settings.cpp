#include "settings.h"
#include "WProgram.h"

// modes defines for param class member mode

// TODO: parameter status menu screen OR persistent symbol in corner of LCD
// referring to which modes are active
// see defines in settings.h... MIDIUSB = 0, MIDIUART = 1, OSC = 2
//
// If we switch from either MIDIUSB or MIDIUART to OSC, we shouldn't have to erase it IFF
// OSC options are stored at different indices from the MIDI options.
// How much control do we give the user wrt OSC here?
// Couldn't there be any number of possible options, given the open nature of OSC?
   void param::set(uint8_t option, uint8_t value)
   {
      settings[option] = value; // menu.cpp already takes care of ensuring all MIDI settings
      // fall in valid range
   }
   uint8_t param::get(uint8_t option) // read current menu option value
   {
      return settings[option];
   }

   uint8_t tpxSettings::setParamMode(char param, uint8_t modeToSet)
   {
      switch (param) {
         case 'X':
            // test for valid combination of modes with prospective mode change
            {
              X.mode = modeToSet;
              return 0;
            }
        case 'Y':
            {
              Y.mode = modeToSet;
              return 0;
            }
        case 'Z':
            {
               Z.mode = modeToSet;
               return 0;
            }
        default:
            Serial.println("in tpxSettings::setParamMode invalid param name.");
            return -1; // error, invalid parameter
      }
   }
   
   uint8_t tpxSettings::getParamMode(char param)
   {
      switch (param) {
         case 'X':
            return X.mode;
         case 'Y':
            return Y.mode;
         case 'Z':
            return Z.mode;
         default:
            return -1; // invalid parameter passed to function
      }
   }
   uint8_t tpxSettings::getParamSetting(char param, uint8_t key)
   {
      switch (param) {
         case 'X':
            return X.get(key);

         case 'Y':
            return Y.get(key);

         case 'Z':
            return Z.get(key);

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
   uint8_t tpxSettings::setParamOption(char param, uint8_t option, uint8_t value) 
   {
      switch (param) {
         case 'X':
            X.set(option, value);
            break;
         case 'Y':
            Y.set(option, value);
            break;
         case 'Z':
            Z.set(option, value);
            break;
         default:
            return -1; // error
      }
      return 0;
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
         case 'Z':
            if (on == true) {
               Z.enabled = true;
            }
            else {
               Z.enabled = false;
            }
            break;
      }
      return 0;
       // need to add error handling
   }
   bool tpxSettings::isParamEnabled(char param)
   {
      switch (param) {
         case 'X':
            return X.enabled;
         case 'Y':
            return Y.enabled;
         case 'Z':
            return Z.enabled;
         default: // error
            return false;
      }
   }

