#ifndef VEC3_H
#define VEC3_H

typedef struct {
	float x, y, z;
} vec3;

void vec3Set(vec3* a, vec3* b);
void vec3Setv3(vec3* a, float x, float y, float z);

void vec3Add(vec3* a, vec3* b);
void vec3Addv(vec3* a, float v);

void vec3Sub(vec3* a, vec3* b);
void vec3Subv(vec3* a, float v);

void vec3Mul(vec3* a, vec3* b);
void vec3Mulv(vec3* a, float v);

void vec3Div(vec3* a, vec3* b);
void vec3Divv(vec3* a, float v);

void vec3SetLength(vec3* a, float nl);
void vec3Normalize(vec3* a);

float vec3Dot(vec3* a, vec3* b);
float vec3Distance(vec3* a, vec3* b);
float vec3DistanceSqr(vec3* a, vec3* b);
float vec3Length(vec3* a);

float vec3ColorDistance(vec3* a, vec3* b);

#endif