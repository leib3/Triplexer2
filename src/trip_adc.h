#include "WProgram.h"
#include "ADC.h"

//adc macros
#define NUMAVG    4   //amount of hardware averaging
#define ADCBITS   16  //bit depth of samples from adc
#define SAMPLERATE 1  //in Hz. Currently set really slow for debugging.


void adcinit();
void timerinit();
void sampleTimer_isr();
void adc0_isr();
void adc1_isr();

