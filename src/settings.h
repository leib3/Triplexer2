#include <stdint.h>
#include <map>
class ControlModeAndSettings{
   private:
      std::map<uint8_t, uint8_t> settings;
   public:
      uint8_t mode;
      void update(uint8_t option, uint8_t value);

      uint8_t read(uint8_t option);
};

class param {
   public:
      ControlModeAndSettings active;
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
