/*
 * jwbmp.c
 *
 *  Created on: Aug 7, 2020
 *      Author: catchc-w
 */


#include <jwread.h>
#include <stdlib.h>

float * ProcessBmp(char *textin){
	int pixel_start = textin[10];
	int img_size = textin[18] * textin[22];
	float * result_img;
	result_img = (float *) malloc(sizeof(float)*img_size);

	for(int i = 0; i < img_size;i++){
		result_img[i] = 255.0 - textin[pixel_start + i*3];
	}
	return result_img;
}

