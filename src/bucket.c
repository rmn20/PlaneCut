#include <stdio.h>

#include "bucket.h"

void bucketFree(Bucket* bucket) {
	cvector_free(bucket->colors);
}

void bucketAddColor(Bucket* bucket, vec3* col) {
	//Adding color to a color vector
	cvector_push_back(bucket->colors, *col);
	//Adding a color to the center point of the cut plane
	//(then the division will be made by the number of colors)
	vec3Add(&(bucket->planeCenter), col);
}

size_t min(size_t a, size_t b) {
	return a < b ? a : b;
}

size_t max(size_t a, size_t b) {
	return a > b ? a : b;
}

void bucketCompute(Bucket* bucket, int bits) {
	//Bucket data calculation function
	cvector_vector_type(vec3) colors = bucket->colors;
	size_t colorsLength = cvector_size(colors);
	
	//Nothing to calculate
	if(colorsLength == 0) return;
	
	//Divide the sum of colors by their count
	//Using the arithmetic mean, we get the center point of the cut plane and the color of the bucket
	vec3Divv(&(bucket->planeCenter), colorsLength);
	bucket->col = vec3ToColor(&(bucket->planeCenter), bits);
	
	//Distance from the bucket color to the colors inside the bucket
	bucket->length = 0;
	for(size_t i=0; i<colorsLength; i++) {
		bucket->length += vec3ColorDistance(colors + i, &(bucket->planeCenter));
	}
	
	//Special cases
	if(colorsLength == 2) {
		vec3 tmp = *(colors + 1);
		vec3Sub(&tmp, colors);
		
		bucket->planeNormal = tmp;
		
		return;
	} else if(colorsLength == 1) return;
	
	//Calculating cut plane normal
	//Based on zalo.github.ioblog/line-fitting
	vec3 tmpNormal = (vec3) {0.5, 0.5, 0.5};
	vec3Normalize(&tmpNormal);
	vec3 prev = tmpNormal;
	vec3 pc = bucket->planeCenter;
	
	//I dont use vector functions in for, because then slow down execution
	for(size_t i=0; i<500; i++) {
		float nextDirectionx = 0,
			nextDirectiony = 0,
			nextDirectionz = 0;
		
		for(size_t t=0; t<colorsLength; t++) {
			vec3* p = colors + t;
			float centeredPointx = p->x - pc.x,
				centeredPointy = p->y - pc.y,
				centeredPointz = p->z - pc.z;
			
			float dot = centeredPointx * tmpNormal.x +
				centeredPointy * tmpNormal.y +
				centeredPointz * tmpNormal.z;
			
			nextDirectionx += dot * centeredPointx;
			nextDirectiony += dot * centeredPointy;
			nextDirectionz += dot * centeredPointz;
		}
		
		tmpNormal.x = nextDirectionx;
		tmpNormal.y = nextDirectiony;
		tmpNormal.z = nextDirectionz;
		vec3Normalize(&tmpNormal);
		
		//If current and previous normals are equal we dont need to continue loop
		//This check speeds up the execution almost twice
		if(tmpNormal.x == prev.x && tmpNormal.y == prev.y && tmpNormal.z == prev.z) break;
		prev.x = tmpNormal.x;
		prev.y = tmpNormal.y;
		prev.z = tmpNormal.z;
	}
	
	bucket->planeNormal = tmpNormal;
}

void bucketCut(Bucket* bucket, Bucket* b1, Bucket* b2, int bits) {
	//Spliting bucket in 2
	cvector_vector_type(vec3) colors = bucket->colors;
	size_t colorsLength = cvector_size(colors);
	
	if(colorsLength == 2) {
		//Special case
		bucketAddColor(b1, colors);
		bucketAddColor(b2, colors + 1);
	} else {
		vec3 tmp = (vec3) {0};
		
		for(size_t i=0; i<colorsLength; i++) {
			//Checking whether the color is in front of the section plane, or behind
			vec3Set(&tmp, &(bucket->planeCenter));
			vec3Sub(&tmp, colors + i);
			
			//The dot product of the vector allows you to check the location of the color
			if(vec3Dot(&tmp, &(bucket->planeNormal)) < 0) bucketAddColor(b1, colors + i);
			else bucketAddColor(b2, colors + i);
		}
	}
	
	//Computing data in buckets
	bucketCompute(b1, bits);
	bucketCompute(b2, bits);
}