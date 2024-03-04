#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "imageProcess.h"

void readBMP(FILE* f, image_t* image) {

    if(fread(image->header, sizeof(unsigned char), HEADER_SIZE, f) != HEADER_SIZE){
        fprintf(stderr, "Error: Unable to read image header.\n");
        exit(EXIT_FAILURE);
    }

    image->width = *(int*)&image->header[18];
    image->height = *(int*)&image->header[22];
    image->bitDepth = *(int*)&image->header[28];

    if(image->bitDepth != 8 && image->bitDepth != 24){
        printf("only accept 8 or 24 bitdepth bmp image");
        fclose(f);
        return;
    }

    if(image->bitDepth == 8){
        image->colourTable = (unsigned char*)malloc(sizeof(unsigned char) * COLOURTABLE_SIZE);
        assert(image->colourTable);
        image->grayPixel = (unsigned char*)malloc(sizeof(unsigned char) * image->width * image->height);
        assert(image->grayPixel);
        if(fread(image->colourTable, sizeof(unsigned char), COLOURTABLE_SIZE, f) != COLOURTABLE_SIZE){
            fprintf(stderr, "Error: Unable to read colourTable\n");
            exit(EXIT_FAILURE);    
        }
        if(fread(image->grayPixel, sizeof(unsigned char), image->width * image->height, f) != image->width * image->height){
            fprintf(stderr, "Error: Unable to read pixel file\n");
            exit(EXIT_FAILURE); 
        }
        fclose(f);
    }

    if(image->bitDepth == 24){
        image->colourPixel = (unsigned char**)malloc(sizeof(unsigned char*) * image->width * image->height);
        assert(image->colourPixel);
        int i;
        for(i=0; i < image->width * image->height; i++){
            image->colourPixel[i] = (unsigned char*)malloc(sizeof(unsigned char) * 3);
            assert(image->colourPixel[i]);
            if(fread(image->colourPixel[i], sizeof(unsigned char), 3, f) != 3){
                fprintf(stderr, "Error: Unable to read pixel file\n");
                exit(EXIT_FAILURE); 
            }
        }
    }

    return;
}

void printImage(FILE* f, image_t* image){

    if(image->bitDepth == 8){
        fwrite(image->header, sizeof(unsigned char), HEADER_SIZE, f);
        fwrite(image->colourTable, sizeof(unsigned char), COLOURTABLE_SIZE, f);
        fwrite(image->grayPixel, sizeof(unsigned char), image->height * image->width, f);
    }else if(image->bitDepth == 24){
        fwrite(image->header, sizeof(unsigned char), HEADER_SIZE, f);
        int i;
        for(i = 0; i < image->height * image->width; i++){
            fwrite(image->colourPixel[i], sizeof(unsigned char), 3, f);
        }
    }

}

void freeImage(image_t* image){

    if(image->bitDepth == 8){
        free(image->grayPixel);
    }else if(image->bitDepth == 24){
        int i;
        for(i = 0; i < image->height * image->width; i++){
            free(image->colourPixel[i]);
        }
        free(image->colourPixel);
    }

    free(image);
}

void blurImage(image_t* image){
    float v=1.0 / 9.0;											
	float kernel[3][3]={{v,v,v},
						{v,v,v},
						{v,v,v}};
    int size = image->width * image->height;
    if(image->bitDepth == 8){
        unsigned char buffer[size];
        int i;
        for(i = 0; i < size; i++){
            buffer[i] = image->grayPixel[i];
        }

        int x, y;
        int m, n;
        for(x = 1; x < image->height - 1; x++){
            for(y = 1; y < image->width - 1; y++){             
                float sum= 0.0;
                for(m = -1; m <= 1; m++){
                    for(n = -1; n <= 1; n++){
                        sum=sum+(float)kernel[m+1][n+1]*buffer[(x+m)*image->width+(y+n)];
                    }
                }
                image->grayPixel[(x)*image->width+(y)] = sum;

            }
        }
        
    }else if(image->bitDepth == 24){
        unsigned char buffer[size][3];
        int i;
        for(i = 0; i < size; i++){
            buffer[i][0] = image->colourPixel[i][0];
            buffer[i][1] = image->colourPixel[i][1];
            buffer[i][2] = image->colourPixel[i][2];
        }

        int x, y;
        int m, n;
        for(x = 1; x < image->height - 1; x++){
            for(y = 1; y < image->width - 1; y++){             
                float sum0 = 0.0;
                float sum1 = 0.0;
                float sum2 = 0.0;
                for(m = -1; m <= 1; m++){
                    for(n = -1; n <= 1; n++){
                        sum0=sum0+(float)kernel[m+1][n+1]*buffer[(x+m)*image->width+(y+n)][0];
					    sum1=sum1+(float)kernel[m+1][n+1]*buffer[(x+m)*image->width+(y+n)][1];
					    sum2=sum2+(float)kernel[m+1][n+1]*buffer[(x+m)*image->width+(y+n)][2];
                    }
                }
                image->colourPixel[(x)*image->width+(y)][0]=sum0;
			    image->colourPixel[(x)*image->width+(y)][1]=sum1;
			    image->colourPixel[(x)*image->width+(y)][2]=sum2;

            }
        }
    }
}

