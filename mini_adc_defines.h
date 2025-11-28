//#define PCLK   15000000
#define ADCLK   3000000
#define CLKDIV   ((PCLK/ADCLK)-1)//Clock Divider Value to set

#define CLKDIV_BITS 8//clk divider from 8 to 15 bits
#define PDN_BIT     21//P0.21 for PinDownBit in GPIO
#define START_CONVERTION 24//Start pin for start and stop convertion
#define DONE_BIT 31//Done bit for to check convertion status
#define ADC_RESULT 6//To check the resultant 10 bit value from pin 16

#define CH0 0
#define CH1 1
#define CH2 2
#define CH3 3





