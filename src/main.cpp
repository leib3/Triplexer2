#include "WProgram.h"
#include "menu.h"
#include "ADC.h"
#include "IntervalTimer.h"



               // RS, E, D4, D5, D6, D7
//LiquidCrystal lcd(29, 31, 19, 17, 14, 33);
//static int * raw;
const int channel = 1;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5  
#define NUMAVG    4   //amount of hardware averaging
#define ADCBITS   16  //bit depth of samples from adc
#define SAMPLERATE 1  //in Hz. Currently set really slow for debugging.

const int readPinUL = A0; // uses ADC0
const int readPinUR = A1; // uses ADC0
const int readPinLL = A2; // uses ADC1
const int readPinLR = A3; // uses ADC1

ADC myAdc;
IntervalTimer sampleTimer;

volatile int adc0_state, adc1_state;   //state used in adc isr's
volatile unsigned int ul, ur, ll, lr;  //upper left, upper right, lower left, lower right values (A0 to A3, respectively)

void sampleTimer_isr(){
   adc0_state = 0;
   adc1_state = 0;
   //this should change to output values instead of printing them.
   Serial.println(ul);
   Serial.println(ur);
   Serial.println(ll);
   Serial.println(lr);
   Serial.print("\n\r");
   myAdc.startSingleRead(readPinUL, ADC_0);
   myAdc.startSingleRead(readPinLL, ADC_1);
}


//these adc isr's will either read the left value, then start the right value read, or
//on the subsequent time, only read the right value
//they are designed to work with a timer interrupt.
void adc0_isr(){
   if(adc0_state == 0){
      adc0_state += 1;
      ul = myAdc.adc0->readSingle();
      myAdc.startSingleRead(readPinUR, ADC_0);
   } else
   ur = myAdc.adc0->readSingle();
}

void adc1_isr(){
   if(adc1_state == 0){
      adc1_state += 1;
      ll = myAdc.adc1->readSingle();
      myAdc.startSingleRead(readPinLR, ADC_1);
   } else
   lr = myAdc.adc1->readSingle();
}



void  init() {
   ADC * adc = &myAdc;
   Serial.begin(9600);
   Serial.println("Hello from outside Arduino!");
	pinMode(13, OUTPUT);
   digitalWriteFast(13, HIGH);


//ADC initialization
    pinMode(readPinUL, INPUT);
    pinMode(readPinUR, INPUT);
    pinMode(readPinLL, INPUT);
    pinMode(readPinLR, INPUT);
/*
    pinMode(A10, INPUT); //Diff Channel 0 Positive
    pinMode(A11, INPUT); //Diff Channel 0 Negative
    #if ADC_NUM_ADCS>1
    pinMode(A12, INPUT); //Diff Channel 3 Positive
    pinMode(A13, INPUT); //Diff Channel 3 Negative
    #endif
*/
    Serial.begin(9600);

    Serial.println("Begin setup");

    ///// ADC0 ////
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->setAveraging(NUMAVG); // set number of averages
    adc->setResolution(ADCBITS); // set bits of resolution

    // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
    // see the documentation for more information
    // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
    // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED); // change the conversion speed
    // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(3.3/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(3.3*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V

    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    adc->enableInterrupts(ADC_0);


    ////// ADC1 /////
    adc->setAveraging(NUMAVG, ADC_1); // set number of averages
    adc->setResolution(ADCBITS, ADC_1); // set bits of resolution
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_1); // change the conversion speed
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1); // change the sampling speed

    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_1);

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_1), 0, ADC_1); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V


    // If you enable interrupts, note that the isr will read the result, so that isComplete() will return false (most of the time)
    adc->enableInterrupts(ADC_1);


    Serial.println("End setup");
   //lcd.begin(16,2);
  // usbMIDI.sendNoteOn(60, 99, channel);
  // usbMIDI.sendControlChange(5, 50, channel);
   //raw = (int *)malloc(sizeof(int));
}


extern "C" int main() 
{

    ADC *adc = &myAdc;
	// To use Teensy 3.0 without Arduino, simply put your code here.
	// For example:
   init();
   sampleTimer.begin(sampleTimer_isr, 1000000/SAMPLERATE);
   sampleTimer.priority(20);
   while (1) {
      /*browseMenu();
      if (usbMIDI.read() != 0) {
         Serial.print("Rcvd type ");
         Serial.println(usbMIDI.getType());
         Serial.print("MIDI channel ");
         Serial.println(usbMIDI.getChannel());
      } */
   } 
}




