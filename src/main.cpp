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
#include "button.h"
#include "triplexer.h" //contains stuff that would gunk up main.cpp but doesn't really fit elsewhere
#include "Encoder.h"

using namespace std;

//extern globals
extern volatile unsigned int adc_ticks; //global ticks. Incremented by adc isr
extern IntervalTimer sampleTimer;

// globals

//Menus
Menu main_menu;
Menu utilities_menu;
Menu *current_menu = &main_menu;

//current saved setting number
int settings_index = 0;

//PWM backlight control
struct rgb_color_t blue {0, 0, 255};
RGB rgb(8, 9, 10);

//ST7565 graphic lcd
ST7565 lcd(7, 6, 5, 4);

//buttons
Button save_button(SAVE_BUTTON_PIN, BUTTON_DEBOUNCE_PERIOD);
Button back_button(BACK_BUTTON_PIN, BUTTON_DEBOUNCE_PERIOD);
Button enc_button(ENC_BUTTON_PIN, BUTTON_DEBOUNCE_PERIOD);

Encoder encoder(23, 22);
//settings
tpxSettings settings_object;
tpxSettings * Settings = &settings_object;


extern "C" int main() 
{
  //intialize_EEPROM_with_default_settings(); //uncomment to initialize EEPROM on first board programming.
  teensy_init();
  button_init();
  lcd_init();
  menu_init();
  adcinit();
  //oscinit();
  //calEEPROMinit();
  //loadPresetDefault();
//menu test stuff
  timerinit();
  while (1) {
    //Serial.flush();
    delay(1);
    //Serial.print("hello\n\r");
    //Serial.send_now();
    //sampleTimer.end();
    //Serial.print(adc_ticks);
    //update button state
    back_button.update(adc_ticks);
    save_button.update(adc_ticks);
    enc_button.update(adc_ticks);
    encoder_update(adc_ticks);
    lcd_refresh(adc_ticks);
    //checkmidi();
    //checkosc(); //this function only works inside a while loop
    //digitalWriteFast(13, HIGH);
    //delay(1);
    //digitalWriteFast(13, LOW);
    //delay(500);
  } 
}

