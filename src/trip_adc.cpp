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
unsigned int ul_sens = 150; unsigned int ur_sens = 255; unsigned int ll_sens=200; unsigned int lr_sens = 245; //sensitivy values for each sensor, 0 to 255
static int max_total; // max total weight
volatile int adc0_state, adc1_state;   //state used in adc isr's

ADC myAdc;
IntervalTimer sampleTimer;


//response curves and init functions for output. 256 points (with linear interpolation)
unsigned short *x_response_curve;
unsigned short *y_response_curve;
unsigned short *t_response_curve;

static unsigned short linear_response_curve[257] = {0};
static unsigned short linear_w_edges_response_curve[257] = {0};
static unsigned short log_response_curve[257] = {0};

void linear_response_curve_init(){
   short i;
   for(i=0; i<256; i++)
      linear_response_curve[i] = i<<8;
   linear_response_curve[256] = 0xffff;
}

void linear_w_edges_response_curve_init(){
   int i;
   int n = 20;
   int slope = (1<<16)/(256-2*n);
   for(i=n; i<256-n; i++)
      linear_w_edges_response_curve[i] = linear_w_edges_response_curve[i-1]+slope;
   for(i = 256 - n; i<257; i++)
       linear_w_edges_response_curve[i] = (1<<16) - 1; 
}

void log_response_curve_init(){
   double multiplier = 0.0;
   double previous = 1.0;
   int i;
   log_response_curve[0] = 1; //initial value
   multiplier = pow((double)(1<<16), 1.0/256.0);
   for(i = 1; i<=256; i++){
      previous =  previous*multiplier;
      log_response_curve[i] = (unsigned short)previous;
   }
}


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

    linear_response_curve_init();
    linear_w_edges_response_curve_init();
    log_response_curve_init();
    // If you enable interrupts, note that the isr will read the result, so that isComplete() will return false (most of the time)
    //also note you can't use the basic blocking analogRead function without disabling interrupts first.
    adc->enableInterrupts(ADC_0);
    adc->enableInterrupts(ADC_1);
    
}

void timerinit(){
   sampleTimer.begin(sampleTimer_isr, 1000000/SAMPLERATE);
   sampleTimer.priority(20); //could tweak this. Teensy interrupts are mostly set to priority=128, so this is a pretty high priority
   x_response_curve = linear_w_edges_response_curve;
   y_response_curve = linear_w_edges_response_curve;
   t_response_curve = log_response_curve;
   Serial.println("log response 256 = ");
   Serial.println(log_response_curve[256], HEX);
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
    int m_ul, m_ur, m_ll, m_lr;
    m_ul = m_ur = m_ll = m_lr = 0;
    for(int i = 0; i<256; i++){
       m_ul += myAdc.analogRead(A0);
       m_ur += myAdc.analogRead(A1);
       m_ll += myAdc.analogRead(A2);
       m_lr += myAdc.analogRead(A3);
       delayMicroseconds(3910);  // =  3.91 ms   (*256 = 1 second)
    }
//now we have an average of sorts for each corner. Use these to set max total weight, 
    m_ul =(m_ul>>8) - z_ul; 
    m_ul = ((int)m_ul) < 0 ? 0 : m_ul;
    m_ur =(m_ur>>8) - z_ur; 
    m_ur = ((int)m_ur) < 0 ? 0 : m_ur;
    m_ll =(m_ll>>8) - z_ll; 
    m_ll = ((int)m_ll) < 0 ? 0 : m_ll;
    m_lr = (m_lr>>8) - z_lr; 
    m_lr = ((int)m_lr) < 0 ? 0 : m_lr;
    //add, scaling by sensitivity for each sensor
    max_total = m_ul*ul_sens + m_ur*ur_sens+m_lr*lr_sens+m_ll*ll_sens;
}


void adcCalibrate(){
//using LED indication for now. TODO Use display later.
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
/*
    //print zero and max value for debugging
    Serial.print("zeros are: z_ul=");
    Serial.println(z_ul);
    Serial.print("z_ur=");
    Serial.println(z_ur);
    Serial.print("z_ll=");
    Serial.println(z_ll);
    Serial.print("z_lr=");
    Serial.println(z_lr);
    Serial.println("\n\nmax value is: ");
    Serial.println(max_total);
    Serial.print("\n\n");
*/
    //turn on interrupts for timer and adc
    myAdc.enableInterrupts(ADC_0);
    myAdc.enableInterrupts(ADC_1);
    timerinit();
    return;
}


