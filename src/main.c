#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <float.h>
#include <string.h>
#include <time.h>

#include "lodepng.h"
#include "cvector.h"

#include "color.h"
#include "bucket.h"

cvector_vector_type(Color) buildPalette(Color* img, int w, int h, size_t paletteSize,int bits);
cvector_vector_type(Color) bucketsToPalette(cvector_vector_type(Bucket) buckets);

void exportPNG(char* file, Color* img, int w, int h, cvector_vector_type(Color) palette, bool dither, int bits);
size_t findNearestColor(vec3* pixel, cvector_vector_type(vec3) palette);

int main(int argc, char** args) {
	
	if(argc == 1) {
		printf("PlaneCut 1.0\nImage palette generator by Roman Lahin\n");
		
		printf("Enter the paths to the images as arguments.\n");
		printf("Use -p to change palette size. (from 1 to 256 colors) (default is 256)\n");
		printf("Use -dither 0/1 to enable dithering. (default is 0)\n");
		printf("Use -bits RGB to change the number of bits per channel. (default is 888)\n");
		printf("Use -outdir to change output directory.\n");
	}

	size_t paletteSize = 256;
	bool dither = false;
	int bits = 888;
	char* outdir = NULL;

	for(size_t i=1; i<argc; i++) {
		char* fname = *(args + i);
		
		if(*fname == '-') {
			i++;
			if(i >= argc) break;
			
			char* arg = *(args + i);
			
			if(!strcmp(fname, "-p")) {
				paletteSize = atoi(arg);
				
				if(paletteSize <= 0 || paletteSize > 256) {
					printf("Invalid palette size, using 256 colors instead.\n");
					paletteSize = 256;
				}
			} else if(!strcmp(fname, "-dither")) {
				dither = atoi(arg);
			} else if(!strcmp(fname, "-bits")) {
				bits = atoi(arg);
			} else if(!strcmp(fname, "-outdir")) {
				outdir = arg;
			}
		}
	}
	
	for(size_t i=1; i<argc; i++) {
		char* path = *(args + i);
		
		if(*path == '-') {
			i++;
			continue;
		}
		
		unsigned char* image32 = NULL;
		unsigned int w, h;
		
		//Image loading
		int error = lodepng_decode32_file(&image32, &w, &h, path);
		
		if(error) {
			printf("Can't load %s, error %d\n", path, error);
		} else {
			printf("Processing %s...\n", path);
			//RGBA to pixels array
			Color* image = image32;
			
			time_t ms = clock();
			
			//Platte generation
			cvector_vector_type(Color) palette = buildPalette(image, w, h, paletteSize, bits);
			
			printf("Palette generation time: %lu ms\n", (unsigned long) (clock() - ms) * 1000 / CLOCKS_PER_SEC);
			
			char* newfName = NULL;
			
			if(outdir == NULL) {
				//Adding -out.png postfix
				newfName = malloc(strlen(path) + 8);
				strcpy(newfName, path);
				char* pos = strrchr(newfName, '.');
				
				if(pos) strcpy(pos, "-out.png");
				else strcat(newfName, "-out.png");
				
			} else {
				//Adding outdir prefix
				char* fname = strrchr(path, '/');
				if(fname == NULL) fname = strrchr(path, '\\');
				if(fname == NULL) fname = path;
				
				size_t outdir_len = strlen(outdir);
				
				newfName = malloc(outdir_len + 1 + strlen(fname));
				
				strcpy(newfName, outdir);
				if(outdir[outdir_len - 1] != '\\' && outdir[outdir_len - 1] != '/') strcat(newfName, "/");
				
				strcat(newfName, fname);
			}
			
			printf("Export path: %s\n", newfName);
			exportPNG(newfName, image, w, h, palette, dither, bits);
			free(newfName);
		}
		
		free(image32);
	}
	
	return 0;
}

