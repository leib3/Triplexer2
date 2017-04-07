#include "stdint.h"

#define PIN_ENTER 0 // digital pins for menu
#define PIN_UP 7
#define PIN_DOWN 1
#define PIN_BACK 3
#define PIN_SAVE 10 // may need to change

#define ENTER 0
#define UP 1
#define DOWN 2
#define BACK 3
#define SAVE 4

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

