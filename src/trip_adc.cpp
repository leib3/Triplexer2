#include "trip_adc.h"
#include "trip_osc.h"
#include "WProgram.h"


//adc pin defs
const int readPinUL = A0; // uses ADC0
const int readPinUR = A1; // uses ADC0
const int readPinLL = A2; // uses ADC1
const int readPinLR = A3; // uses ADC1


volatile unsigned int ul, ur, ll, lr;  //upper left, upper right, lower left, lower right values (A0 to A3, respectively)
static int z_ul, z_ur, z_ll, z_lr; //store corner zero values
static int w_t; // max total weight
volatile int adc0_state, adc1_state;   //state used in adc isr's

ADC myAdc;
IntervalTimer sampleTimer;


void adcinit(){
    ADC * adc = &myAdc;
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
    //also note you can't use the basic blocking analogRead function without disabling interrupts first.
    adc->enableInterrupts(ADC_0);
    adc->enableInterrupts(ADC_1);

}

void timerinit(){
   sampleTimer.begin(sampleTimer_isr, 1000000/SAMPLERATE);
   sampleTimer.priority(20); //could tweak this. Teensy interrupts are mostly set to priority=128, so this is a pretty high priority
}

//called by adcCalibrate
void calibrateZero(void){
    unsigned int m_ul, m_ur, m_ll, m_lr;
    m_ul = m_ur = m_ll = m_lr = 0;
    for(int i = 0; i<256; i++){
       m_ul += myAdc.analogRead(A0);
       m_ur += myAdc.analogRead(A1);
       m_ll += myAdc.analogRead(A2);
       m_lr += myAdc.analogRead(A3);
       delayMicroseconds(3910);  // =  3.91 ms   (*256 = 1 second)
    }
    z_ul = m_ul >> 8;
    z_ur = m_ur >> 8;
    z_ll = m_ll >> 8;
    z_lr = m_lr >> 8;
}

//called by adcCalibrate
void calibrateMax(void){
    int m_ul, m_ur, m_ll, m_lr, w_ul, w_ur, w_ll, w_lr;
    m_ul = m_ur = m_ll = m_lr = 0;
    for(int i = 0; i<256; i++){
       m_ul += myAdc.analogRead(A0);
       m_ur += myAdc.analogRead(A1);
       m_ll += myAdc.analogRead(A2);
       m_lr += myAdc.analogRead(A3);
       delayMicroseconds(3910);  // =  3.91 ms   (*256 = 1 second)
    }
//now we have an average of sorts for each corner. Use these to set max total weight, 
//and individual weights for weighted avgs of corners
    m_ul = (1<<15) / ((m_ul>>8) - z_ul); 
    m_ul = w_ul < 1 ? 1 : w_ul;
    m_ur = (1<<15) / ((m_ur>>8) - z_ur);
    m_ur = w_ur < 1 ? 1 : w_ur;
    m_ll = (1<<15) / ((m_ll>>8) - z_ll);
    m_ll = w_ll < 1 ? 1 : w_ll;
    m_lr = (1<<15) / ((m_lr>>8) - z_lr);
    m_lr = w_lr < 1 ? 1 : w_lr;
    w_t = w_ul+w_ur_w_ll+w_lr;
}


void adcCalibrate(){
//using LED indication for now. Use display later?
    //turn off interrupts for timer and adc
    myAdc.disableInterrupts(ADC_0);
    myAdc.disableInterrupts(ADC_1);
    sampleTimer.end();
    //LED on
    pinMode(13, OUTPUT);
    digitalWriteFast(13, HIGH);
    calibrateZero();
    //LED off
    digitalWriteFast(13, LOW);
    //wait 1 second
    delayMicroseconds(1000000);
    //LED on
    digitalWriteFast(13, HIGH);
    //wait 1 second
    delayMicroseconds(1000000);
    calibrateMax();
    //LED off
    digitalWriteFast(13, LOW);
    //turn on interrupts for timer and adc

//print values for debugging
    Serial.println("z_ul = ");
    Serial.println(z_ul);
    Serial.println("w_ul = ");
    Serial.println(w_ul);

    Serial.println("z_ur = ");
    Serial.println(z_ur);
    Serial.println("w_ur = ");
    Serial.println(w_ur);

    Serial.println("z_ll = ");
    Serial.println(z_ll);
    Serial.println("w_ll = ");
    Serial.println(w_ll);

    Serial.println("z_lr = ");
    Serial.println(z_lr);
    Serial.println("w_lr = ");
    Serial.println(w_lr);

    myAdc.enableInterrupts(ADC_0);
    myAdc.enableInterrupts(ADC_1);
    timerinit();
    return;
}


void sampleTimer_isr(){
   if(adc1_state == 1){
   adc0_state = 0;
   adc1_state = 0;
   //TODO this should change to check output format for each of x, y, t
   //and call appropriate output functions
   //calculate 16-bit values-put this in a function!
   unsigned int x, y, t, ul_norm, ur_norm, ll_norm, lr_norm, t_norm;
   int lshiftamt = 16;  //(multiplier = 2^lshiftamt)
   ul_norm = (int)ul-z_ul > 0 ? ul-z_ul: 0;  //norm values will always be less than 2^16
   ur_norm = (int)ur-z_ur > 0 ? ur-z_ur: 0;
   ll_norm = (int)ll-z_ll > 0 ? ll-z_ll: 0;
   lr_norm = (int)lr-z_lr > 0 ? lr-z_lr: 0;
   t_norm = w_ul*ul_norm + w_ur*ur_norm + w_ll*ll_norm + w_lr*lr_norm;
   if (t_norm == 0) t_norm = 1; //avoid divde by 0
   x = ((w_ur*ur_norm + w_lr*lr_norm) << lshiftamt) / t_norm;
   x = x >= (1<<16) ? (1<<16)-1 : x;  //prevents x from being larger than the guaranteed max value of 2^16 -1.
   y = ((w_ul*ul_norm + w_ur*ur_norm) << lshiftamt) / t_norm;
   y = y >= (1<<16) ? (1<<16)-1 : y;  //prevents y from being larger than the guaranteed max value of 2^16 -1.
   t = (t_norm)>>1; // = max t_norm = 2^17. l shift amt - 2^16. t = (t_norm/max t_norm)<<lshiftamt = t_norm >> 1;
   t = t >= (1<<16) ? (1<<16)-1 : t;  //prevents t from being larger than the guaranteed max value of 2^16 -1.
//output to osc, midi, or midi over usb
   //oscsend3(x, y, t);
/*
   //print values for debugging
   Serial.print("\n\r");
   Serial.println( ul);
   Serial.println( ur);
   Serial.println(ll);
   Serial.println( lr);
   Serial.print("\n\r");
   Serial.println( ul_norm);
   Serial.println( ur_norm);
   Serial.println(ll_norm);
   Serial.println( lr_norm);
   Serial.print("\n\r");
*/
   }
   myAdc.startSingleRead(readPinUL, ADC_0);
   myAdc.startSingleRead(readPinLL, ADC_1);
}


//these adc isr's will either read the left value, then start the right value read, or
//on the subsequent time, only read the right value
//they are designed to work with a separate timer interrupt, which starts the first read.
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
