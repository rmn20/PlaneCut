#include <math.h>
#include <float.h>

#include "color.h"

int clamp(int x, int min, int max) {
	if(x < min) return min;
	if(x > max) return max;
	return x;
}

bool colorEquals(Color* c1, Color* c2) {
	return c1->r == c2->r && c1->g == c2->g && c1->b == c2->b && c1->a && c2->a;
}

inline float sRGBtoLinear(float x) {
    if(x >= 0.04045) return pow((x + 0.055)/(1 + 0.055), 2.4);
    else return x / 12.92;
}

vec3 colorToVec3(Color* x) {
	//Srgb to linear rgb
	vec3 c = (vec3) {
		sRGBtoLinear(x->r / 255.0),
		sRGBtoLinear(x->g / 255.0),
		sRGBtoLinear(x->b / 255.0)
	};
	
	//to oklab
	float l = 0.4122214708f * c.x + 0.5363325363f * c.y + 0.0514459929f * c.z;
	float m = 0.2119034982f * c.x + 0.6806995451f * c.y + 0.1073969566f * c.z;
	float s = 0.0883024619f * c.x + 0.2817188376f * c.y + 0.6299787005f * c.z;

    float l_ = cbrtf(l);
    float m_ = cbrtf(m);
    float s_ = cbrtf(s);

    vec3 out = (vec3) {
        (0.2104542553f*l_ + 0.7936177850f*m_ - 0.0040720468f*s_) * 1,
        1.9779984951f*l_ - 2.4285922050f*m_ + 0.4505937099f*s_,
        0.0259040371f*l_ + 0.7827717662f*m_ - 0.8086757660f*s_,
    };
	
	return (vec3) {out.x, out.y, out.z};
}

inline float linearToSRGB(float x) {
    if(x >= 0.0031308) return (1.055) * pow(x, 1.0/2.4) - 0.055;
    else return 12.92 * x;
}

Color vec3ToColor(vec3* v, int bits) {
	//oklab to linear rgb
	float l_ = v->x / 1 + 0.3963377774f * v->y + 0.2158037573f * v->z;
    float m_ = v->x / 1 - 0.1055613458f * v->y - 0.0638541728f * v->z;
    float s_ = v->x / 1 - 0.0894841775f * v->y - 1.2914855480f * v->z;

    float l = l_*l_*l_;
    float m = m_*m_*m_;
    float s = s_*s_*s_;

    vec3 fltCol = (vec3) {
		+4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
		-1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
		-0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
    };
	
	//to srgb
	fltCol.x = linearToSRGB(fltCol.x);
	fltCol.y = linearToSRGB(fltCol.y);
	fltCol.z = linearToSRGB(fltCol.z);
	
	int rBits = (bits / 100) % 10,
		gBits = (bits / 10) % 10,
		bBits = bits % 10;
	
	int rMax = 255 >> (8 - rBits),
		gMax = 255 >> (8 - gBits),
		bMax = 255 >> (8 - bBits);

	int r = clamp((int) roundf(fltCol.x * rMax), 0, rMax),
		g = clamp((int) roundf(fltCol.y * gMax), 0, gMax),
		b = clamp((int) roundf(fltCol.z * bMax), 0, bMax);
	
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