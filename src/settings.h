#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <stdint.h>
// convenient defines to pass to setParamMode, get from getParamMode 
#define SETTINGS_MODE_MIDIUSB 0
#define SETTINGS_MODE_MIDIUART 1
#define SETTINGS_MODE_OSC 2
// convenient defines for use with getParamOption, setParamOption, which take uint8's
#define SETTINGS_MIDICHNL 0
#define SETTINGS_MIDICC 1
#define SETTINGS_INV 2
#define SETTINGS_CURVE_SENSITIVITY 3
#define SETTINGS_CURVE_WIDTH 4

class param {
   public:
      uint8_t settings[6];
      bool enabled;
      uint8_t mode;
      param() : enabled(true), mode(SETTINGS_MODE_MIDIUSB) // on instantiation all params are initially disabled
      {
       // initialize settings map! 
         settings[0] = 1; // MIDI Channel
         settings[1] = 1; // MIDI CC number
         settings[2] = 0; // invert
         settings[3] = 50; // response curve sensitivity 0 = exponential, 50 = linear, 100 = log
         settings[4] = 100; //100 = max width, 0 = min
      }
      //copy constructor. Handy for use with menu
      param(const param &p){
         enabled = p.enabled;
         mode = p.mode;
         settings[0] = p.settings[0]; // MIDI Channel
         settings[1] = p.settings[1]; // MIDI CC number
         settings[2] = p.settings[2]; // invert
         settings[3] = p.settings[3]; // response curve sensitivity 0 = exponential, 50 = linear, 100 = log
         settings[4] = p.settings[4]; //100 = max width, 0 = min
      }
      void copy(const param &p) {
         enabled = p.enabled;
         mode = p.mode;
         settings[0] = p.settings[0]; // MIDI Channel
         settings[1] = p.settings[1]; // MIDI CC number
         settings[2] = p.settings[2]; // invert
         settings[3] = p.settings[3]; // response curve sensitivity 0 = exponential, 50 = linear, 100 = log
         settings[4] = p.settings[4]; //100 = max width, 0 = min
      }
      // does no error checking. rely on front-end to do it
      void set(uint8_t option, uint8_t value); 
      uint8_t get(uint8_t option);
};


//param mode, param invert
class tpxSettings {
   public:
      param X, Y, Z;
      //bool isValid(uint8_t XMode, uint8_t YMode, uint8_t Z); 
      bool isParamEnabled(char param);
      uint8_t setParamMode(char param, uint8_t modeToSet);
      uint8_t getParamMode(char param);
      uint8_t getParamSetting(char param, uint8_t key);
      uint8_t setParamOption(char param, uint8_t option, uint8_t value);
      uint8_t enOrDisableParam(char param, bool on);
};

#endif
