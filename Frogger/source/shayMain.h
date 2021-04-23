#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include <wiringPi.h>
#include <stdbool.h>
#include "SNES.h"
#include <pthread.h>
#include <time.h>

//All the Images
#include "froggie.h"
#include "menuScreen.h"
#include "blueCar1.h"
#include "redCar1.h"
#include "GameScreen.h"
#include "truck.h"
#include "zero.h"
#include "one.h"
#include "two.h"
#include "three.h"
#include "four.h"
#include "five.h"
#include "six.h"
#include "seven.h"
#include "eight.h"
#include "nine.h"
#include "heart.h"
#include "gameWin.h"
#include "gameLost.h"

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

struct shared{
	int position;
	int lane;
	int maxLane;
	int time; 
	int moves;
	int lives;
	time_t startTime;
	time_t endTime;
	int score;
};

struct image 
{
	int lane;
	int startX;
	int startY;
};

struct shared frog;
struct image heartPack;

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
	short int *gamePtr = (short int *) game1.pixel_data;
	int i = 0;
	for (int y = 0; y < 800; y++)
	{
		for (int x = 0; x < 800; x++) 
		{	 
			pixel->color = gamePtr[i];
			i++;
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
		}
		
	}
}
int getDirectionFromCon(int* frogLane, int* frogStart, unsigned int *gpioPtr){
	int buttonCode = getButtonPressed(gpioPtr);
	
	int size =33;
		if (buttonCode == 5){
			if(*frogLane < 17){
				*frogLane += 1;
				frog.moves -= 1;
			}
		}
		
		else if (buttonCode == 4){
			if(*frogLane > 0){
				*frogLane -= 1;
				frog.moves -= 1;
			}
		}
		else if (buttonCode == 6){
			if(*frogStart >= 264){
				*frogStart -= 33;
				frog.moves -= 1;

			}
		}
		else if (buttonCode == 7){
			if(*frogStart <= 520){
				*frogStart += 33;
				frog.moves -= 1;

			}
		}
		
		if (frog.lane < frog.maxLane)
		{
			frog.maxLane = frog.lane;
		}
	//~ }
	
	//~ printf("lane 2 %d\n", *frogLane);
	return buttonCode;

}

void cleanBackground(Pixel *pixel, int xStart, int yStart, int xEnd, int yEnd){
	short int *gamePtr = (short int *) game1.pixel_data;
	int i = 0;

	for(int x = xStart; x <= xEnd; x++){
		for(int y = yStart; y <= yEnd; y++){
			if(x >= 0 && x< 800 && y < 800 && y >= 0){
				i = (800 * y + x);
				pixel->x = x;
				pixel->y = y;
				pixel->color = gamePtr[i];
				drawPixel(pixel);
			}
		}
	}
}