void sampleTimer_isr(){
   static int current_buffer_i = 0;
   //buffers for output index values
   static unsigned int x_index_buf[BUFSZ] = {0};
   static unsigned int y_index_buf[BUFSZ] = {0};
   static unsigned int t_index_buf[BUFSZ] = {0};
   //TODO this should change to check output format for each of x, y, t- OSC, MIDI, MIDIUSB, and INVERT
   //and call appropriate output functions
   unsigned int x, y, t, //final output values
   ul_zc, ur_zc, ll_zc, lr_zc, //zero-corrected values
   ul_norm, ur_norm, ll_norm, lr_norm, t_norm, //for values normalized by sensitivity of each sensor
   old_x_index, old_y_index, old_t_index,
   new_x_index, new_y_index, new_t_index, //for index into response curve 
   x_index, y_index, t_index, //actual index values
   x_offset, y_offset, t_offset; //actual offset values (for interpolation in response curves) 
   static unsigned int x_buf_sum=0, y_buf_sum=0, t_buf_sum = 0; //for easy averaging 
   //get zero-corrected values;
   ul_zc = ul - z_ul; 
   ur_zc = ur - z_ul;
   ll_zc = ll - z_ll;
   lr_zc = lr - z_lr;
   ul_zc = ((int)ul_zc) < 0 ? 0 : ul_zc;
   ur_zc = ((int)ur_zc) < 0 ? 0 : ur_zc;
   ll_zc = ((int)ll_zc) < 0 ? 0 : ll_zc;
   lr_zc = ((int)lr_zc) < 0 ? 0 : lr_zc;
   //normalize using sensitivity values
   ul_norm = ul_zc * ul_sens;
   ur_norm = ur_zc * ur_sens;
   ll_norm = ll_zc * ll_sens;
   lr_norm = lr_zc * lr_sens;
   //calculate new x, y, and t indices
   t_norm = ul_norm+ur_norm+ll_norm+lr_norm;
   if(t_norm == 0) t_norm = 1; //avoid divide-by-zero errors
   new_x_index = ((((long long)ur_norm + (long long)lr_norm)<<16)/(long long)t_norm ); 
   new_y_index = (( ( (long long)ul_norm + (long long)ur_norm)<<16)/(long long)t_norm );    
   new_t_index =     (((long long)t_norm)<<16)  / max_total ;
   if(new_t_index >= 1<<16){
      new_t_index = 0xffff; 
   }
   //calculate average index and offset using buffer
   old_x_index = x_index_buf[current_buffer_i];
   old_y_index = y_index_buf[current_buffer_i];
   old_t_index = t_index_buf[current_buffer_i];
   x_index_buf[current_buffer_i]= new_x_index;
   y_index_buf[current_buffer_i]= new_y_index;
   t_index_buf[current_buffer_i]= new_t_index;
   current_buffer_i = (current_buffer_i+1)&(BUFSZ-1);  //increment buffer index for next iteration
   x_buf_sum += new_x_index - old_x_index;
   y_buf_sum += new_y_index - old_y_index;
   t_buf_sum += new_t_index - old_t_index;
   //get index and offset
   x_index = x_buf_sum >> (8+BUFSZBITS);
   y_index = y_buf_sum >> (8+BUFSZBITS);
   t_index = t_buf_sum >> (8+BUFSZBITS);
   x_offset = (x_buf_sum >> (BUFSZBITS)) & 0xff;
   y_offset = (y_buf_sum >> (BUFSZBITS)) & 0xff;
   t_offset = (t_buf_sum >> (BUFSZBITS)) & 0xff;
   //now do linear interpolation from response curve
   x = x_response_curve[x_index]+ ((x_response_curve[x_index+1]-x_response_curve[x_index] )*x_offset  >> 8);
   y = y_response_curve[y_index]+ ((y_response_curve[y_index+1]-y_response_curve[y_index] )*y_offset  >> 8);
   t = t_response_curve[t_index]+ ((t_response_curve[t_index+1]-t_response_curve[t_index] )*t_offset  >> 8);
//output to osc, midi, or midi over usb
   oscsend3(x, y, t);
//change adc states
   adc0_state = 0;
   adc1_state = 0;
   myAdc.startSingleRead(readPinUL, ADC_0);
   myAdc.startSingleRead(readPinLL, ADC_1);
}


//these adc isr's will either read the left value, then start the right value read, or
//on the subsequent time, only read the right value
//they are designed to work with a separate timer interrupt, which starts the first read.
void adc0_isr(){
   if(adc0_state == 0){
      adc0_state = 1;
      ul = myAdc.adc0->readSingle();
      myAdc.startSingleRead(readPinUR, ADC_0);
   } else
   ur = myAdc.adc0->readSingle();
}

void adc1_isr(){
   if(adc1_state == 0){
      adc1_state = 1;
      ll = myAdc.adc1->readSingle();
      myAdc.startSingleRead(readPinLR, ADC_1);
   } else
   lr = myAdc.adc1->readSingle();
}
