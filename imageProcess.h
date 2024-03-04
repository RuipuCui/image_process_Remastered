#ifndef _IMAGEPROCESS_H_
#define _IMAGEPROCESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_VALUE 255
#define HEADER_SIZE 54
#define COLOURTABLE_SIZE 1024
#define GRAYTHRESHOLD 128
#define COLOURTHRESHOLD 100
#define WHITE 255
#define BLACK 0

struct image {
    unsigned char header[HEADER_SIZE];
    unsigned char* colourTable;
    unsigned char* grayPixel;
    unsigned char** colourPixel;
    int bitDepth;
    int height;
    int width;
};

typedef struct image image_t;

void readBMP(FILE* f, image_t* image);
void printImage(FILE* f, image_t* image);
void freeImage(image_t* image);
void blurImage(image_t* image);
void brightImage(image_t* image, int factor);
void colourToGray(image_t* image);
void black_white(image_t* image);
void sepiaFilter(image_t* image);
void getInformation(image_t* image, FILE* f);

#endif