void drawFrog(Pixel *pixel, int xStartingPoint, int maxScreenX, int frogLane, int direction){
	short int *frogPtr=(short int *) froggie.pixel_data;
	int i =0;
	int x, y;
	int laneSize = 35;
	int objSize = 33;
	int xCordCleanStart;
	int xCordCleanEnd;
	int yCordCleanStart;
	int yCordCleanEnd;
	for (y = 152 + (laneSize*frogLane); y < 152 + laneSize + (laneSize*frogLane); y++)
	{
		for ( x = xStartingPoint; x < (objSize + xStartingPoint); x++) 
		{	
			if (y < 152 + objSize + laneSize*frogLane){
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
	}
	if (direction >= 0){
		if (direction == RIGHT){
			xCordCleanStart = xStartingPoint - objSize;
			xCordCleanEnd = xStartingPoint-1;
			yCordCleanStart = 152 + laneSize*frogLane;
			yCordCleanEnd = yCordCleanStart + objSize;
		}
		else if (direction == LEFT){
			xCordCleanStart = x;
			xCordCleanEnd = xCordCleanStart + objSize;
			yCordCleanStart = 152 + laneSize*frogLane;
			yCordCleanEnd = yCordCleanStart + objSize;
		}
		if(direction == UP){
			xCordCleanStart = xStartingPoint;
			xCordCleanEnd = x - 1;
			yCordCleanStart = laneSize + laneSize*frogLane + 152;
			yCordCleanEnd = objSize + yCordCleanStart;	
		}
		else if(direction == DOWN){
			xCordCleanStart = xStartingPoint;
			xCordCleanEnd = x - 1;
			yCordCleanStart = (laneSize*frogLane) - laneSize + 152;
			yCordCleanEnd = objSize + yCordCleanStart ;
		}
		cleanBackground(pixel, xCordCleanStart, yCordCleanStart, xCordCleanEnd, yCordCleanEnd);
	}
}
void drawCar(Pixel *pixel, int* carStart, int maxScreenX, int objectLane, int direction){
	short int *carPtr = 0;
	
	
	int length = 33;
	int width = 33;
	if (objectLane == 14){
		carPtr = (short int *) truck.pixel_data;
		length = 70;
	}

	else if (direction == RIGHT){
		carPtr=(short int *) redCar.pixel_data;
	}
	else{
		carPtr=(short int *) blueCar.pixel_data;
	}
	int i =0;
	int x, y;
	int laneSize = 35;
	int xCordCleanStart;
	int xCordCleanEnd;
	int yCordCleanStart;
	int yCordCleanEnd;
	int startPoint = *carStart;
	int yStart = 152 + (laneSize*objectLane);
	int yEnd = 152 + (laneSize + (laneSize*objectLane));
	
	for (y = yStart; y < yEnd; y++)
	{
		for (x = startPoint; x < (length + startPoint); x++) 
		{	
			if (y <  152 + (width + (laneSize * objectLane))){
				//~ printf("%d", x);
				i++;
				if (x > 241 && x<= 561 ) {
					pixel->color = carPtr[i];
					pixel->x = x;
					pixel->y = y;
					drawPixel(pixel);
				
				}
			}
		}
	}


	if (direction == RIGHT){
		xCordCleanStart = startPoint - 1;
		xCordCleanEnd = startPoint - 1;
		yCordCleanStart = laneSize*objectLane;
		yCordCleanEnd = yCordCleanStart + width;
		*carStart +=1;
	}
	else if (direction == LEFT){
		xCordCleanStart = x;
		xCordCleanEnd = x ;
		yCordCleanStart = laneSize*objectLane;
		yCordCleanEnd = yCordCleanStart + width;
		*carStart -=1;
	}
	cleanBackground(pixel, xCordCleanStart, yCordCleanStart + 152, xCordCleanEnd, yCordCleanEnd + 152);
	
}





bool checkCollision(Pixel *pixel, int frogPosition, int objectPosition, int frogLane, int objectLane){
	int objectSize; 
	
	if(objectLane == 14)
	{
		objectSize = 70;
	}
	else
	{
		objectSize = 33;
	}
	
	if (frogLane == objectLane){
		int locationDifference = frogPosition - objectPosition;
		if (locationDifference < objectSize && locationDifference > -33){
			return true;
		}
	}
	return false;
}

bool isHeartObtained ()
{
	int objectSize = 33; 
	if (frog.lane == heartPack.lane)
	{
		int locationDifference = frog.position - heartPack.startX;
		if (locationDifference < objectSize && locationDifference > -33){
			return true;
		}
	}
	return false;

}

void drawHeart (Pixel *pixel)
{
	int xStart = 0;
	int yStart = 0;
	int xEnd = 0;
	int yEnd = 0;

	short int *heartPtr=(short int *) heart.pixel_data;

	int r = (rand() % 4) + 1;
	int randomLane = (r * 4) + 1;
	xStart = (rand() % (522 + 1 - 245)) + 245;
	
	//randomLane = 5;
	if (randomLane == 5)
	{
		yStart = 327;
	}
	else if (randomLane == 9)
	{
		yStart = 467;
	}
	else if (randomLane == 13)
	{
		yStart = 607;
	}
	else if (randomLane == 17)
	{
		yStart = 747;
	}
	
	heartPack.lane = randomLane;
	heartPack.startX = xStart;
	heartPack.startY = yStart;
	
	
	yEnd = yStart + 33;
	xEnd = xStart + 33;
	int i = 0;
	for (int y = yStart; y < yEnd; y++)
	{
		for (int x = xStart; x < xEnd; x++) 
		{	
			
			pixel->color = heartPtr[i];
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
			i++;

		}
	}
		
	
}

short int * getNumPtr (int num)
{
	short int *ptr = 0;
	
	if (num == 0)
	{
		return (short int *) zero.pixel_data;
	}
	else if (num == 1)
	{
		return (short int *) one.pixel_data;
	}
	else if (num == 2)
	{
		return (short int *) two.pixel_data;
	}
	else if (num == 3)
	{
		return (short int *) three.pixel_data;
	}
	else if (num == 4)
	{
		return (short int *) four.pixel_data;
	}
	else if (num == 5)
	{
		return (short int *) five.pixel_data;
	}
	else if (num == 6)
	{
		return (short int *) six.pixel_data;
	}
	else if (num == 7)
	{
		return (short int *) seven.pixel_data;
	}
	else if (num == 8)
	{
		return (short int *) eight.pixel_data;
	}
	else if (num == 9)
	{
		return (short int *) nine.pixel_data;
	}
	
	return 0;
	
}


void drawGameOverScore (Pixel *pixel)
{
	time_t remaining = 200 - frog.endTime;
	frog.score += (frog.moves + remaining + frog.lives)*10;
	
	int first = frog.score / 1000;
	
	int second = frog.score / 100;
	second %= 10;
	
	int third = frog.score / 10;
	third %= 10;
	
	int fourth = frog.score % 10;
	
	short int *numPtr1 = getNumPtr(first);

	
	int yStart = 200 + 250;
	int yEnd = yStart+33;
	int xStart = 203 + 202;
	int xEnd = xStart+33;
	int i = 0;
	for (int y = yStart; y < yEnd; y++)
	{
		for (int x = xStart; x < xEnd; x++) 
		{	
			
			pixel->color = numPtr1[i];
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
			i++;

		}
	}
	
	//second Value
	short int *numPtr2 = getNumPtr(second);
	
	xStart = 237 + 202;
	xEnd = xStart+33;
	i = 0;
	for (int y = yStart; y < yEnd; y++)
	{
		for (int x = xStart; x < xEnd; x++) 
		{	
			
			pixel->color = numPtr2[i];
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
			i++;

		}
	}
	
	//third Value
	short int *numPtr3  = getNumPtr(third);
	
	xStart = 271+ 202;
	xEnd = xStart+33;
	i = 0;
	for (int y = yStart; y < yEnd; y++)
	{
		for (int x = xStart; x < xEnd; x++) 
		{	
			
			pixel->color = numPtr3[i];
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
			i++;

		}
	}
	
	//fourth Value
	short int *numPtr4 = getNumPtr(fourth);
	
	xStart = 305+ 202;
	xEnd = xStart+33;
	i = 0;
	for (int y = yStart; y < yEnd; y++)
	{
		for (int x = xStart; x < xEnd; x++) 
		{	
			
			pixel->color = numPtr4 [i];
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
			i++;

		}
	}
	
	exit(-1);
}

void updateScore (Pixel *pixel)
{
	//time_t remaining = 200 - frog.endTime;
	//int score = (frog.lives + remaining + frog.moves ) * 5;
	
	
	//~ int score = 170 - (frog.maxLane * 10);
	
	//~ int first = score / 1000;
	
	//~ int second = score / 100;
	//~ second %= 10;
	
	//~ int third = score / 10;
	//~ third %= 10;
	
	//~ int fourth = score % 10;
	
	frog.score = 170 - (frog.maxLane * 10);
	
	int first = frog.score / 1000;
	
	int second = frog.score / 100;
	second %= 10;
	
	int third = frog.score / 10;
	third %= 10;
	
	int fourth = frog.score % 10;
	
	//printf("%d", score);
	
	//short int *numPtr = 0;

	//getNumPtr(0, numPtr);
	
	//short int *numPtr=(short int *) zero.pixel_data;
	
	//first Value
	short int *numPtr1 = getNumPtr(first);
	
	int yStart = 170;
	int yEnd = 170+33;
	int xStart = 615;
	int xEnd = xStart+33;
	int i = 0;
	for (int y = yStart; y < yEnd; y++)
	{
		for (int x = xStart; x < xEnd; x++) 
		{	
			
			pixel->color = numPtr1[i];
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
			i++;

		}
	}
	
	//second Value
	short int *numPtr2 = getNumPtr(second);
	
	xStart = 649;
	xEnd = xStart+33;
	i = 0;
	for (int y = yStart; y < yEnd; y++)
	{
		for (int x = xStart; x < xEnd; x++) 
		{	
			
			pixel->color = numPtr2[i];
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
			i++;

		}
	}
	
	//third Value
	short int *numPtr3  = getNumPtr(third);
	
	xStart = 683;
	xEnd = xStart+33;
	i = 0;
	for (int y = yStart; y < yEnd; y++)
	{
		for (int x = xStart; x < xEnd; x++) 
		{	
			
			pixel->color = numPtr3[i];
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
			i++;

		}
	}
	
	//fourth Value
	short int *numPtr4 = getNumPtr(fourth);
	
	xStart = 717;
	xEnd = xStart+33;
	i = 0;
	for (int y = yStart; y < yEnd; y++)
	{
		for (int x = xStart; x < xEnd; x++) 
		{	
			
			pixel->color = numPtr4 [i];
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
			i++;

		}
	}
	//cleanBackground(pixel, xStart, yStart, xEnd, yEnd);

	
}

void drawGameWon(Pixel *pixel)
{
	short int *gameWonPtr = (short int *) gameWin.pixel_data;
	int i = 0;
	for (int y = 245; y < 545; y++)
	{
		for (int x = 202; x < 602; x++) 
		{	 
			pixel->color = gameWonPtr[i];
			i++;
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
		}
		
	}
	
	
	drawGameOverScore(pixel);
	
}

void drawGameLost(Pixel *pixel)
{
	short int *gameLostPtr = (short int *) gameLost.pixel_data;
	int i = 0;
	for (int y = 245; y < 545; y++)
	{
		for (int x = 202; x < 602; x++) 
		{	 
			pixel->color = gameLostPtr[i];
			i++;
			pixel->x = x;
			pixel->y = y;
			drawPixel(pixel);
		}
		
	}
	
	
	drawGameOverScore(pixel);
	
}

void updateLives (Pixel *pixel)
{
	
	int live = frog.lives;
	
	
	if (live >= 0)
	{
	
		short int *numPtr1 = getNumPtr(live);
		int yStart = 170;
		int yEnd = 170+33;
		int xStart = 103;
		int xEnd = xStart+33;
		int i = 0;
		for (int y = yStart; y < yEnd; y++)
		{
			for (int x = xStart; x < xEnd; x++) 
			{	
				
				pixel->color = numPtr1[i];
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
				i++;

			}
		}
	}
	
	if (live == 0)
	{
		drawGameLost(pixel);
	}
	

	
}

void updateSteps (Pixel *pixel)
{
	
	int steps = frog.moves;
	
	if (steps >= 0)
	{
		int first = steps / 100;
		
		int second = steps / 10;
		second %= 10;
		
		int third = steps % 10;
		
		short int *numPtr1 = getNumPtr(first);
		
		int yStart = 380;
		int yEnd = yStart+33;
		int xStart = 35;
		int xEnd = xStart+33;
		int i = 0;
		for (int y = yStart; y < yEnd; y++)
		{
			for (int x = xStart; x < xEnd; x++) 
			{	
				
				pixel->color = numPtr1[i];
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
				i++;

			}
		}
		
		
		//second Value
		short int *numPtr2 = getNumPtr(second);
		
		xStart = 69;
		xEnd = xStart+33;
		i = 0;
		for (int y = yStart; y < yEnd; y++)
		{
			for (int x = xStart; x < xEnd; x++) 
			{	
				
				pixel->color = numPtr2[i];
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
				i++;

			}
		}
		
		//third Value
		short int *numPtr3 = getNumPtr(third);
		
		xStart = 103;
		xEnd = xStart+33;
		i = 0;
		for (int y = yStart; y < yEnd; y++)
		{
			for (int x = xStart; x < xEnd; x++) 
			{	
				
				pixel->color = numPtr3[i];
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
				i++;

			}
		}
	}
	
	if (steps == 0)
	{
		drawGameLost(pixel);
	}
	
	
	
}

void updateTime (Pixel *pixel)
{
	int secondRemaining = 200 - frog.endTime;
	
	if (secondRemaining >= 0)
	{
		int first = secondRemaining / 100;
		
		int second = secondRemaining / 10;
		second %= 10;
		
		int third = secondRemaining % 10;
		
		short int *numPtr1 = getNumPtr(first);
		
		int yStart = 595;
		int yEnd = yStart+33;
		int xStart = 35;
		int xEnd = xStart+33;
		int i = 0;
		for (int y = yStart; y < yEnd; y++)
		{
			for (int x = xStart; x < xEnd; x++) 
			{	
				
				pixel->color = numPtr1[i];
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
				i++;

			}
		}
		
		
		//second Value
		short int *numPtr2 = getNumPtr(second);
		
		xStart = 69;
		xEnd = xStart+33;
		i = 0;
		for (int y = yStart; y < yEnd; y++)
		{
			for (int x = xStart; x < xEnd; x++) 
			{	
				
				pixel->color = numPtr2[i];
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
				i++;

			}
		}
		
		//third Value
		short int *numPtr3 = getNumPtr(third);
		
		xStart = 103;
		xEnd = xStart+33;
		i = 0;
		for (int y = yStart; y < yEnd; y++)
		{
			for (int x = xStart; x < xEnd; x++) 
			{	
				
				pixel->color = numPtr3[i];
				pixel->x = x;
				pixel->y = y;
				drawPixel(pixel);
				i++;

			}
		}
	}
	
	if (secondRemaining == 0)
	{
		drawGameLost(pixel);
	}
	
}

bool isGameWon(Pixel *pixel)
{
	int castleX[] = {255, 321, 387, 453, 519};
	if (frog.lane == 1)
	{
		for (int i = 0; i <= 4; i++)
		{
			if (castleX[i] == frog.position)
			{
				return true;
			}
		}
		drawGameLost(pixel);
		
	}
	return false;
}

void *carLaneThread(){
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int maxScreenX = 800;
	int maxScreenY = 800;
	bool collided = false;
	//bool gotHeart = false;

	int maxLane = 17;
	
	int cars[] = {561, 198, 198};
	int carLanes[] = {15,16,14};
	
	int valuePackDrawn = 0;
  
	while(1){ 
			if (cars[0] <= 205){
				cars[0] = 561;
			}
			if (cars[1] >= 561){
				cars[1] = 205;
			}

			delayMicroseconds(60000);
			
			drawCar(pixel, &cars[0], maxScreenX, carLanes[0], LEFT);
			drawCar(pixel, &cars[1], maxScreenX, carLanes[1], RIGHT);
			drawCar(pixel, &cars[2], maxScreenX, carLanes[2], RIGHT);

			for (int i = 0; i < 3; i++){
				collided = checkCollision(pixel, frog.position, cars[i], frog.lane, carLanes[i]);
				if(collided){
					//clean the dead frog from screen adn reset frog position
					cleanBackground(pixel, frog.position,  152 + 35*frog.lane, frog.position + 33 ,  152 + 35*frog.lane + 33);
					frog.position = 387;
					frog.lane = 17;
					frog.lives -= 1;
					}
				//~ gotHeart = isHeartObtained();
				//~ if (gotHeart)
				//~ {
					//~ cleanBackground(pixel, heartPack.startX,  152 + 35*heartPack.lane, heartPack.startX + 33 ,  152 + 35*heartPack.lane + 33);
					//~ frog.lives ++;
				//~ }
				
			}
			time_t finish = time(NULL);
			frog.endTime = finish - frog.startTime;
			
			
			if (frog.endTime == 30)
			{
				valuePackDrawn ++;
			}
			
			updateScore(pixel);
			
			updateLives(pixel);
			updateSteps(pixel);
			updateTime(pixel);
			
			if (valuePackDrawn == 1)
			{
				drawHeart(pixel);
			}

	
		
}	
}


void cleanMenu(Pixel *pixel, int xStart, int yStart, int xEnd, int yEnd){
	short int *menuPtr = (short int *) MenuScreen.pixel_data;
	int i = 0;

	for(int x = xStart; x <= xEnd; x++){
		for(int y = yStart; y <= yEnd; y++){
			if(x >= 0 && x< 800 && y < 800 && y >= 0){
				i = (800 * y + x);
				pixel->x = x;
				pixel->y = y;
				pixel->color = menuPtr[i];
				drawPixel(pixel);
			}
		}
	}
}

void underlineStart(Pixel *pixel)
{
	int y = 360;
	int startingX = 176;
	int endingX = 612;
	for (int x = startingX; x <= endingX; x++)
	{
		pixel->color = 0xF;
		pixel->x = x;
		pixel->y = y;
		drawPixel(pixel);
	}

	cleanMenu(pixel, 194, 499, 586, 499);
	
}

void underlineQuit(Pixel *pixel)
{
	int y = 499;
	int startingX = 194;
	int endingX = 586;
	for (int x = startingX; x <= endingX; x++)
	{
		pixel->color = 0xF;
		pixel->x = x;
		pixel->y = y;
		drawPixel(pixel);
	}
	cleanMenu(pixel, 176, 360, 612, 360);
}
      
/* main function */
int main(){
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	unsigned int *gpioPtr = getGPIOPtr();  

	drawMainMenu(pixel);
	
	bool start = false;
	int buttonCode = 0;
	
	do
	{
		
		buttonCode = getButtonPressed(gpioPtr);
		
		
		if (buttonCode == 4)
		{
			underlineStart(pixel);
			start = true;
		}
		else if (buttonCode == 5)
		{
			underlineQuit(pixel);
			start = false;
		}
		
	}while (buttonCode != 8);
	
	if (!start)
	{
		return 0;
		
	}

	
	srand((unsigned int)time(NULL));
	
	frog.startTime = time(NULL);
				

	//create car thread
	int rc;
	pthread_t carThread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	rc = pthread_create(&carThread, &attr, carLaneThread, NULL);


	int maxScreenX = 800;
	int maxScreenY = 800;
	frog.lane = 17;
	frog.maxLane = 17;
	int carLane = 5;
	bool gotHeart = false;
	int heartCount = 0;
	
	int currentDirection = 1;
	int frogDirection;

	frog.position = 387;
	frog.lives = 4;
	frog.moves = 200;
	frog.time = 200;
	int testPoint = 800;
	int currentCode = -2;	 
	int cars[] = {561, 198, 198};
	int carLanes[] = {15,16,14};
	int carPos;
	drawGameScreen(pixel);
	bool buttonPressed = false;	
	  // get gpio pointer
	
	drawFrog(pixel, frog.position, maxScreenX, frog.lane, -1);
	while(1){ 
			delayMicroseconds(60000);
			frogDirection = getDirectionFromCon(&frog.lane, &frog.position, gpioPtr);		
			drawFrog(pixel, frog.position, maxScreenX, frog.lane, frogDirection);	
			
			if (heartCount == 0)
			{
				gotHeart = isHeartObtained();
				if (gotHeart)
				{
					cleanBackground(pixel, heartPack.startX,  152 + 35*heartPack.lane, heartPack.startX + 33 ,  152 + 35*heartPack.lane + 33);
					frog.lives ++;
					heartCount = 1;
				}
			}
			
			if (isGameWon(pixel))
			{
				drawGameWon(pixel);
			}
			
	}
	//~ pthred_exit(NULL);
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




