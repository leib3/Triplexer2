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

// Create global struct or pointer to global struct 
// containing current MIDI channel #s and CC #s for each 
// control parameter, plus other relevant status info for
// debugging purposes. There will be a single instance of this
// struct that is shared by both the menu and the load cell
// input processing module (TODO)
typedef struct settingsStruct
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
} tpxSettings;

void browseMenu();

void showMenu();
