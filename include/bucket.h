#ifndef BUCKET_H
#define BUCKET_H

#include <stdbool.h>

#include "cvector.h"

#include "vec3.h"
#include "color.h"

typedef struct {
	//Cut plane normal
	vec3 planeNormal;
	//Cut plane center
	vec3 planeCenter;
	
	//Distance from the colors in the bucket to its color
	float length;
	bool dontPick;
	
	cvector_vector_type(vec3) colors;
	
	//Bucket color
	Color col;
} Bucket;

void bucketFree(Bucket* bucket);

void bucketAddColor(Bucket* bucket, vec3* col);

void bucketCompute(Bucket* bucket, int bits);
void bucketCut(Bucket* bucket, Bucket* b1, Bucket* b2, int bits);

#endif