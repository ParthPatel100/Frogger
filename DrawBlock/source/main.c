#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include "Mario.h"
#include <unistd.h>
#include <wiringPi.h>

/* Definitions */
typedef struct {
	
	int color;
	int x, y;
} Pixel;

struct fbs framebufferstruct;
void drawPixel(Pixel *pixel);


/* Draw a pixel */
void drawPixel(Pixel *pixel){
	long int location =(pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	*((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
}

void drawBackground(Pixel *pixel){
	for (int y = 0; y < 600; y++)
		{
			for (int x = 0; x < 600; x++) 
			{	 
				pixel->color = 0xF;
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
			}
		}
}

void drawAlien(Pixel *pixel, int startingPoint){
	int *alienPtr=(int *) Mario.image_pixels;
	int i =0;
	for (int y = 0; y < 30; y++)
	{
		for (int x = startingPoint; x < (30 + startingPoint); x++) 
		{	 
			pixel->color = alienPtr[i];
			pixel->x = x;
			pixel->y = y;
			i++;
			drawPixel(pixel);
		}
	}
		//~ delayMicroseconds(100000);
}

void cleanBackground(Pixel *pixel, int startingPoint){
	for(int y = 0; y < 30; y++){
		pixel->color = 0xF;
		pixel->x = startingPoint;
		pixel->y = y;
		drawPixel(pixel);
	}
}

/* main function */
int main(){


	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	

	
	/* initialize a pixel */
	Pixel *pixel;
	

	pixel = malloc(sizeof(Pixel));
	while(1){
	drawBackground(pixel);
	int startingPoint = 0;
	while(startingPoint <= 570){
		drawAlien(pixel, startingPoint);
		cleanBackground(pixel, startingPoint);
		startingPoint++;
		delayMicroseconds(18000);
	}
}
			/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	return 0;
}




