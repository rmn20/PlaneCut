#ifndef BUCKET_H
#define BUCKET_H

#include <stdbool.h>

#include "cvector.h"

#include "vec3.h"
#include "color.h"

typedef struct {
	//Нормаль (вектор направления) плоскости сечения
	vec3 planeNormal;
	//Центр плоскости сечения
	vec3 planeCenter;
	
	//Расстояние от цветов в ведре до его цвета
	float length;
	bool dontPick;
	
	cvector_vector_type(vec3) colors;
	
	//Цвет самого ведра
	Color col;
} Bucket;

void bucketFree(Bucket* bucket);

void bucketAddColor(Bucket* bucket, vec3* col);

void bucketCompute(Bucket* bucket, int bits);
void bucketCut(Bucket* bucket, Bucket* b1, Bucket* b2, int bits);

#endif