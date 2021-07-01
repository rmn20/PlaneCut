#include <stdio.h>

#include "bucket.h"

void bucketFree(Bucket* bucket) {
	cvector_free(bucket->colors);
}

void bucketAddColor(Bucket* bucket, vec3* col) {
	//Добавление цвета в вектор цветов
	cvector_push_back(bucket->colors, *col);
	//Добавление цвета к центральной точке плоскости сечения
	//(потом будет произведено деление на кол-во цветов)
	vec3Add(&(bucket->planeCenter), col);
}

size_t min(size_t a, size_t b) {
	return a < b ? a : b;
}

size_t max(size_t a, size_t b) {
	return a > b ? a : b;
}

void bucketCompute(Bucket* bucket, int bits) {
	//Функция подсчитывания данных ведра
	cvector_vector_type(vec3) colors = bucket->colors;
	size_t colorsLength = cvector_size(colors);
	
	//Нечего подсчитывать
	if(colorsLength == 0) return;
	
	//Делим сумму цветов на их количество
	//При помощи среднего арифметического получаем
	//центральную точку плоскости сечения и цвет ведра
	vec3Divv(&(bucket->planeCenter), colorsLength);
	bucket->col = vec3ToColor(&(bucket->planeCenter), bits);
	
	//Расстояние от цвета ведра до цветов внутри ведра
	bucket->length = 0;
	for(size_t i=0; i<colorsLength; i++) {
		bucket->length += vec3ColorDistance(colors + i, &(bucket->planeCenter));
	}
	
	//Особые случаи
	if(colorsLength == 2) {
		vec3 tmp = *(colors + 1);
		vec3Sub(&tmp, colors);
		
		bucket->planeNormal = tmp;
		
		return;
	} else if(colorsLength == 1) return;
	
	//Подсчитывание нормали плоскости сечения
	//Основано на zalo.github.ioblog/line-fitting
	vec3 tmpNormal = (vec3) {0.5, 0.5, 0.5};
	vec3Normalize(&tmpNormal);
	vec3 prev = tmpNormal;
	vec3 pc = bucket->planeCenter;
	
	//Я не использую в цикле векторые функции, поскольку они сильно замедляют выполнение программы
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
		
		//Если текущая нормаль совпадает с прошлой, то нет смысла продолжать выполнение
		//Эта проверка повышает скорость выполнения почти в 2 раза
		if(tmpNormal.x == prev.x && tmpNormal.y == prev.y && tmpNormal.z == prev.z) break;
		prev.x = tmpNormal.x;
		prev.y = tmpNormal.y;
		prev.z = tmpNormal.z;
	}
	
	bucket->planeNormal = tmpNormal;
}

void bucketCut(Bucket* bucket, Bucket* b1, Bucket* b2, int bits) {
	//Разделение ведра на 2 штуки
	cvector_vector_type(vec3) colors = bucket->colors;
	size_t colorsLength = cvector_size(colors);
	
	if(colorsLength == 2) {
		//Особый случай
		bucketAddColor(b1, colors);
		bucketAddColor(b2, colors + 1);
	} else {
		vec3 tmp = (vec3) {0};
		
		for(size_t i=0; i<colorsLength; i++) {
			//Проверка, находится ли цвет перед плоскостью сечения, или сзади
			//Составляем вектор направления от центра плоскости сечения до цвета
			vec3Set(&tmp, &(bucket->planeCenter));
			vec3Sub(&tmp, colors + i);
			
			//Скалярное произведение вектора позволяет проверить расположение цвета
			if(vec3Dot(&tmp, &(bucket->planeNormal)) < 0) bucketAddColor(b1, colors + i);
			else bucketAddColor(b2, colors + i);
		}
	}
	
	//Подсчитывание данных в вёдрах
	bucketCompute(b1, bits);
	bucketCompute(b2, bits);
}