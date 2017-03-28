#include "WProgram.h"
#include "ADC.h"

//adc macros
#define NUMAVG    1   //amount of hardware averaging
#define ADCBITS   16  //bit depth of samples from adc
#define SAMPLERATE 1000  //in Hz. Currently set slow for debugging.
#define BUFSZ 16
#define BUFSZBITS  4

void adcinit();
void adcCalibrate();
void timerinit();
void sampleTimer_isr();
void adc0_isr();
void adc1_isr();

