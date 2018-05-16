#include "WProgram.h"
#include "ADC.h"
#include "IntervalTimer.h"
#include "settings.h"
#include "trip_osc.h"
#include "trip_adc.h"
#include "MIDI.h"
#include "trip_midi.h"

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

extern "C" int main() 
{
  init();
  adcinit();
  timerinit();
  //oscinit();
  //calEEPROMinit();
  //loadPresetDefault();
  //adcCalibrate();
   while (1) {
   //checkmidi();
   //checkosc(); //this function only works inside a while loop
   } 
}