cvector_vector_type(Color) buildPalette(
	Color* img, int w, int h, size_t paletteSize, int bits) {
		
	cvector_vector_type(Bucket) buckets = NULL;
	bool hasAlpha = false;
	
	//Create the first bucket and fill it with colors
	Bucket firstBucket = (Bucket) {0};
	
	for(size_t i=0; i<w*h; i++) {
		//Transparent pixels
		if(img[i].a == 0) {
			if(!hasAlpha) {
				hasAlpha = true;
				paletteSize--;
			}
			continue;
		}
		
		vec3 col = colorToVec3(img + i);
		bucketAddColor(&firstBucket, &col);
	}
	
	//Processing a bucket and adding it to the bucket vector
	if(cvector_size(firstBucket.colors) > 0) {
		bucketCompute(&firstBucket, bits);
		cvector_push_back(buckets, firstBucket);
	}
	
	//Splitting buckets
	while(true) {
		//Checking the number of unique colors
		cvector_vector_type(Color) palette = bucketsToPalette(buckets);
		size_t colorsCount = cvector_size(palette);
		cvector_free(palette);
		
		//Termination of execution if there are enough buckets in the vector
		if(colorsCount >= paletteSize) break;
		
		printf("%lu / %lu done\n", (unsigned long) colorsCount, (unsigned long) paletteSize);
		
		//Search for the bucket with the largest distance between the colors in it and its main color
		int longestBucketId = -1;
		float maxLength = -FLT_MAX;
		
		for(size_t i=0; i<cvector_size(buckets); i++) {
			Bucket* bucket = buckets + i;

			float len = bucket->length;
			
			if(!bucket->dontPick && cvector_size(bucket->colors) > 1 && len > maxLength) {
				maxLength = len;
				longestBucketId = i;
			}
		}
		
		//Can't find a suitable bucket
		if(longestBucketId == -1) break;
		
		//Splitting the selected bucket into 2
		Bucket b1 = (Bucket) {0}, b2 = (Bucket) {0};
		bucketCut(buckets + longestBucketId, &b1, &b2, bits);
		
		//Deleting the current bucket
		bucketFree(buckets + longestBucketId);
		cvector_erase(buckets, longestBucketId);
		
		/*If one of the buckets contains 0 colors, then
		the code cannot split the current bucket into 2, and
		we need to indicate this so that code no longer select this bucket*/
		if(cvector_size(b1.colors) == 0) b2.dontPick = true;
		if(cvector_size(b2.colors) == 0) b1.dontPick = true;
		
		//Adding non-empty buckets to the vector
		if(cvector_size(b1.colors) > 0) cvector_push_back(buckets, b1);
		if(cvector_size(b2.colors) > 0) cvector_push_back(buckets, b2);
	}
	
	//Generating a palette and adding a transparent color
	cvector_vector_type(Color) palette = paletteSize == 0 ? NULL : bucketsToPalette(buckets);
	if(hasAlpha) cvector_push_back(palette, (Color) {0});
	
	//Cleaning
	for(size_t i=0; i<cvector_size(buckets); i++) {
		bucketFree(buckets + i);
	}
	cvector_free(buckets);
	
	return palette;
}

cvector_vector_type(Color) bucketsToPalette(cvector_vector_type(Bucket) buckets) {
	//Creating a vector of non-repeating colors from a vector of buckets
	cvector_vector_type(Color) palette = NULL;
	
	size_t bucketsLength = cvector_size(buckets);
	
	for(size_t i=0; i<bucketsLength; i++) {
		Color* bucketCol = &((buckets + i)->col);
		
		bool found = false;
		
		size_t paletteSize = cvector_size(palette);
		for(size_t t=0; t<paletteSize; t++) {
			
			if(colorEquals(bucketCol, palette + t)) {
				found = true;
				break;
			}
		}
		
		if(!found) cvector_push_back(palette, *bucketCol);
	}
	
	return palette;
}

