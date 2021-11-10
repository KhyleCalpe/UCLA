/*
 * cnn.h
 *
 *  Created on: Oct 24, 2020
 *      Author: phill
 */

#ifndef INC_CNN_H_
#define INC_CNN_H_

// last parameter maybe bias size
void conv2D(float*** input, float* kernel, float* bias, float*** output, int height, int width,
		int depth, int kernel_depth);
void pooling(float*** input, float*** output, int height, int width, int depth, int pool_size);
void dense_R(float* input, float* kernel, float* bias, float* output, int input_size, int output_size);
void dense_S(float* input, float* kernel, float* bias, double output[], int input_size, int output_size);

// helper functions
float* flatten3D(float*** input, int height, int width, int channel);
float*** create3DArray(int height, int width, int depth);
void free3DArray(float*** array, int height, int depth);

// testing
void randomize3DArray(float*** array, float max, int height, int width, int depth);
float getRandomFloat(float max);
void print3DArray(float*** array, int height, int width, int channel);
void change1Dto3DArray(float* input, float*** output, int height, int width, int depth);

#endif /* INC_CNN_H_ */
