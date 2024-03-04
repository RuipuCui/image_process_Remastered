#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "imageProcess.h"

int main(int argc, char* argv[]){
    FILE* input = fopen(argv[1],"rb");
    FILE* output = fopen(argv[2], "wb");
    image_t* image = (image_t*)malloc(sizeof(image_t));
    readBMP(input, image);
    if(strcmp(argv[3], "copy") == 0){
    }else if(strcmp(argv[3], "blur") == 0){
        blurImage(image);
    }else if(strcmp(argv[3], "brighter") == 0){
        int factor = atoi(argv[4]);
        brightImage(image, factor);
    }else if(strcmp(argv[3], "gray") == 0){
        colourToGray(image);
    }else if(strcmp(argv[3], "black_white") == 0){
        black_white(image);
    }else if(strcmp(argv[3], "sepiaFilter") == 0){
        sepiaFilter(image);
    }

    printImage(output, image);
    freeImage(image);
    fclose(input);
    fclose(output);
}