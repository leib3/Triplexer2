#include "WProgram.h"
#include "ADC.h"
#include "IntervalTimer.h"
#include "settings.h"
#include "trip_osc.h"
#include "trip_adc.h"
#include "MIDI.h"
#include "trip_midi.h"
#include "menu.h"
#include "rgb.h"
#include "ST7565.h"

using namespace std;

//Globals
ST7565 lcd(7, 6, 5, 4);

//fixes stupid compiler error. Maybe move this somewhere less obtrusive?
extern "C"{
  int _getpid(){ return -1;}
  int _kill(int pid, int sig){ return -1; }
  int _write(){return -1;}
}

tpxSettings settings_object;
tpxSettings * Settings = &settings_object;
extern ADC myAdc;

//static int * raw;
const int channel = 1;

void  init() {
   Serial.begin(9600);
   pinMode(13, OUTPUT);
   //pinMode(0, INPUT);
   digitalWriteFast(13, HIGH);
   //MIDI.begin(1);
}

void menu_init() {

}

extern "C" int main() 
{
  init();
  adcinit();
  //timerinit(); //add this in to make the adc run
  //oscinit();
  //calEEPROMinit();
  //loadPresetDefault();

//menu test stuff
  RGB rgb (8, 9, 10);
  struct rgb_color_t blue;
  blue.r = 0;
  blue.g = 0;
  blue.b = 255;
  rgb.set_pwm_n(blue);
  lcd.begin(30);
  lcd.clear();
  lcd.drawstring(2, 5, "Triplexer 2");
  lcd.display();
  int test_data = -100;
  char buf[16];
  MenuEntryDataInt mydata(&test_data);
  SubMenu toplevel (NULL); 
  SubMenu lowerlevel (&toplevel);
  MenuEntry entry1((char *)"entry 1", NULL, (MenuEntryData *) &mydata );
  MenuEntry entry2((char *)"entry 2", NULL, NULL, &lowerlevel);
  MenuEntry lower_entry1((char *)"lower entry 1", NULL, NULL);
  MenuEntry lower_entry2((char *)"lower entry 2", NULL, NULL);
  toplevel.add_entry(&entry1);
  toplevel.add_entry(&entry2);
  lowerlevel.add_entry(&lower_entry1);
  lowerlevel.add_entry(&lower_entry2);
  LcdDrawMenu lcd_menu_draw (MENU_MAX_STR_LEN);
  Menu menu (&toplevel, &toplevel, (DrawMenu *) &lcd_menu_draw);
  menu.draw();
   while (1) {
   //checkmidi();
   //checkosc(); //this function only works inside a while loop
   } 
}





