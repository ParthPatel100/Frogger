#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include "Mario.h"
#include "MenuScreen.h"
#include "colors.h"
#include <wiringPi.h>
#include "initGPIO.h"
#include "SNES.h"
#include "johnnyG.h"

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

void drawMainMenu(Pixel *pixel){
	int *menuPtr = (int *) MenuScreen.image_pixels;
	int i = 0;
for (int y = 0; y < 800; y++)
		{
			for (int x = 0; x < 800; x++) 
			{	 
				pixel->color = menuPtr[i];
				i++;
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
			}
		}
}

void drawColor(Pixel *pixel){
	int *col = (int *) johnnyG.image_pixels;
	int i = 0;
for (int y = 0; y < 450; y++)
		{
			for (int x = 0; x < 450; x++) 
			{	 
				pixel->color = col[i];
				i++;
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
		
			i++;
			if (x>=0) {
				pixel->color = alienPtr[i];
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
			}
		}
	}
		//~ delayMicroseconds(100000);
}

void underlineStart(Pixel *pixel)
{
	int y = 540;
	int startingX = 78;
	int endingX = 350;
	for (int x = startingX; x <= endingX; x++)
	{
		pixel->color = 0xF;
		pixel->x = x;
		pixel->y = y;
		drawPixel(pixel);
	}
	
	for (int x = 455; x <= 715; x++)
	{
		pixel->color = 0xE9967A;
		pixel->x = x;
		pixel->y = y;
		drawPixel(pixel);
	}
}

void underlineQuit(Pixel *pixel)
{
	int y = 540;
	int startingX = 455;
	int endingX = 715;
	for (int x = startingX; x <= endingX; x++)
	{
		pixel->color = 0xF;
		pixel->x = x;
		pixel->y = y;
		drawPixel(pixel);
	}
	
	for (int x = 78; x <= 350; x++)
	{
		pixel->color = 0xE9967A;
		pixel->x = x;
		pixel->y = y;
		drawPixel(pixel);
	}
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
	//char * string[] = {"B", "Y", "Select", "Start", "Joy-Pad UP", "Joy-Pad DOWN", "Joy-Pad LEFT", "Joy-Pad RIGHT", "A", "X", "L", "R", "?", "?", "?", "?", "?" };
	int currentCode = 6;
	int buttonCode = 0;
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	/* initialize a pixel */
	Pixel *pixel;
	
	pixel = malloc(sizeof(Pixel));
	drawColor(pixel);
	//drawMainMenu(pixel);
	//underlineStart(pixel);
	//underlineQuit(pixel);
	
	//~ do
	//~ {
		
		//~ buttonCode = getButtonPressed(currentCode);
		
		
		//~ if (buttonCode == 6)
		//~ {
			//~ underlineStart(pixel);
		//~ }
		//~ else if (buttonCode == 7)
		//~ {
			//~ underlineQuit(pixel);
		//~ }
		
		//~ currentCode = buttonCode;
		
		
	//~ }while (buttonCode != 8);

	
	while(0){
	
	int startingPoint = -30;
	while(startingPoint <= 600){
		drawAlien(pixel, startingPoint);
		if(startingPoint >= 0){
			cleanBackground(pixel, startingPoint);
		}
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




