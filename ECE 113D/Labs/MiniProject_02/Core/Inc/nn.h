/*
 * nn.h
 *
 *  Created on: Dec 1, 2020
 *      Author: kcalp
 */

#ifndef INC_NN_H_
#define INC_NN_H_

// layer sizes
#define SIZE_1 5

extern float x1_offset[13];
extern float x1_gain[13];
extern float x1_ymin;

extern float b1[SIZE_1];
extern float w1[13*SIZE_1];
extern float b2[4];
extern float w2[SIZE_1*4];

// NN functions
void dense_Sig(float input[], double output[], int input_size, int output_size);
void dense_S(double input[], double output[], int input_size, int output_size);

#endif /* INC_NN_H_ */
