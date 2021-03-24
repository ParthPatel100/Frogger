#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include "Mario.h"
#include "MainMenu.h"
#include <wiringPi.h>
#include <stdbool.h>
#include "SNES.h"

#define UP 4
#define RIGHT 7
#define DOWN 5
#define LEFT 6

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

void drawBackground(Pixel *pixel, int maxScreenX, int maxScreenY){
	for (int y = 0; y < maxScreenY; y++)
	{
		for (int x = 0; x < maxScreenX; x++) 
		{	 
			pixel->color = 0xF;
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
		}
	}
}

void drawMainMenu(Pixel *pixel){
	int *menuPtr = (int *) MainMenu.image_pixels;
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

int getDirectionFromCon(int* frogLane, int* frogStart, int currentCode){
	int buttonCode = getButtonPressed(currentCode);
	//~ printf("lane %d\n", *frogLane);
	if (buttonCode == 5){
		*frogLane += 1;
	}
	if (buttonCode == 4){
		*frogLane -= 1;
	}
	//~ printf("lane 2 %d\n", *frogLane);
	return buttonCode;

}

void cleanBackground(Pixel *pixel, int xStart, int yStart, int xEnd, int yEnd){
	for(int x = xStart; x <= xEnd; x++){
		for(int y = yStart; y <= yEnd; y++){
			if(x >= 0 && x< 800 && x < 800 && y >= 0){
				pixel->x = x;
				pixel->y = y;
				pixel->color = 0xF;
				drawPixel(pixel);
			}
		}
	}
}

int drawFrog(Pixel *pixel, int xStartingPoint, int maxScreenX, int frogLane, int direction){
	int *alienPtr=(int *) Mario.image_pixels;
	int i =0;
	int x, y;
	int size = 30;
	int xCordCleanStart;
	int xCordCleanEnd;
	int yCordCleanStart;
	int yCordCleanEnd;
	for (y = size*frogLane; y < size + (size*frogLane); y++)
	{
		for ( x = xStartingPoint; x < (30 + xStartingPoint); x++) 
		{	
			i++;
			if (x>=0 && x< maxScreenX) {
				pixel->color = alienPtr[i];
				pixel->x = x;
				pixel->y = y;
				if(x <= maxScreenX){
					drawPixel(pixel);
				}
			}
		}
	}
	if (direction != -1){
		if (direction == RIGHT){
			xCordCleanStart = xStartingPoint-1;
			xCordCleanEnd = xStartingPoint-1;
			yCordCleanStart = size*frogLane;
			yCordCleanEnd = y - 1;
		}
		else if (direction == LEFT){
			xCordCleanStart = x;
			xCordCleanEnd = x;
			yCordCleanStart = size*frogLane;
			yCordCleanEnd = y - 1;
		}
		else if(direction == UP){
			xCordCleanStart = xStartingPoint;
			xCordCleanEnd = x - 1;
			yCordCleanStart = size + size*frogLane;
			yCordCleanEnd = size + yCordCleanStart;	
		}
		else if(direction == DOWN){
			//~ printf("here");
			xCordCleanStart = xStartingPoint;
			xCordCleanEnd = x - 1;
			yCordCleanStart = (size*frogLane) - size;
			yCordCleanEnd = size*frogLane - 1;
		}
		cleanBackground(pixel, xCordCleanStart, yCordCleanStart, xCordCleanEnd, yCordCleanEnd);
	}
	if (x>=0){
		return xStartingPoint;
	}
	else{
		return 0;
	}
	//~ delayMicroseconds(100000);
}

int drawAlien(Pixel *pixel, int startingPoint, int maxScreenX, int objectLane, int direction){
	int *alienPtr=(int *) Mario.image_pixels;
	int i =0;
	int x, y;
	int size = 30;
	int xCordCleanStart;
	int xCordCleanEnd;
	int yCordCleanStart;
	int yCordCleanEnd;
	for (y = size*objectLane; y < size + (size*objectLane); y++)
	{
		for (x = startingPoint; x < (30 + startingPoint); x++) 
		{	
			i++;
			if (x>=0 && x< maxScreenX) {
				pixel->color = alienPtr[i];
				pixel->x = x;
				pixel->y = y;
				if(x <= maxScreenX){
					drawPixel(pixel);
				}
			}
		}
	}


	if (direction == RIGHT){
		xCordCleanStart = startingPoint - 1;
		xCordCleanEnd = startingPoint - 1;
		yCordCleanStart = size*objectLane;
		yCordCleanEnd = y - 1;
	}
	else if (direction == LEFT){
		xCordCleanStart = x;
		xCordCleanEnd = x ;
		yCordCleanStart = size*objectLane;
		yCordCleanEnd = y - 1;
	}
	cleanBackground(pixel, xCordCleanStart, yCordCleanStart, xCordCleanEnd, yCordCleanEnd);
	if (x>=0){
		return startingPoint;
	}
	else{
		return 0;
	}
	//~ delayMicroseconds(100000);
}



bool checkCollision(Pixel *pixel, int frogPosition, int objectPosition, int frogLane, int objectLane){
	if (frogLane == objectLane){
		int locationDifference = frogPosition - objectPosition;
		if (locationDifference <= 30 && locationDifference >= -30){
			return true;
		}
	}
	return false;


}
      
/* main function */
int main(){
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	/* initialize a pixel */
	Pixel *pixel;
	int maxScreenX = 800;
	int maxScreenY = 800;
	int frogLane = 4;
	int objectLane = 1;
	int objectPosition= 0;
	int frogPosition = 0;
	int frogDirection = 5;
	int counter = 0;
	int currentDirection = 1;
	bool collided = false;
	pixel = malloc(sizeof(Pixel));
	//~ drawMainMenu(pixel);
	int frogStartPoint = 30;
	int startingPoint = -30;
	int testPoint = 800;
	int currentCode = -2;	
	
	bool buttonPressed = false;	
	drawBackground(pixel, maxScreenX, maxScreenY);
	frogPosition = drawFrog(pixel, frogStartPoint, maxScreenX, frogLane, -1);
	while(1){

			frogDirection = getDirectionFromCon(&frogLane, &frogStartPoint, currentCode);
			//~ printf("code %d\n", frogDirection);
			frogPosition = drawFrog(pixel, frogStartPoint, maxScreenX, frogLane, frogDirection);
			currentCode = frogDirection;
	//~ while(startingPoint <= maxScreenX){
		//~ frogDirection = getDirectionFromCon(&frogLane);
		//~ frogPosition = drawFrog(pixel, 0, maxScreenX, frogLane, frogDirection);
		//~ objectPosition = drawAlien(pixel, testPoint, maxScreenX, objectLane, LEFT);
		//~ int secindObj = drawAlien(pixel, startingPoint + 20, maxScreenX, 5, RIGHT);
		//~ collided = checkCollision(pixel, frogPosition, objectPosition, frogLane, objectLane) || checkCollision(pixel, frogPosition, secindObj, frogLane, 5) ;
		//~ if (collided){
			//~ break;
		//~ }
		//~ testPoint--;
		//~ startingPoint++;
		//~ counter++;
		//~ else if (frogLane == 2){
			//~ frogLane--;
			//~ currentDirection = 1;
			//~ delayMicroseconds(10000);
		//~ }
	//~ }

}
			/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	return 0;
}




