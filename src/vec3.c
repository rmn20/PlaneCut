#include <math.h>

#include "vec3.h"

void vec3Set(vec3* a, vec3* b) {
	a->x = b->x;
	a->y = b->y;
	a->z = b->z;
}

void vec3Setv3(vec3* a, float x, float y, float z) {
	a->x = x;
	a->y = y;
	a->z = z;
}

void vec3Add(vec3* a, vec3* b) {
	a->x += b->x;
	a->y += b->y;
	a->z += b->z;
}

void vec3Addv(vec3* a, float v) {
	a->x += v;
	a->y += v;
	a->z += v;
}

void vec3Sub(vec3* a, vec3* b) {
	a->x -= b->x;
	a->y -= b->y;
	a->z -= b->z;
}

void vec3Subv(vec3* a, float v) {
	a->x -= v;
	a->y -= v;
	a->z -= v;
}

void vec3Mul(vec3* a, vec3* b) {
	a->x *= b->x;
	a->y *= b->y;
	a->z *= b->z;
}

void vec3Mulv(vec3* a, float v) {
	a->x *= v;
	a->y *= v;
	a->z *= v;
}

void vec3Div(vec3* a, vec3* b) {
	a->x /= b->x;
	a->y /= b->y;
	a->z /= b->z;
}

void vec3Divv(vec3* a, float v) {
	a->x /= v;
	a->y /= v;
	a->z /= v;
}

void vec3SetLength(vec3* a, float nl) {
	float mul = nl / vec3Length(a);
	
	a->x *= mul;
	a->y *= mul;
	a->z *= mul;
}

void vec3Normalize(vec3* a) {
	float l = vec3Length(a);
	if(l == 0) return;
	
	a->x /= l;
	a->y /= l;
	a->z /= l;
}

float vec3Dot(vec3* a, vec3* b) {
	//dot product
	return a->x * b->x + a->y * b->y + a->z * b->z;
}

float vec3Distance(vec3* a, vec3* b) {
	return sqrtf(vec3DistanceSqr(a, b));
}

float vec3DistanceSqr(vec3* a, vec3* b) {
	return 
		(a->x - b->x)*(a->x - b->x) +
		(a->y - b->y)*(a->y - b->y) +
		(a->z - b->z)*(a->z - b->z);
}

float vec3Length(vec3* a) {
	return sqrtf(vec3Dot(a, a));
}

//Redmean from wikipedia
float vec3ColorDistance(vec3* c1, vec3* c2) {
	
	//return vec3DistanceSqr(c1, c2);
	float r = (c1->x + c2->x) / 2;
	
	float C = (2 + r) * (c1->x - c2->x)*(c1->x - c2->x) +
		4 * (c1->y - c2->y)*(c1->y - c2->y) +
		(2 + (1 - r)) * (c1->z - c2->z)*(c1->z - c2->z);
	
	return C;
}