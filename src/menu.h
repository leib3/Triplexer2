#include "stdint.h"

#define PIN_ENTER 0 // digital pins for menu
#define PIN_UP 7
#define PIN_DOWN 1
#define PIN_BACK 3

#define ENTER 0
#define UP 1
#define DOWN 2
#define BACK 3
typedef struct MenuStruct
{  const char * text;
   unsigned char num_menupoints;
   unsigned char up;
   unsigned char down;
   unsigned char enter;
   unsigned char back;
   void ( *fp ) ( void );
} MenuEntry;

void browseMenu();
void showMenu();
void debounceInit();

//struct settingsStruct
//{
//   uint8_t xCCNum;
//   uint8_t xChannelNum;
//   uint8_t yCCNum;
//   uint8_t yChannelNum;
//   uint8_t totCCNum;
//   uint8_t totChannelNum;
//   bool xInv;
//   bool yInv;
//   bool totInv;
//};

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

