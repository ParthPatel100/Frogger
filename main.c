#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include <wiringPi.h>
#include <stdbool.h>
#include "SNES.h"
#include <pthread.h>
#include <time.h>
#include <string.h>

//All the Images
#include "froggie.h"
#include "menuScreen.h"
#include "blueCar1.h"
#include "redCar1.h"
#include "GameScreen.h"
#include "truck.h"
#include "tumbleWeed.h"
#include "logBig.h"
#include "log.h"
#include "logBig2.h"
#include "snake.h"
#include "snake2.h"
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

struct Game{
	short int screen[800*800*2];
}game;

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
	int direction;
	int onLog;
	int speed;
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
void drawGameScreen( Pixel *pixel){
	for (int y = 0; y < 800; y++)
	{
		for (int x = 0; x < 800; x++) 
		{	 
			pixel -> x = x;
			pixel -> y = y;
			pixel -> color = game.screen[800 * y + x];
			
			drawPixel(pixel);
		}
	}
	
}

void drawGameBoard(Pixel *pixel){
	short int *gamePtr = (short int *) game1.pixel_data;
	int i = 0;
	for (int y = 0; y < 800; y++)
	{
		for (int x = 0; x < 800; x++) 
		{	 
			game.screen[i] = gamePtr[i];
			i++;
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
			game.screen[i] = menuPtr[i];
			i++;
			
			//~ drawPixel(pixel);
		}
	}
}
void cleanBackground(Pixel *pixel, int xStart, int yStart, int xEnd, int yEnd){
	short int *gamePtr = (short int *) game1.pixel_data;
	int i = 0;

	for(int x = xStart; x <= xEnd; x++){
		for(int y = yStart; y <= yEnd; y++){
			if(x >= 0 && x< 800 && y < 800 && y >= 0){
				i = (800 * y + x);
				game.screen[i] = gamePtr[i];
				drawPixel(pixel);
			}
		}
	}
}
void resetFrog(Pixel *pixel){
	//clean the dead frog from screen adn reset frog position
	cleanBackground(pixel, frog.position,  152 + 35*frog.lane, frog.position + 33 ,  152 + 35*frog.lane + 33);
	frog.position = 387;
	frog.lane = 17;
	frog.direction = -1;
	frog.lives -= 1;

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
			if(*frogStart <= 528){
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
					if(frogPtr[i] >= -5000){
						game.screen[800*y + x] = frogPtr[i];
					}
					//draws the frog inside the screen and ignores the backgrounds of the frog image
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




void drawObj(Pixel *pixel, int* objStart, int maxScreenX, int objectLane, int direction){
	short int *objectPtr = 0;
	int j = 0;
	int length = 33;
	int width = 33;
	if(objectLane >= 14 && objectLane <= 16){
		if (objectLane == 14){
			objectPtr= (short int *) truck.pixel_data;
			length = 70;
		}

		else if (direction == RIGHT){
			objectPtr=(short int *) redCar.pixel_data;
			
		}
		else{
			objectPtr=(short int *) blueCar.pixel_data;
		}
	}
	else if (objectLane >= 2 && objectLane <= 4){
		if (objectLane == 4){
			objectPtr= (short int *) logBig.pixel_data;
			length = 75;
		}

		else if (direction == RIGHT){
			objectPtr=(short int *) logBig2.pixel_data;
			length = 75;
		}
		else{
			objectPtr=(short int *) log.pixel_data;
			length = 50;
		}	
	}
	
	else if (objectLane >= 10 && objectLane <=12){
		if (direction == RIGHT){
			objectPtr = snake2.pixel_data;
			length = 50;
		}
		else if(direction == LEFT){
			objectPtr = snake.pixel_data;
			length = 50;
		}
	
	}
	else if (objectLane >= 6 && objectLane <=8){
		objectPtr = tumbleWeed.pixel_data;
		length = 33;
		

	
	}
	
	
	int i =0;
	int x, y;
	int laneSize = 35;
	int xCordCleanStart;
	int xCordCleanEnd;
	int yCordCleanStart;
	int yCordCleanEnd;
	int startPoint = *objStart;
	int yStart = 152 + (laneSize*objectLane);
	int yEnd = 152 + (laneSize + (laneSize*objectLane));
	
	for (y = yStart; y < yEnd; y++)
	{
		for (x = startPoint; x < (length + startPoint); x++) 
		{	
			
			if (y <  152 + (width + (laneSize * objectLane))){
				//~ printf("%d", x);
		
				if (x > 241 && x<= 561 ) {
					
					game.screen[800 * y + x] = objectPtr[i];
				
				}
				i++;
			}
		}
	}
	srand(time(NULL));

	if (direction == RIGHT){
		int speed = 2; //can be randomized
		*objStart +=speed;
		if(frog.lane == objectLane){
			frog.speed = speed;
		}
		xCordCleanStart = startPoint - speed;
		xCordCleanEnd = startPoint - 1;
		yCordCleanStart = laneSize*objectLane  + 152;
		yCordCleanEnd = yCordCleanStart + width;
	}
	else if (direction == LEFT){
		int speed = 1 ; //can be randomized
		*objStart -= speed;
		if(frog.lane == objectLane){
			frog.speed = speed;
		}
		xCordCleanStart = x;
		xCordCleanEnd = x + speed - 1;
		
		yCordCleanStart = laneSize*objectLane + 152;
		yCordCleanEnd = yCordCleanStart + width;
	}
	cleanBackground(pixel, xCordCleanStart, yCordCleanStart , xCordCleanEnd, yCordCleanEnd );
	
}
bool checkCollision(Pixel *pixel, int frogPosition, int objectPosition, int frogLane, int objectLane){
	int objectSize; 
	
	if(objectLane == 14)
	{
		objectSize = 70;
	}
	else if (objectLane == 3 || objectLane == 4)
	{
		objectSize = 75;
	}
	else if (objectLane >= 10 && objectLane <= 12)
	{
		objectSize = 50;
	}
	else
	{
		objectSize = 33;
	}
	
	if (frogLane == objectLane){
		int locationDifference = frog.position - objectPosition;
		if (locationDifference < objectSize && locationDifference > -33){
			return true;
		}
	}
	return false;
}
bool checkOnLog(Pixel *pixel, int objectPosition, int objectLane, int direction){
	int objectSize = 75;
	
	if (objectLane == 2){
		objectSize = 50;
	}
	if (frog.lane == objectLane){
		int locationDifference = frog.position - objectPosition;
		if(locationDifference >= 0 && locationDifference < objectSize - 33){
			frog.onLog = true;
			//if frog goes out of screen, return false
			if(frog.position <= 241){
				return false;
			}
			if(frog.position >= 528){
				return false;
			}
			
			//if log is moving to the right, make the frog move to the right otherwise to the left
			if(direction == RIGHT){
				frog.position += frog.speed;
			}
			else if (direction == LEFT){
				frog.position -= frog.speed;
			}
			return true;
		}
		
	}
	return false;
		
}

int generateRandInt(){
	return rand() % 300 + 241;	
	
}

int generateRandInt2(){
	return 160 - rand() % 40;	
}

void generateRandomStartPos(int objStartPointArray[6], int identifier){
	if (identifier % 1 == 0){
		objStartPointArray[0] = generateRandInt();
		objStartPointArray[1] = objStartPointArray[0] - generateRandInt2();
		objStartPointArray[2] = generateRandInt();
		objStartPointArray[3] = objStartPointArray[2] + generateRandInt2();
		objStartPointArray[4] = generateRandInt();
		objStartPointArray[5] =  objStartPointArray[4] - generateRandInt2();
	}
	else if (identifier % 1 == 1){
		objStartPointArray[0] = generateRandInt();
		objStartPointArray[1] = objStartPointArray[0] + generateRandInt2();
		objStartPointArray[2] = generateRandInt();
		objStartPointArray[3] = objStartPointArray[2] - generateRandInt2();
		objStartPointArray[4] = generateRandInt();
		objStartPointArray[5] =  objStartPointArray[4] + generateRandInt2();	
	}
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
			
			//~ pixel->color = heartPtr[i];
			//~ pixel->x = x;
			//~ pixel->y = y;
			//~ drawPixel(pixel);
			game.screen[800 * y + x] = heartPtr[i];
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
			
			//~ pixel->color = numPtr1[i];
			//~ pixel->x = x;
			//~ pixel->y = y;
			//~ drawPixel(pixel);
			game.screen[800 * y + x] = numPtr1[i];
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
			
			//~ pixel->color = numPtr2[i];
			//~ pixel->x = x;
			//~ pixel->y = y;
			//~ drawPixel(pixel);
			game.screen[800 * y + x] = numPtr2[i];
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
			
			//~ pixel->color = numPtr3[i];
			//~ pixel->x = x;
			//~ pixel->y = y;
			//~ drawPixel(pixel);
			game.screen[800 * y + x] = numPtr3[i];
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
			
			//~ pixel->color = numPtr4 [i];
			//~ pixel->x = x;
			//~ pixel->y = y;
			//~ drawPixel(pixel);
			game.screen[800 * y + x] = numPtr4[i];
			i++;

		}
	}
	drawGameScreen(pixel);

	exit(-1);
}