void exportPNG(char* file, Color* img, int w, int h, cvector_vector_type(Color) palette, bool dither, int bits) {
	//Exporting an image to png
	LodePNGState state = (LodePNGState) {0};
	lodepng_state_init(&state);
	
	//Data initialization
	state.info_png.color.colortype = LCT_PALETTE;
	state.info_png.color.bitdepth = 8;
	
	state.info_raw.colortype = LCT_PALETTE;
	state.info_raw.bitdepth = 8;
	
	state.encoder.auto_convert = false;
	
	//Filling palette
	cvector_vector_type(vec3) vecPalette = NULL;
	
	for(size_t i=0; i<cvector_size(palette); i++) {
		Color col = palette[i];
		
		lodepng_palette_add(&state.info_png.color, col.r, col.g, col.b, col.a);
		lodepng_palette_add(&state.info_raw, col.r, col.g, col.b, col.a);
		
		if(col.a != 0) cvector_push_back(vecPalette, colorToVec3(&col));
	}
	
	//We dont change original image to prevent dithering leaking
	vec3* ditherDiff;
	if(dither) {
		//2 lines of pixels are enough
		ditherDiff = calloc(w * 2, sizeof(vec3));
	}
	
	//Search for the nearest colors in the palette
	unsigned char* indices = malloc(w * h);
	
	for(size_t y=0; y<h; y++) {
		for(size_t x=0; x<w; x++) {
			
			Color colPixel = img[x + y*w];
			vec3 origPixel = colorToVec3(img + x + y*w);
			vec3 pixel = origPixel;
			
			//Add dither difference
			if(dither) vec3Add(&pixel, ditherDiff + x);
			
			unsigned char nearestColorIndex = cvector_size(palette) - 1; //special case for transparent pixels
			if(colPixel.a != 0) nearestColorIndex = findNearestColor(&pixel, vecPalette);
			
			indices[x + y*w] = nearestColorIndex;
			
			if(dither && palette[nearestColorIndex].a != 0) {
				//Floyd–Steinberg dithering
				vec3 nearestColor = vecPalette[nearestColorIndex];
				
				vec3 quantError = (vec3) {
					origPixel.x - nearestColor.x, 
					origPixel.y - nearestColor.y, 
					origPixel.z - nearestColor.z
				};
				
				const float errorSpread = 0.75;
				vec3Mulv(&quantError, errorSpread);
				
				if(x <= w-2) {
					vec3 addVec = quantError;
					vec3Mulv(&addVec, 6 / 16.0);
					vec3Addr(ditherDiff + x+1, &addVec);
					
					addVec = quantError; 
					vec3Mulv(&addVec, 1 / 16.0);
					if(y <= h-2) vec3Addr(ditherDiff + x+1 + w, &addVec);
				}
				
				if(y <= h-2) {
					vec3 addVec = quantError;
					vec3Mulv(&addVec, 4 / 16.0);
					if(x >= 1) vec3Addr(ditherDiff + x-1 + w, &addVec);
					
					addVec = quantError;
					vec3Mulv(&addVec, 5 / 16.0);
					vec3Addr(ditherDiff + x + w, &addVec);
				}
			}
		}
		
		//Move dither difference one line up
		if(dither) {
			memcpy(ditherDiff, ditherDiff + w, sizeof(vec3) * w);
			memset(ditherDiff + w, 0, sizeof(vec3) * w);
		}
	}
	
	//Export
	unsigned char* buffer = NULL;
	size_t bufferSize = 0;
	
	//unsigned int error = lodepng_encode32_file(file, imgTest, w, h);
	unsigned int error = lodepng_encode(&buffer, &bufferSize, indices, w, h, &state);
	
	if(!error) {
		error = lodepng_save_file(buffer, bufferSize, file);
		if(error) printf("lodepng save file error %u: %s\n", error, lodepng_error_text(error));
		
	} else printf("lodepng encode error %u: %s\n", error, lodepng_error_text(error));
	
	lodepng_state_cleanup(&state);
	free(buffer);
	free(indices);
}

size_t findNearestColor(vec3* pixel, cvector_vector_type(vec3) palette) {
	//Search for the nearest color in the palette
	float minDist = FLT_MAX;
	size_t closest = 0;
	
	for(size_t i=0; i<cvector_size(palette); i++) {
		float dist = vec3ColorDistance(pixel, palette + i);
		
		if(dist < minDist) {
			minDist = dist;
			closest = i;
		}
	}
	
	return closest;
}