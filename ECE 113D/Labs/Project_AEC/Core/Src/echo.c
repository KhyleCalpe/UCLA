#include "echo.h"

void echo(int buffer_length, float fs, float delay, float gain)
{
	// Calculate samples
	float dt  = 1 / fs;
	pts = round((delay/1000.f)/dt);
	// Even out pts
	pts -= pts%2;
	// Fill in output before echo
	for (int i = 0; i < pts; i+=2)
	{
		ADC_out[i] = ADC_buff[i];
		ADC_out[i+1] = ADC_buff[i+1];
	}
	// Add the echo signal
	for (int j = pts; j < buffer_length; j+=2)
	{
		ADC_out[j] = ADC_buff[j] + ADC_buff[j-pts] * gain;
		ADC_out[j+1] = ADC_buff[j+1] + ADC_buff[j+1-pts] * gain;
	}
}

/*
 *  Helper Functions
 */

void setSeed(int seed)
{
	// set the seed
	srand(seed);
}

float randFloat(float min, float max)
{
	float scale = rand() / (float) RAND_MAX; // [0, 1.0]
	return min + scale * (max - min);      // [min, max]
}