void drawGameWon(Pixel *pixel)
{
	short int *gameWonPtr = (short int *) gameWin.pixel_data;
	int i = 0;
	for (int y = 245; y < 545; y++)
	{
		for (int x = 202; x < 602; x++) 
		{	 
			//~ pixel->color = gameWonPtr[i];
			i++;
			//~ pixel->x = x;
			//~ pixel->y = y;
			//~ drawPixel(pixel);
			game.screen[800 * y + x] = gameWonPtr[i];
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
			//~ pixel->color = gameLostPtr[i];
			//~ i++;
			//~ pixel->x = x;
			//~ pixel->y = y;
			//~ drawPixel(pixel);
			game.screen[800 * y + x] = gameLostPtr[i];
			i++;
		}
		
	}
	
	
	drawGameOverScore(pixel);
	
}

bool isGameWon(Pixel *pixel)
{
	int castleXStart[] = {241, 309, 375, 441, 507};
	int castleXEnd[] = {295, 365, 431, 497, 561};
	if (frog.lane == 1)
	{
		for (int i = 0; i <= 4; i++)
		{
			if (castleXStart[i] <= frog.position && castleXEnd[i] >= frog.position + 33)
			{
				if (i == 0)
				{
					drawFrog(pixel, 255, 800, frog.lane, -1);
				}
				else if (i == 1)
				{
					drawFrog(pixel, 321, 800, frog.lane, -1);
				}
				else if (i == 2)
				{
					drawFrog(pixel, 387, 800, frog.lane, -1);
				}
				else if (i == 3)
				{
					drawFrog(pixel, 453, 800, frog.lane, -1);
				}
				else if (i == 4)
				{
					drawFrog(pixel, 519, 800, frog.lane, -1);
				}
				return true;
			}
		}
		drawGameLost(pixel);
		
	}
	return false;
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
			
			//~ pixel->color = numPtr1[i];
			//~ pixel->x = x;
			//~ pixel->y = y;
			//~ drawPixel(pixel);
			game.screen[800 * y + x] = numPtr1[i];
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
			
			//~ pixel->color = numPtr2[i];
			//~ pixel->x = x;
			//~ pixel->y = y;
			//~ drawPixel(pixel);
			game.screen[800 * y + x] = numPtr2[i];
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
			
			//~ pixel->color = numPtr3[i];
			//~ pixel->x = x;
			//~ pixel->y = y;
			//~ drawPixel(pixel);
			game.screen[800 * y + x] = numPtr3[i];
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
			
			//~ pixel->color = numPtr4 [i];
			//~ pixel->x = x;
			//~ pixel->y = y;
			//~ drawPixel(pixel);
			
			game.screen[800 * y + x] = numPtr4[i];
			i++;

		}
	}
	//cleanBackground(pixel, xStart, yStart, xEnd, yEnd);

	
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
				
				//~ pixel->color = numPtr1[i];
				//~ pixel->x = x;
				//~ pixel->y = y;
				//~ drawPixel(pixel);
				
				game.screen[800 * y + x] = numPtr1[i];
				i++;

			}
		}
	}
	
	if (live == 0)
	{
		drawGameLost(pixel);
	}
	
	//~ drawGameScreen(pixel);

	
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
				
				//~ pixel->color = numPtr1[i];
				//~ pixel->x = x;
				//~ pixel->y = y;
				//~ drawPixel(pixel);
				game.screen[800 * y + x] = numPtr1[i];
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
				
				//~ pixel->color = numPtr2[i];
				//~ pixel->x = x;
				//~ pixel->y = y;
				//~ drawPixel(pixel);
				game.screen[800 * y + x] = numPtr2[i];
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
				
				//~ pixel->color = numPtr3[i];
				//~ pixel->x = x;
				//~ pixel->y = y;
				//~ drawPixel(pixel);
				game.screen[800 * y + x] = numPtr3[i];
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
				
				//~ pixel->color = numPtr1[i];
				//~ pixel->x = x;
				//~ pixel->y = y;
				//~ drawPixel(pixel);
				game.screen[800 * y + x] = numPtr1[i];
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
				
				//~ pixel->color = numPtr2[i];
				//~ pixel->x = x;
				//~ pixel->y = y;
				//~ drawPixel(pixel);
				game.screen[800 * y + x] = numPtr2[i];
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
				
				//~ pixel->color = numPtr3[i];
				//~ pixel->x = x;
				//~ pixel->y = y;
				//~ drawPixel(pixel);
				game.screen[800 * y + x] = numPtr3[i];
				i++;

			}
		}
	}
	
	if (secondRemaining == 0)
	{
		drawGameLost(pixel);
	}
	
}

