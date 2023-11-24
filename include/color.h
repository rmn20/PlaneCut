#ifndef COLOR_H
#define COLOR_H

#include <stdbool.h>

#include "vec3.h"

typedef struct {
	unsigned char r, g, b, a;
} Color;

bool colorEquals(Color* c1, Color* c2);

vec3 colorToVec3(Color* c);
Color vec3ToColor(vec3* v, int bits);

#endif