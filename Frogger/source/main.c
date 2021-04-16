#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include <wiringPi.h>
#include <stdbool.h>
#include "SNES.h"

//All the Images
#include "froggie.h"
#include "menuScreen.h"
#include "blueCar1.h"
#include "redCar1.h"
#include "GameScreen.h"

#define UP 4
#define RIGHT 7
#define DOWN 5
#define LEFT 6

/* Definitions */
typedef struct {
	short int color;
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
	short int *menuPtr = (short int *) MenuScreen.pixel_data;
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



void drawGameScreen(Pixel *pixel){
	short int *gameScreenPtr = (short int *) gameScreen.pixel_data;
	int i = 0;
	for (int y = 0; y < 800; y++)
	{
		for (int x = 0; x < 800; x++) 
		{	 
			pixel->color = gameScreenPtr[i];
			i++;
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
		}
	}
}
int getDirectionFromCon(int* frogLane, int* frogStart, int currentCode){
	int buttonCode = getButtonPressed(currentCode);
	int size =33;
	//~ printf("lane %d\n", *frogLane);
	//~ if(*frogLane >= 0 && *frogLane <= 10 && *frogStart>= 30 && *frogStart <= 800){

		if (buttonCode == 5){
			if(*frogLane < 23){
				*frogLane += 1;
			}
		}
		if (buttonCode == 4){
			if(*frogLane > 0){
				*frogLane -= 1;
			}
		}
		if (buttonCode == 6){
			if(*frogStart >= 30){
				*frogStart -= 33;
			}
		}
		if (buttonCode == 7){
			if(*frogStart <= 770){
				*frogStart += 33;
			}
		}
	//~ }
	
	//~ printf("lane 2 %d\n", *frogLane);
	return buttonCode;

}

void cleanBackground(Pixel *pixel, int xStart, int yStart, int xEnd, int yEnd){
	short int *gameScreenPtr = (short int *) gameScreen.pixel_data;
	int i = 0;

	for(int x = xStart; x <= xEnd; x++){
		for(int y = yStart; y <= yEnd; y++){
			if(x >= 0 && x< 800 && x < 800 && y >= 0){
				i = (800 * y + x);
				pixel->x = x;
				pixel->y = y;
				pixel->color = gameScreenPtr[i];
				drawPixel(pixel);
			}
		}
	}
	}


void drawFrog(Pixel *pixel, int xStartingPoint, int maxScreenX, int frogLane, int direction){
	short int *frogPtr=(short int *) froggie.pixel_data;
	int i =0;
	int x, y;
	int size = 33;
	int xCordCleanStart;
	int xCordCleanEnd;
	int yCordCleanStart;
	int yCordCleanEnd;
	for (y = size*frogLane; y < size + (size*frogLane); y++)
	{
		for ( x = xStartingPoint; x < (size + xStartingPoint); x++) 
		{	
			i++;
			if (x>=0 && x< maxScreenX) {
				pixel->color = frogPtr[i];
				pixel->x = x;
				pixel->y = y;
				//draws the frog inside the screen and ignores the backgrounds of the frog image
				if(x <= maxScreenX && frogPtr[i] >= -5000){
					drawPixel(pixel);
				}
			}
		}
	}
	if (direction >= 0){
		if (direction == RIGHT){
			xCordCleanStart = xStartingPoint - size;
			xCordCleanEnd = xStartingPoint-1;
			yCordCleanStart = size*frogLane;
			yCordCleanEnd = y - 1;
		}
		else if (direction == LEFT){
			xCordCleanStart = x;
			xCordCleanEnd = xCordCleanStart + size;
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
			xCordCleanStart = xStartingPoint;
			xCordCleanEnd = x - 1;
			yCordCleanStart = (size*frogLane) - size;
			yCordCleanEnd = size*frogLane - 1;
		}
		cleanBackground(pixel, xCordCleanStart, yCordCleanStart, xCordCleanEnd, yCordCleanEnd);
	}
	//~ if (x>=0){
		//~ return xStartingPoint;
	//~ }
	//~ else{
		//~ return 0;
	//~ }
	//~ delayMicroseconds(100000);
}

void drawCar(Pixel *pixel, int* carStart, int maxScreenX, int objectLane, int direction){
	short int *carPtr = 0;
	if (direction == RIGHT){
		carPtr=(short int *) redCar.pixel_data;
	}
	else{
		carPtr=(short int *) blueCar.pixel_data;
	}
	int i =0;
	int x, y;
	int size = 33;
	int xCordCleanStart;
	int xCordCleanEnd;
	int yCordCleanStart;
	int yCordCleanEnd;
	int startPoint = *carStart;
	
	for (y = size*objectLane; y < size + (size*objectLane); y++)
	{
		for (x = startPoint; x < (size + startPoint); x++) 
		{	
			//~ printf("%d", x);
			i++;
			if (x>=0 && x< maxScreenX ) {
				pixel->color = carPtr[i];
				pixel->x = x;
				pixel->y = y;
				if(x <= maxScreenX){
					drawPixel(pixel);
				}
			}
		}
	}


	if (direction == RIGHT){
		xCordCleanStart = startPoint - 1;
		xCordCleanEnd = startPoint - 1;
		yCordCleanStart = size*objectLane;
		yCordCleanEnd = y - 1;
		*carStart +=1;
	}
	else if (direction == LEFT){
		xCordCleanStart = x;
		xCordCleanEnd = x ;
		yCordCleanStart = size*objectLane;
		yCordCleanEnd = y - 1;
		*carStart -=1;
	}
	cleanBackground(pixel, xCordCleanStart, yCordCleanStart, xCordCleanEnd, yCordCleanEnd);
	
}





bool checkCollision(Pixel *pixel, int frogPosition, int objectPosition, int frogLane, int objectLane){
	if (frogLane == objectLane){
		int locationDifference = frogPosition - objectPosition;
		if (locationDifference < 33 && locationDifference > -33){
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
	pixel = malloc(sizeof(Pixel));
	
	
	int maxScreenX = 800;
	int maxScreenY = 800;
	int frogLane = 9;
	int carLane = 5;

	int frogPosition = 0;
	int frogDirection = 5;
	
	int currentDirection = 1;
	bool collided = false;
	int frogStartPoint = 400;
	int carStartPointFromRight = 800;
	int carStartPointFromLeft = -33;
	int testPoint = 800;
	int currentCode = -2;	 
	int cars[] = {600, 100, 0};
	int carLanes[] = {4,5,6};
	int carPos;
	drawMainMenu(pixel);
	drawGameScreen(pixel);
	bool buttonPressed = false;	
	
	drawFrog(pixel, frogStartPoint, maxScreenX, frogLane, -1);
	while(1){ 
			for(int i = 0; i < 3; i++ ){
				if (i == 0){
					if (cars[i] <= 100){
						cars[i] =  600;
						
					}
				}
				else if (i ==1){
					if (cars[i] >= 600){
						cars[i] = 100;
						
					}
				}
			}
			frogDirection = getDirectionFromCon(&frogLane, &frogStartPoint, currentCode);
			delayMicroseconds(80000);
			drawFrog(pixel, frogStartPoint, maxScreenX, frogLane, frogDirection);
			currentCode = frogDirection;
			drawCar(pixel, &cars[0], maxScreenX, carLanes[0], LEFT);
			drawCar(pixel, &cars[1], maxScreenX, carLanes[1], RIGHT);
			for (int i = 0; i < 3; i++){
			collided = checkCollision(pixel, frogStartPoint, cars[i], frogLane, carLanes[i]);
			if(collided){
				cleanBackground(pixel, frogStartPoint,  33*frogLane, frogStartPoint + 33 ,  33*frogLane + 33);
				frogStartPoint = 400;
				frogLane = 9;
				}
			}
			
			
			
			
	}
	//~ int initStartPointArr = -30;
	//~ int objectStartPointArr[] = {-30, -90, -150};
	//~ int Obj3;
	//~ int Obj2;
	//~ int Obj; 
	//~ while(1){
			
			//~ Obj = drawAlien(pixel, objectStartPointArr[0], maxScreenX, 5, RIGHT);
			//~ objectStartPointArr[0]++;
			//~ Obj2 =  drawAlien(pixel, objectStartPointArr[1], maxScreenX, 5, RIGHT);
			//~ objectStartPointArr[1]++;
			//~ printf("val %d", objectStartPointArr[1]);
			//~ Obj3 =  drawAlien(pixel, objectStartPointArr[2], maxScreenX, 5, RIGHT);
			//~ objectStartPointArr[2]++;
			//~ for(int i = 0; i < 3; i++ ){
				//~ if (objectStartPointArr[i]> 800){
					//~ objectStartPointArr[i] = initStartPointArr;
					//~ break;
				//~ }
			//~ }
			//~ delayMicroseconds(10000);
		

	//~ while(startingPoint <= maxScreenX){
		//~ frogDirection = getDirectionFromCon(&frogLane);
		//~ frogPosition = drawFrog(pixel, 0, maxScreenX, frogLane, frogDirection);
		//~ objectPosition = drawAlien(pixel, testPoint, maxScreenX, objectLane, LEFT);
		//~ int secondObj = drawAlien(pixel, startingPoint, maxScreenX, 5, RIGHT);
	
		//~ int thirdObj = drawAlien(pixel, startingPoint - 60, maxScreenX, 5, RIGHT);
		//~ collided = checkCollision(pixel, frogPosition, objectPosition, frogLane, objectLane) || checkCollision(pixel, frogPosition, secindObj, frogLane, 5) ;
		//~ if (collided){
			//~ break;
		//~ }
		//~ delayMicroseconds(10000);
		//~ testPoint--;
		//~ startingPoint++;
		//~ counter++;
		//~ else if (frogLane == 2){
			//~ frogLane--;
			//~ currentDirection = 1;
			//~ delayMicroseconds(10000);
		//~ }
		//~ }
		//~ while(startingPoint - 60 <= maxScreenX){
			//~ int thirdObj = drawAlien(pixel, startingPoint - 60, maxScreenX, 5, RIGHT);
		//~ }	
		//~ startingPoint = -30;
	//~ }

			/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	return 0;
}




