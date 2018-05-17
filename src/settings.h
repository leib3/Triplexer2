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
   private:
      uint8_t settings[6];
   public:
      bool enabled;
      uint8_t mode;
      param() : enabled(false), mode(SETTINGS_MODE_MIDIUSB) // on instantiation all params are initially disabled
      {
       // initialize settings map! 
         settings[0] = 1; // MIDI Channel
         settings[1] = 1; // MIDI CC number
         settings[2] = 0; // invert
         settings[3] = 50; // response curve sensitivity 0 = exponential, 50 = linear, 100 = log
         settings[4] = 100; //100 = max width, 0 = min
      }
      // does no error checking. rely on front-end to do it
      void set(uint8_t option, uint8_t value); 
      uint8_t get(uint8_t option);
};


//param mode, param invert
class tpxSettings {
   private:
      param X, Y, Z;
   public:
      //bool isValid(uint8_t XMode, uint8_t YMode, uint8_t Z); 
      bool isParamEnabled(char param);
      uint8_t setParamMode(char param, uint8_t modeToSet);
      uint8_t getParamMode(char param);
      uint8_t getParamSetting(char param, uint8_t key);
      uint8_t setParamOption(char param, uint8_t option, uint8_t value);
      uint8_t enOrDisableParam(char param, bool on);
};
