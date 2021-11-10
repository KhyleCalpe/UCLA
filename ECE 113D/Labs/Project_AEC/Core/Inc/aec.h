#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <arm_math.h>

#define ARM_MATH_CM7
#define __FPU_PRESENT 1

extern float 				DC_offset;
extern int 					pts;
extern float 				gain;
extern uint16_t 		ADC_buff[];
extern uint16_t 		ADC_out[];

extern int 					head; // keep track of current head of array
extern const float 	stepsize;
extern float			 	signal_energy;

extern float 				x[]; // buffer to store samples backwards
extern float 				abs_x[]; // used for dtd
extern float 				weights[]; // weights vector

// Filters
float prewhiten_x(float in);
float prewhiten_e(float in);
float bandpass(float in);

/*
 *  Double Talk Detector
 *  Check if near end talk (mic-side speaker) is present
 *  If present, filter updating is frozen to prevent speaker
 * */
bool dtd(float input, float output);

/*
 *  Adaptive FIR Filter
 *  The following equation calculates the FIR filter response:
 *
 *              out = sum(k=0,N-1) w * x(n-k)
 *
 *  where
 *              N   ~ # of filter taps
 *              w   ~ filter coefficients
 *              x   ~ speaker-side signal
 *              x   = y(n-0), y(n-1), ..., y(n-N+1)
 */
float nlms(float input, float output, bool update);

/*
 *  Wrapper function for entire aec process
 */
void aec(int len);
