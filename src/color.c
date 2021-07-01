#include <math.h>
#include <float.h>

#include "color.h"

int clamp(int x, int min, int max) {
	if(x < min) return min;
	if(x > max) return max;
	return x;
}

//Redmean from wikipedia
float colorDistance(Color* c1, Color* c2) {
	//Прозрачность - особый случай
	if(c1->a == 0 || c2->a == 0) {
		if(c1->a == c2->a) return 0;
		else return FLT_MAX;
	}
	
	/*return (c1->r - c2->r) * (c1->r - c2->r) +
		(c1->g - c2->g) * (c1->g - c2->g) +
		(c1->b - c2->b) * (c1->b - c2->b);*/
	float r = (c1->r + c2->r) / 2;
	
	float C = (2 + r / 256) * (c1->r - c2->r) * (c1->r - c2->r) +
		4 * (c1->g - c2->g) * (c1->g - c2->g) +
		(2 + (255-r) / 256) * (c1->b - c2->b) * (c1->b - c2->b);
	
	return C;
}

bool colorEquals(Color* c1, Color* c2) {
	return c1->r == c2->r && c1->g == c2->g && c1->b == c2->b && c1->a && c2->a;
}

vec3 colorToVec3(Color* x) {
	return (vec3) {x->r / 255.0, x->g / 255.0, x->b / 255.0};
}

void colorAdd(Color* c, int* add, float power) {
	c->r = (unsigned char) clamp(c->r + add[0] * power, 0, 255);
	c->g = (unsigned char) clamp(c->g + add[1] * power, 0, 255);
	c->b = (unsigned char) clamp(c->b + add[2] * power, 0, 255);
}

Color vec3ToColor(vec3* v, int bits) {
	int rBits = (bits / 100) % 10,
		gBits = (bits / 10) % 10,
		bBits = bits % 10;

	int r = clamp((int) roundf(v->x * 255), 0, 255) >> (8 - rBits),
		g = clamp((int) roundf(v->y * 255), 0, 255) >> (8 - gBits),
		b = clamp((int) roundf(v->z * 255), 0, 255) >> (8 - bBits);
	
	r = (r << (8 - rBits)) | (r >> (rBits - (8 - rBits)));
	g = (g << (8 - gBits)) | (g >> (gBits - (8 - gBits)));
	b = (b << (8 - bBits)) | (b >> (bBits - (8 - bBits)));
	
	return (Color) {
		(unsigned char) r,
		(unsigned char) g,
		(unsigned char) b,
		255
	};
}