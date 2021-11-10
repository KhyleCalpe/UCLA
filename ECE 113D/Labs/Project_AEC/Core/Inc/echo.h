#include "stm32h7xx_hal.h"
#include <stdlib.h>

extern int 			pts;
extern uint16_t ADC_buff[];
extern uint16_t ADC_out[];

void echo(int buffer_length, float fs, float delay, float gain);

// Helper functions
void 	setSeed(int seed);
float randFloat(float min, float max);
