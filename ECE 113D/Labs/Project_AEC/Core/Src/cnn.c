/*
 * cnn.c
 *
 *  Created on: Oct 24, 2020
 *      Author: phill
 */
#include "cnn.h"
#include "math.h"
#include <stdlib.h>
#include <stdio.h>

void conv2D(float*** input, float* kernel, float* bias, float*** output, int height, int width,
		int depth, int kernel_depth)
{
	// ONLY works for 3x3 filter
	// for each element in array, take surrounding elements in array equal to filter size
	// dot product with filter weights, assume same padding
	// apply relu on result

	int kernel_height = 3;
	int kernel_width = 3;
	int out_height = height;
	int out_width  = width;
	int out_depth  = kernel_depth;
	float input_val  = 0;
	float kernel_val = 0;
	float output_val = 0;

	for (int row = 0; row < out_height; row++)
	{
		for (int col = 0; col < out_width; col++)
		{
			for (int channel = 0; channel < out_depth; channel++)
			{
				output[row][col][channel] = 0;
			}
		}
	}
	for (int kernel_channel = 0; kernel_channel < kernel_depth; kernel_channel++)
	{
		for (int row = 0; row < height; row++)
		{
			for (int col = 0; col < width; col++)
			{
				for (int channel = 0; channel < depth; channel++)
				{
					for (int kernel_row = 0; kernel_row < kernel_height; kernel_row++)
					{
						// if far top or bottom, skip
						if (kernel_row+row-1 < 0)
							continue;
						if (kernel_row+row-1 >= height)
							continue;

						for (int kernel_col = 0; kernel_col < kernel_width; kernel_col++)
						{
							// if far left or right, skip
							if (kernel_col+col-1 < 0)
								continue;
							if (kernel_col+col-1 >= width)
								continue;

							input_val   = input[kernel_row+row-1][kernel_col+col-1][channel];
							kernel_val  = kernel[kernel_channel + kernel_depth*(channel + depth*(kernel_col + kernel_width*kernel_row))];
							output_val += (input_val * kernel_val);
						}
					}
				}
				output_val += bias[kernel_channel];
				// relu
				if (output_val < 0)
					output[row][col][kernel_channel] = 0;
				else
					output[row][col][kernel_channel] = output_val;
				// Reset output value
				output_val = 0;
			}
		}

	}
}

void pooling(float*** input, float*** output, int height, int width, int depth, int pool_size)
{
	// Round up max pool count based on input dimensions
	int max_count_pool_row = height/pool_size;
	int max_count_pool_col = width/pool_size;

	// Store max value element
	float input_value = 0;
	float max_value = 0;

	// Propagate through the arrays
	for(int channel = 0; channel < depth; channel++)
	{
		// Cycle through the input layer row by pool size
		for (int row_count = 0; row_count < max_count_pool_row; row_count++)
		{
			// Cycle through the input layer col by pool size
			for (int col_count = 0; col_count < max_count_pool_col; col_count++)
			{
				// Cycle through pool row
				for (int pool_row = row_count*pool_size;
						pool_row < height && pool_row < (row_count*pool_size+pool_size);
						pool_row++)
				{
					// Cycle through pool col
					for (int pool_col = col_count*pool_size;
							pool_col < width && pool_col < (col_count*pool_size+pool_size);
							pool_col++)
					{
						// Check for max value in a pool
						input_value = input[pool_row][pool_col][channel];

						if (input_value > max_value)
						{
							max_value = input_value;
						}
						input_value = 0;
					}
				}
				// Set output element to max value
				output[row_count][col_count][channel] = max_value;
				// Reset max value
				max_value = 0;
			}
		}
	}
}

void dense_R(float* input, float* kernel, float* bias, float* output, int input_size, int output_size)
{
	for (int row=0; row<output_size; row++)
	{
		for (int col=0; col<input_size; col++)
		{
			// dot product
			// kernel transposed before flattened
			output[row] += (kernel[col + row*input_size] * input[col]);
		}
		output[row] += bias[row];
		if (output[row] < 0)
			output[row] = 0;
	}
}

void dense_S(float* input, float* kernel, float* bias, double output[], int input_size, int output_size)
{
	// matrix multiplication between input and kernel
	// add bias to the result
	// calculate exp of the result
	// divide each output exp by sum of exps to get probability
	// should have 1 output for every image
	//float input_sum = 0;
	//int   kernel_size = input_size*output_size;
	//float weight_sum = 0;
	float output_value = 0;
	double output_sum = 0;
	/*
	for (int i=0; i<input_size; i++)
		input_sum += input[i];
	for (int i=0; i<input_size; i++)
		input[i] /= input_sum;
	for (int j=0; j<kernel_size; j++)
		weight_sum += kernel[j];
	for (int j=0; j<kernel_size; j++)
		kernel[j] /= weight_sum;
	 */
	for (int row=0; row<output_size; row++)
	{
		for (int col=0; col<input_size; col++)
		{
			// dot product
			// kernel transposed before flattened
			output[row] += (kernel[col + row*input_size] * input[col]);
		}
		// add bias
		output_value = output[row] + bias[row];
		output[row] = (double) output_value;
		output[row] = exp(output[row]);
		output_sum += output[row];
	}

	// normalizing for probability
	for (int i=0; i<output_size; i++)
	{
		output[i] /= output_sum;
	}
}

float* flatten3D(float*** input, int height, int width, int channel)
{
	// create a 1D array from a 3D array
	int output_size = height * width * channel;
	float* output = malloc(output_size * sizeof(float));

	for (int row=0; row<height; row++)
	{
		for (int col=0; col<width; col++)
		{
			for (int depth=0; depth<channel; depth++)
				output[depth + channel*(col + width*row)] = input[row][col][depth];
		}
	}

	// free input
	free3DArray(input, height, width);

	return output;
}

float*** create3DArray(int height, int width, int depth)
{
	// Allocate memory for array
	float*** array = (float***) malloc(height * sizeof(float**));
	// 2nd dimension
	for (int i = 0; i < height; i++)
	{
		array[i] = (float**) malloc(width * sizeof(float*));
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			array[i][j] = (float*) malloc(depth * sizeof(float));
		}
	}
	return array;
}

void free3DArray(float*** array, int height, int width)
{
	// Free occupied resources by column
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
			free(array[i][j]);
	}
	// Free occupied resources by row
	for (int i = 0; i < height; i++)
		free(array[i]);

	// Free occupied resources by channel
	free(array);
}

void randomize3DArray(float*** array, float max, int height, int width, int depth)
{
	// Set the array elements
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			for (int k = 0; k < depth; k++)
			{
				array[i][j][k] = getRandomFloat(max);
			}
		}
	}
}

float getRandomFloat(float max)
{
	return max * ((float) rand() / RAND_MAX);
}

void print3DArray(float*** array, int height, int width, int channel)
{
	float val;
	// Print the array
	for (int k = 0; k < channel; k++)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				val = array[i][j][k];
				if (val < 10)
				{
					printf("  %.2f ", val);
				}
				else if (val < 100)
				{
					printf(" %.2f ", val);
				}
				else
				{
					printf("%.2f ", val);
				}
			}
			printf("\n");
		}
		printf("\n");
	}
}

void change1Dto3DArray(float* input, float*** output, int height, int width, int depth)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			for (int k = 0; k < depth; k++)
			{
				output[i][j][k] = input[k + depth * (j + width * i)];
			}
		}
	}
}