void brightImage(image_t* image, int factor){
    if(image->bitDepth == 8){
        int i;
        for(i = 0; i < image->width * image->height; i++){
            image->grayPixel[i] = image->grayPixel[i] + factor;
            if(image->grayPixel[i] > MAX_VALUE){
                image->grayPixel[i] = MAX_VALUE;
            }
        }
    }else if(image->bitDepth == 24){
        int i;
        for(i = 0; i < image->width * image->height; i++){
            image->colourPixel[i][0] = (image->colourPixel[i][0] + factor > MAX_VALUE) ? MAX_VALUE : image->colourPixel[i][0] + factor;
            image->colourPixel[i][1] = (image->colourPixel[i][1] + factor > MAX_VALUE) ? MAX_VALUE : image->colourPixel[i][1] + factor;
            image->colourPixel[i][2] = (image->colourPixel[i][2] + factor > MAX_VALUE) ? MAX_VALUE : image->colourPixel[i][2] + factor;
        }

    }

}

void colourToGray(image_t* image){
    if(image->bitDepth == 8){
        fprintf(stderr, "already be a gray scale image.\n");
        exit(EXIT_FAILURE);
    }

    int i;
    int size = image->height * image->width;
    for(i = 0; i < size; i++){
        int grayValue = image->colourPixel[i][2] * 0.3 + image->colourPixel[i][1] * 0.59 + image->colourPixel[i][0] * 0.11;
        image->colourPixel[i][0] = grayValue;
        image->colourPixel[i][1] = grayValue;
        image->colourPixel[i][2] = grayValue;
    }

}

void black_white(image_t* image){
    int size = image->width * image->height;
    if(image->bitDepth == 8){
        int i;
        for(i = 0; i < size; i++){
            if(image->grayPixel[i] > GRAYTHRESHOLD){
                image->grayPixel[i] = WHITE;
            }else{
                image->grayPixel[i] = BLACK;
            }
        }
    }else if(image->bitDepth == 24){
        int i;
        for(i = 0; i < size ; i++){
            int darkness = (image->colourPixel[i][0] + image->colourPixel[i][1] + image->colourPixel[i][2]) / 3;
            if(darkness > COLOURTHRESHOLD){
                image->colourPixel[i][0] = WHITE;
                image->colourPixel[i][1] = WHITE;
                image->colourPixel[i][2] = WHITE;
            }else{
                image->colourPixel[i][0] = BLACK;
                image->colourPixel[i][1] = BLACK;
                image->colourPixel[i][2] = BLACK;
            }
        }
    }

}

void sepiaFilter(image_t* image){
    if(image->bitDepth == 8){
        fprintf(stderr, "sepia filter can only apply on gray scale image (bitdepth == 8).\n");
        exit(EXIT_FAILURE);
    }else if(image->bitDepth == 24){
        int size = image->width * image->height;
        int i, r, g, b;
        for(i = 0; i < size; i++){
            b = (image->colourPixel[i][0]*0.393) + (image->colourPixel[i][1]*0.769)	+ (image->colourPixel[i][2]*0.189);
		    g = (image->colourPixel[i][0]*0.349) + (image->colourPixel[i][1]*0.686)	+ (image->colourPixel[i][2]*0.168);
		    r = (image->colourPixel[i][0]*0.272) + (image->colourPixel[i][1]*0.534)	+ (image->colourPixel[i][2]*0.131);
            if(r > MAX_VALUE){											
			    r = MAX_VALUE;
		    }
		    if(g > MAX_VALUE){
			    g = MAX_VALUE;
		    }
		    if(b > MAX_VALUE){
			    b = MAX_VALUE;
		    }
            image->colourPixel[i][0] = r;
            image->colourPixel[i][1] = g;
            image->colourPixel[i][2] = b;
        }
    }
}