#include <stdint.h>
#include <map>

#define MIDIUSB 0
#define MIDIUART 1
#define OSC 2

class param {
   private:
      std::map<uint8_t, uint8_t> settings;
   public:
      bool enabled;
      uint8_t mode;
      param() : enabled(false), mode(MIDIUART) // on instantiation all params are initially disabled
      {
       // initialize settings map! 
         settings[0] = 1; // MIDI Channel
         settings[1] = 1; // MIDI CC number
         settings[2] = 0; // invert
         settings[3] = 1; // OSC setting, to be determined
         settings[4] = 1; // OSC setting, to be determined
      }
      // does no error checking. rely on front-end to do it
      // which is exactly what the LCD menu implementation does right now
      void update(uint8_t option, uint8_t value); 
      uint8_t read(uint8_t option);
};

class tpxSettings {
   private:
      param X, Y, TOT;
   public:
      bool isValid(uint8_t XMode, uint8_t YMode, uint8_t TOTMode); 
      uint8_t setXMode(uint8_t modeToSet);
      uint8_t setYMode(uint8_t modeToSet);
      uint8_t setTOTMode(uint8_t modeToSet);
      uint8_t getParamMode(char param);
      uint8_t getParamSetting(char param, uint8_t key);
      uint8_t setXOption(uint8_t option, uint8_t value);
      uint8_t setYOption(uint8_t option, uint8_t value);
      uint8_t setTOTOption(uint8_t option, uint8_t value);
};