void *ImageThread(){
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int maxScreenX = 800;
	int maxScreenY = 800;
	bool collided = false;
	int valuePackDrawn = 0;
	bool gotHeart = false;
	int heartCount = 0;

	
	int car[6], log[6], snake[6], tweed[6];
	generateRandomStartPos(car, 1);
	int carLane[] = {16, 16, 15, 15, 14, 14};
	int carDir[] = {RIGHT, RIGHT, LEFT, LEFT, RIGHT, RIGHT};
	
	
	generateRandomStartPos(log, 2);
	int logLane[] = {4, 4, 3, 3, 2, 2};
	int logDir[] = {LEFT, LEFT, RIGHT, RIGHT, LEFT, LEFT};
	bool onLog = false;
	
	generateRandomStartPos(snake, 2);
	int snakeLane[] = {12,12, 11, 11, 10, 10};
	int snakeDir[] = {LEFT, LEFT, RIGHT, RIGHT, LEFT, LEFT};
	
	generateRandomStartPos(tweed, 1);
	int tweedLane[] = {8, 8, 7, 7, 6, 6};
	int tweedDir[] = {RIGHT, RIGHT, LEFT, LEFT, RIGHT, RIGHT};

	while(1){ 
			for (int i = 0; i < 6; i++){
				if(carLane[i] == 16 && car[i] >= 563){
					car[i] = 208;
				}
				else if(carLane[i] == 15 && car[i] <= 208){
					car[i] = 562;
				}
				else if(carLane[i] == 14 && car[i] >= 563 ){
					car[i] = 166;
				}
				drawObj(pixel, &car[i], maxScreenX, carLane[i], carDir[i]);
				
				if(logLane[i] == 2 && log[i] <= 190){
					log[i] = 562;
				}
				else if(logLane[i] == 3 && log[i] >= 563){
					log[i] = 166;
				}
				else if(logLane[i] == 4 && log[i] <= 165){
					log[i] = 562;
				}
				drawObj(pixel, &log[i], maxScreenX, logLane[i], logDir[i]);
				
				if(snakeLane[i] == 10 && snake[i] <= 191){
					snake[i] = 591;
				}	
				else if(snakeLane[i] == 11 && snake[i] >= 563){
					snake[i] = 191;
				}
				else if(snakeLane[i] == 12 && snake[i] <=191){
					snake[i] = 591;
				}
				drawObj(pixel, &snake[i], maxScreenX, snakeLane[i], snakeDir[i]);
				
				if(tweedLane[i] == 8 && tweed[i] >= 563){
					tweed[i] = 208;
				}
				else if(tweedLane[i] == 7 && tweed[i] <= 208){
					tweed[i] = 562;
				}
				else if(tweedLane[i] == 6 && tweed[i] >= 563 ){
					tweed[i] = 208;
				}
				drawObj(pixel, &tweed[i], maxScreenX, tweedLane[i], tweedDir[i]);
			
			}
		
			
			
			drawFrog(pixel, frog.position, maxScreenX, frog.lane, frog.direction);	
			for (int i = 0; i < 6; i++){
				collided = checkCollision(pixel, frog.position, car[i], frog.lane, carLane[i]);
				if(collided){
					resetFrog(pixel);
				}
				collided = checkCollision(pixel, frog.position, tweed[i], frog.lane, tweedLane[i]);
				if(collided){
					resetFrog(pixel);
				}
				
				collided = checkCollision(pixel, frog.position, snake[i], frog.lane, snakeLane[i]);
				if(collided){
					resetFrog(pixel);
				}
				onLog = checkOnLog(pixel, log[i], logLane[i], logDir[i]);
				if(onLog){
					break;
				}
			}
			if(frog.lane >= 2 && frog.lane <= 4 && onLog == false){
				frog.onLog = false;
				resetFrog(pixel);
			}
			
		
			
			time_t finish = time(NULL);
			frog.endTime = finish - frog.startTime;
			
			
			if (frog.endTime == 5)
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
		
			
			drawGameScreen(pixel);
	
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
		srand(time(NULL));
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	
	unsigned int *gpioPtr = getGPIOPtr();  

	drawMainMenu(pixel);
	drawGameScreen(pixel);

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
	
	drawGameBoard(pixel);
	drawGameScreen(pixel);
	
	frog.position = 387;
    frog.lane = 17;
    frog.direction = -1;
    frog.lives = 4;
	frog.moves = 200;
	frog.time = 200;
	frog.maxLane = 17;
		//create car thread
	int rc;
	pthread_t imageRenderThread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	rc = pthread_create(&imageRenderThread, &attr, ImageThread, NULL);
	
	//~ while(1);

	
	//~ int maxScreenX = 800;
	//~ int maxScreenY = 800;
	//~ frog.lane = 17;
	//~ int carLane = 5;

	
	//~ int currentDirection = 1;
	//~ int frogDirection;

	//~ frog.position = 373;
	//~ int testPoint = 800;
	//~ int currentCode = -2;	 
	//~ int cars[] = {561, 198, 198};
	//~ int carLanes[] = {15,16,14};
	//~ int carPos;
	//~ drawMainMenu(pixel);
	//~ drawGameScreen(pixel);
	//~ bool buttonPressed = false;	
	  //~ // get gpio pointer
   // unsigned int *gpioPtr = getGPIOPtr();  

	//~ drawFrog(pixel, frog.position, maxScreenX, frog.lane, -1);
	while(1){ 
			delayMicroseconds(90000);
			frog.direction = getDirectionFromCon(&frog.lane, &frog.position, gpioPtr);		
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




