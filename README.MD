#basic image processing library writen in C 
#followed the instruction on https://abhijitnathwani.github.io/blog/
#rebuild the project 
#supported functions: copy, blur, brighter, darker, change colour image to gray scale, convert image to black and white and apply sepia filter
#commands need to run the project: 
    1. make
    2../dict1 <path/to/input/file> <path/to/output/file> <functionkeyword> <factorifneeded>
#function keywords: copy, blue, bright, gray, black_white, sepiaFilter
#note: for bright function, the command need to followed a brightness factor(can be negative if you want to make the image darker).
#command examples:
    ./dict1 images/lena_color.bmp images/lena_sepia.bmp sepiaFilter
    ./dict1 images/lena_color.bmp images/lena_bright.bmp bright 25

#update 4 Mar 2024
    new function: get information of the bmp image
    keyword: getInfo