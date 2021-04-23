//Shailly Patel - 30088139
//Parth Patel - 30096473

//Project Part 2 - Frogger Game Implementation 
//Description: 	Upon running the make file. user can start the game and 
//				play Frogger. They have 4 challenges to cross: road, forest
//				desert, and river. If they touch the water, they lose a life.
//				If they touch the water by the castle lane, they lose the game.
//				The frog must be completely in the castle to win.

//include required libraries
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
#include "gameMenu.h"

//define common used macros
#define UP 4
#define RIGHT 7
#define DOWN 5
#define LEFT 6
#define PAUSE 3

//Reference point for the lanes, where the top most lane is lane0 and the starting frog lane is lane17
#define REF_POINT 152

/* Definitions */
typedef struct {
	short int color;
	int x, y;
} Pixel;

//Game structure to store current info of the game
struct Game{
	short int screen[800*800*2];
	short int pausescreen[400*300*2];
	bool pause;
	time_t pauseDuration;
	bool finished;
}game;

struct fbs framebufferstruct;
void drawPixel(Pixel *pixel);
void restartGame(Pixel *pixel);
void goToMainMenu(Pixel *pixel);

//frog structure to store current info of the frog
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

//image struct to store current info of the heart value pack
struct image 
{
	int lane;
	int startX;
	int startY;
	int valuePackDrawn;
	bool gotHeart;
	int heartCount;
};

struct shared frog;
struct image heartPack;

/* Draw a pixel */
void drawPixel(Pixel *pixel){
	long int location =(pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	*((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
}

//parameters: 	pixel
//return: 		void
//description:	draw the game screen from the pointer storing the image
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


//parameters: 	pixel
//return: 		void
//description:	copy the pixels for the game screen to the game
//				screen pointer
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

//parameters: 	pixel
//return: 		void
//description:	copy the pixels for the main menu screen to the game
//				screen pointer
void drawMainMenu(Pixel *pixel){
	
	short int *menuPtr = (short int *) MenuScreen.pixel_data;
	int i = 0;
	for (int y = 0; y < 800; y++)
	{
		for (int x = 0; x < 800; x++) 
		{	 
			game.screen[i] = menuPtr[i];
			i++;

		}
	}
}
//parameters: 	pixel
//return: 		void
//description:	draw the game menu screen from the pointer storing the image
void drawGameMenuScreen( Pixel *pixel){
	int i = 0;
	for (int y = 245; y < 545; y++)
	{
		for (int x = 202; x < 602; x++) 
		{	 
			pixel -> x = x;
			pixel -> y = y;
			pixel -> color = game.pausescreen[i];
			i++;
			drawPixel(pixel);
		}
	}
	
}
//parameters: 	pixel
//return: 		void
//description:	copy the pixels for the game menu screen to the game
//				pause screen pointer
void drawGameMenu(Pixel *pixel){
	short int *gamePausePtr = (short int *) gameMenu.pixel_data;
	int i = 0;
	for (int y = 245; y < 545; y++)
	{
		for (int x = 202; x < 602; x++) 
		{	 
			game.pausescreen[i] = gamePausePtr[i];
			i++;
		}
	}
}

//parameters: 	pixel, coordinates of the screen section needed to be clean
//return: 		void
//description:	clean the game screen by the specified coordinates
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


//parameters: 	pixel
//return: 		void
//description:	resets the frog to original position when frog collides
//				into an object. Frog will also lose life.
void resetFrog(Pixel *pixel){
	//clean the dead frog from screen adn reset frog position
	cleanBackground(pixel, frog.position,  REF_POINT + 35*frog.lane, frog.position + 33 ,  REF_POINT + 35*frog.lane + 33);
	frog.position = 400;
	frog.lane = 17;
	frog.direction = -1;
	frog.lives -= 1;

}

//parameters: 	frog's current lane, frog x position, gpio pointer
//return: 		int, the button code pressed by the user
//description:	gets the button pressed by the user, and changes the 
//				frog accoringly. Also updates the maximum lane the frog 
//				has reached.
int getDirectionFromCon(unsigned int *gpioPtr){
	int buttonCode = getButtonPressed(gpioPtr);
	
	//move the frog accordingly to how the button is pressed
	if (buttonCode == 5){
		if(frog.lane < 17){
			frog.lane += 1;
			frog.moves -= 1;
		}
	}
	
	else if (buttonCode == 4){
		if(frog.lane > 0){
			frog.lane-= 1;
			frog.moves -= 1;
		}
	}
	else if (buttonCode == 6){
		if(frog.position >= 264){
			frog.position -= 33;
			frog.moves -= 1;
		}
	}
	else if (buttonCode == 7){
		if(frog.position <= 528){
			frog.position += 33;
			frog.moves -= 1;
		}
	}
	
	if (frog.lane < frog.maxLane)
	{
		frog.maxLane = frog.lane;
	}

	return buttonCode;

}

//parameters: pixel
//returns: void
//description: update the gameScreen array with the prog array based on the lane and position and clean the background
void drawFrog(Pixel *pixel){
	short int *frogPtr=(short int *) froggie.pixel_data;
	int i =0;
	int x, y;
	int laneSize = 35;
	int objSize = 33;
	
	//the x, y co-ordinates for the clean background
	int xCordCleanStart;
	int xCordCleanEnd;
	int yCordCleanStart;
	int yCordCleanEnd;
	
	//variables for the frog postion, direction and lane
	int xStartingPoint = frog.position; 
	int direction = frog.direction;
	int frogLane = frog.lane;
	
	//reference point is 152 
	for (y = REF_POINT + (laneSize*frogLane); y < REF_POINT + laneSize + (laneSize*frogLane); y++)
	{
		for ( x = xStartingPoint; x < (objSize + xStartingPoint); x++) 
		{	
			
			if (y < REF_POINT + objSize + laneSize*frogLane){
				//draws the frog inside the screen and ignores the backgrounds of the frog image
				if(frogPtr[i] >= -5000){
					game.screen[800*y + x] = frogPtr[i];
				}
				i++;
			}
		}
	}
	
	//set the x,y co-ordinated and clean the background based on the direction
	if (direction >= 0){
		if (direction == RIGHT){
			xCordCleanStart = xStartingPoint - objSize;
			xCordCleanEnd = xStartingPoint-1;
			yCordCleanStart = REF_POINT + laneSize*frogLane;
			yCordCleanEnd = yCordCleanStart + objSize;
		}
		else if (direction == LEFT){
			xCordCleanStart = x;
			xCordCleanEnd = xCordCleanStart + objSize;
			yCordCleanStart = REF_POINT + laneSize*frogLane;
			yCordCleanEnd = yCordCleanStart + objSize;
		}
		if(direction == UP){
			xCordCleanStart = xStartingPoint;
			xCordCleanEnd = x - 1;
			yCordCleanStart = laneSize + laneSize*frogLane + REF_POINT;
			yCordCleanEnd = objSize + yCordCleanStart;	
		}
		else if(direction == DOWN){
			xCordCleanStart = xStartingPoint;
			xCordCleanEnd = x - 1;
			yCordCleanStart = (laneSize*frogLane) - laneSize + REF_POINT;
			yCordCleanEnd = objSize + yCordCleanStart ;
		}
		cleanBackground(pixel, xCordCleanStart, yCordCleanStart, xCordCleanEnd, yCordCleanEnd);
	}
}

//parameters: pixel, objStart, objectLane, direction
//returns: void
//decription: draw the object, car, snake, log, tumbleweed based on lane number
void drawObj(Pixel *pixel, int* objStart, int objectLane, int direction){
	short int *objectPtr = 0;
	int length = 33;
	int width = 33;
	
	//draw the object based on lane
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
			length = 60;
		}	
	}
	
	else if (objectLane >= 10 && objectLane <=12){
		if (direction == RIGHT){
			objectPtr =(short int *) snake2.pixel_data;
			length = 50;
		}
		else if(direction == LEFT){
			objectPtr =(short int *) snake.pixel_data;
			length = 50;
		}
	
	}
	else if (objectLane >= 6 && objectLane <=8){
		objectPtr =(short int *) tumbleWeed.pixel_data;
		length = 33;
		

	
	}
	
	//store the object array in the game array
	int i =0;
	int x, y;
	int laneSize = 35;
	int xCordCleanStart;
	int xCordCleanEnd;
	int yCordCleanStart;
	int yCordCleanEnd;
	int startPoint = *objStart;
	int yStart = REF_POINT + (laneSize*objectLane);
	int yEnd = REF_POINT + (laneSize + (laneSize*objectLane));
	
	for (y = yStart; y < yEnd; y++)
	{
		for (x = startPoint; x < (length + startPoint); x++) 
		{	
			
			if (y <  REF_POINT + (width + (laneSize * objectLane))){
		
				if (x > 241 && x<= 561 ) {
					
					game.screen[800 * y + x] = objectPtr[i];
				
				}
				i++;
			}
		}
	}

	//based on direciton, set its speed to their repective directions, and if the frog in on the log, set the frog speed to the log speed 
	if (direction == RIGHT){
		int speed = 2; 
		*objStart +=speed;
		if(frog.lane == objectLane){
			frog.speed = speed;
		}
		xCordCleanStart = startPoint - speed;
		xCordCleanEnd = startPoint - 1;
		yCordCleanStart = laneSize*objectLane  + REF_POINT;
		yCordCleanEnd = yCordCleanStart + width;
	}
	else if (direction == LEFT){
		int speed = 1 ; 
		*objStart -= speed;
		if(frog.lane == objectLane){
			frog.speed = speed;
		}
		xCordCleanStart = x;
		xCordCleanEnd = x + speed - 1;
		
		yCordCleanStart = laneSize*objectLane + REF_POINT;
		yCordCleanEnd = yCordCleanStart + width;
	}
	cleanBackground(pixel, xCordCleanStart, yCordCleanStart , xCordCleanEnd, yCordCleanEnd );
	
}

//parameters: pixel, objectPosition, objectLane
//returns: boolean
//description: if the frog collides with an object, return true else return false
bool checkCollision(Pixel *pixel, int objectPosition, int objectLane){
	int objectSize; 
	
	//define object size based on the lane number
	if(objectLane == 14)
	{
		objectSize = 70;
	}
	else if (objectLane >= 10 && objectLane <= 12)
	{
		objectSize = 50;
	}
	else
	{
		objectSize = 33;
	}
	
	//check the difference between the locations 
	//if the frog touches the object by even 1 pixel, collision has occured
	if (frog.lane == objectLane){
		int locationDifference = frog.position - objectPosition;
		if (locationDifference < objectSize && locationDifference > -33){
			return true;
		}
	}
	return false;
}

//parameters: pixel, objectPosition, objectLane, direction
//returns: boolean
//description: check if the frog is on log, in which case the frog position is updated by the speed
bool checkOnLog(Pixel *pixel, int objectPosition, int objectLane, int direction){
	int objectSize = 75;
	
	if (objectLane == 2){
		objectSize = 60;
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

//generate random integer for the object start point
int generateRandInt(){
	return rand() % 300 + 241;	
	
}

//generate a 2nd random integer for the 2nd object start point
int generateRandInt2(){
	return 160 + rand() % 40;	
}

//parameters: objectStartPointArray, identifier
//returns: void
//decription: generate random start points for the objects 
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

//parameters: 	none
//return: 		bool, if frog has touched the heart
//description:	return true if frog has obtained the heart
//				return false if frog has not touched the heart
bool isHeartObtained ()
{
	int objectSize = 33; 
	if (frog.lane == heartPack.lane)
	{
		//check if frog has collided with the heart image (pixel overlap)
		int locationDifference = frog.position - heartPack.startX;
		if (locationDifference < objectSize && locationDifference > -33){
			return true;
		}
	}
	return false;

}

//parameters: 	pixel
//return: 		void
//description:	draws the heart into a random safe lane at a random position
void drawHeart (Pixel *pixel)
{
	int xStart = 0;
	int yStart = 0;
	int xEnd = 0;
	int yEnd = 0;

	short int *heartPtr=(short int *) heart.pixel_data;

	int r = (rand() % 4) + 1; //random number btn 1-4
	int randomLane = (r * 4) + 1; //get the random safe lane based off of random number
	xStart = (rand() % (522 + 1 - 245)) + 245; //get random x postion 
	
	//depending on the lane, initialize the y coordinate.
	yStart = REF_POINT + (35*randomLane);
	
	heartPack.lane = randomLane;
	heartPack.startX = xStart;
	heartPack.startY = yStart;
	
	
	yEnd = yStart + 33;
	xEnd = xStart + 33;
	int i = 0;
	
	//draw frog
	for (int y = yStart; y < yEnd; y++)
	{
		for (int x = xStart; x < xEnd; x++) 
		{	
			game.screen[800 * y + x] = heartPtr[i];
			i++;

		}
	}
		
	
}

//parameters: 	number of the pointer image we want
//return: 		short int *, the pointer of the image
//description:	checks the number passed, and returns the pointer of its
//				corressponding image
short int * getNumPtr (int num)
{
	
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

//parameters: 	pixel, the coordinates of where to print the image and
//				the pointer of the image to be displayed
//return: 		void
//description:	draw the image stored in the ptr
void updateScreenInfo (Pixel *pixel, int xStart, int yStart, int xEnd, int yEnd, short int *ptr)
{
	int i = 0;
	for (int y = yStart; y < yEnd; y++)
	{
		for (int x = xStart; x < xEnd; x++) 
		{	
			game.screen[800 * y + x] = ptr[i];
			i++;

		}
	}
}

//parameters: 	pixel
//return: 		void
//description:	draw the final score of the player after they either win or lose the game
//				the final score is calcuated as follows...
//				final score = 10(remaining moves + remaining time + remaining live) + laneScore
void drawGameOverScore (Pixel *pixel)
{
	time_t remaining = 200 - frog.endTime;
	frog.score += (frog.moves + remaining + frog.lives)*10; //calculate final score
	
	//calculate digit from the place values 
	int first = frog.score / 1000;
	
	int second = frog.score / 100;
	second %= 10;
	
	int third = frog.score / 10;
	third %= 10;
	
	int fourth = frog.score % 10;
	
	//pointer storing image of first place value
	short int *numPtr1 = getNumPtr(first);

	int yStart = 200 + 250;
	int yEnd = yStart+33;
	int xStart = 203 + 202;
	int xEnd = xStart+33;
	updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr1); //draw image

	
	//second Value
	short int *numPtr2 = getNumPtr(second);
	
	xStart = 237 + 202;
	xEnd = xStart+33;
	updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr2);

	//third Value
	short int *numPtr3  = getNumPtr(third);
	
	xStart = 271+ 202;
	xEnd = xStart+33;
	updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr3);

	
	//fourth Value
	short int *numPtr4 = getNumPtr(fourth);
	
	xStart = 305+ 202;
	xEnd = xStart+33;
	updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr4);

	drawGameScreen(pixel);

}

//parameters: 	pixel
//return: 		void
//description:	draw the game won message
void drawGameWon(Pixel *pixel)
{
	drawGameScreen(pixel);
	bool buttonPressed = false;
	short int *gameWonPtr = (short int *) gameWin.pixel_data;
	int i = 0;
	for (int y = 245; y < 545; y++)
	{
		for (int x = 202; x < 602; x++) 
		{	 
			game.screen[800 * y + x] = gameWonPtr[i];
			i++;
		}
		
	}
	
	
	unsigned int *gpioPtr = getGPIOPtr();  
	
	drawGameOverScore(pixel);
	do{
		delayMicroseconds(160000);
		int buttonCode = getButtonPressed(gpioPtr);
		if(buttonCode >= 0)
		{
			buttonPressed = true;
		}
	}while(!buttonPressed);
	
	goToMainMenu(pixel);
	restartGame(pixel);
}

//parameters: 	pixel
//return: 		void
//description:	draw the game lost message
void drawGameLost(Pixel *pixel)
{
	drawGameScreen(pixel);
	bool buttonPressed = false;
	short int *gameLostPtr = (short int *) gameLost.pixel_data;
	int i = 0;
	for (int y = 245; y < 545; y++)
	{
		for (int x = 202; x < 602; x++) 
		{	 
			game.screen[800 * y + x] = gameLostPtr[i];
			i++;
		}
		
	}
	
	unsigned int *gpioPtr = getGPIOPtr();  
	
	drawGameOverScore(pixel);
	do{
		delayMicroseconds(160000);
		int buttonCode = getButtonPressed(gpioPtr);
		if(buttonCode >= 0)
		{
			buttonPressed = true;
		}
	}while(!buttonPressed);
	
	goToMainMenu(pixel);
	restartGame(pixel);
	
}

//parameters: 	pixel
//return: 		bool, returns true if game is won, false if game is not won
//description:	draw the game lost message
bool isGameWon(Pixel *pixel)
{
	//range of the castle 
	int castleXStart[] = {245, 311, 377, 433, 509};
	int castleXEnd[] = {294, 360, 426, 492, 558};
	
	if (frog.lane == 1)
	{
		for (int i = 0; i <= 4; i++)
		{
			if (castleXStart[i] <= frog.position && castleXEnd[i] >= frog.position + 33)
			{
				drawFrog(pixel);
				
				return true;
			}
		}
		//if not in the castle range, then in the water.. so game lost
		drawGameLost(pixel);
		
	}
	return false;
}

//parameters: 	pixel
//return: 		void
//description:	draw the lane score (player gets 10 points for each new lane)
void updateScore (Pixel *pixel)
{
	
	frog.score = 170 - (frog.maxLane * 10);
	
	//calculate digit from the place values 
	int first = frog.score / 1000;
	
	int second = frog.score / 100;
	second %= 10;
	
	int third = frog.score / 10;
	third %= 10;
	
	int fourth = frog.score % 10;
	
	
	//first Value
	short int *numPtr1 = getNumPtr(first);
	
	int yStart = 170;
	int yEnd = 170+33;
	int xStart = 615;
	int xEnd = xStart+33;
	updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr1);
	
	//second Value
	short int *numPtr2 = getNumPtr(second);
	
	xStart = 649;
	xEnd = xStart+33;
	updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr2);
	
	//third Value
	short int *numPtr3  = getNumPtr(third);
	
	xStart = 683;
	xEnd = xStart+33;
	updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr3);
	
	//fourth Value
	short int *numPtr4 = getNumPtr(fourth);
	
	xStart = 717;
	xEnd = xStart+33;
	updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr4);

}

//parameters: 	pixel
//return: 		void
//description:	draw the remaining lives on the screen
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
		updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr1);
	}
	
	if (live == 0)
	{
		drawGameLost(pixel);
	}
	
}

//parameters: 	pixel
//return: 		void
//description:	draw the remaining moves left on the screen
void updateSteps (Pixel *pixel)
{
	
	int steps = frog.moves;
	
	if (steps >= 0)
	{
		//calculate digit from the place values 
		int first = steps / 100;
		
		int second = steps / 10;
		second %= 10;
		
		int third = steps % 10;
		
		short int *numPtr1 = getNumPtr(first);
		
		int yStart = 380;
		int yEnd = yStart+33;
		int xStart = 35;
		int xEnd = xStart+33;
		updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr1);
		
		
		//second Value
		short int *numPtr2 = getNumPtr(second);
		
		xStart = 69;
		xEnd = xStart+33;
		updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr2);
		
		//third Value
		short int *numPtr3 = getNumPtr(third);
		
		xStart = 103;
		xEnd = xStart+33;
		updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr3);
	}
	
	if (steps == 0)
	{
		drawGameLost(pixel);
	}
	
}

//parameters: 	pixel
//return: 		void
//description:	draw the remaining time on the screen
void updateTime (Pixel *pixel)
{
	int secondRemaining = 150 - frog.endTime;
	
	if (secondRemaining >= 0)
	{
		//calculate digit from the place values 
		int first = secondRemaining / 100;
		
		int second = secondRemaining / 10;
		second %= 10;
		
		int third = secondRemaining % 10;
		
		short int *numPtr1 = getNumPtr(first);
		
		int yStart = 595;
		int yEnd = yStart+33;
		int xStart = 35;
		int xEnd = xStart+33;
		updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr1);
		
		
		//second Value
		short int *numPtr2 = getNumPtr(second);
		
		xStart = 69;
		xEnd = xStart+33;
		updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr2);
		
		//third Value
		short int *numPtr3 = getNumPtr(third);
		
		xStart = 103;
		xEnd = xStart+33;
		updateScreenInfo(pixel, xStart, yStart, xEnd, yEnd, numPtr3);
	}
	
	if (secondRemaining == 0)
	{
		drawGameLost(pixel);
	}
	
}

//parameters: 	pixel, coordinates of the screen to be cleared
//return: 		void
//description:	draw the remaining time on the screen
void cleanScreenMenu(Pixel *pixel, int xStart, int yStart, int xEnd, int yEnd){
	short int *pausePtr = (short int *) gameMenu.pixel_data;
	int i = 0;

	for(int x = xStart; x <= xEnd; x++){
		for(int y = yStart; y <= yEnd; y++){
			
			i ++;
			pixel->x = x;
			pixel->y = y;
			pixel->color = pausePtr[i];
			drawPixel(pixel);
			
		}
	}
}

//parameters: 	pixel, coordinates of the screen to underline, and second line coordinates
//return: 		void
//description:	draw the remaining time on the screen
void underline(Pixel *pixel, int startingX, int endingX, int y, int line2startX, int line2endX, int line2y)
{
	//position the coordinates in reference to the center
	startingX += 202;
	endingX += 202;
	y += 250;
	line2startX += 202;
	line2endX += 202;
	line2y += 250;
	
	//draw the line 
	for (int x = startingX; x <= endingX; x++)
	{
		pixel->color = 0xFFF;
		pixel->x = x;
		pixel->y = y;
		drawPixel(pixel);
	}
	
	//clean the other line
	cleanScreenMenu(pixel, line2startX, line2y, line2endX, line2y);
	
}

//parameters: 	pixel, coordinates the clean the menu screen 
//return: 		void
//description:	clean the screen (draw original screen) in the specified coordinates
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

//parameters: 	pixel
//return: 		void
//description:	underline the start button on the main menu screen
void underlineStart(Pixel *pixel)
{
	int y = 360;
	int startingX = 176;
	int endingX = 612;
	
	//draw underline
	for (int x = startingX; x <= endingX; x++)
	{
		pixel->color = 0xF;
		pixel->x = x;
		pixel->y = y;
		drawPixel(pixel);
	}

	//clean the underline on the quit button
	cleanMenu(pixel, 194, 499, 586, 499);
	
}

//parameters: 	pixel
//return: 		void
//description:	underline the quit button on the main menu screen
void underlineQuit(Pixel *pixel)
{
	int y = 499;
	int startingX = 194;
	int endingX = 586;
	
	//draw underline
	for (int x = startingX; x <= endingX; x++)
	{
		pixel->color = 0xF;
		pixel->x = x;
		pixel->y = y;
		drawPixel(pixel);
	}
	
	//clean the underline on the start button
	cleanMenu(pixel, 176, 360, 612, 360);
}

//parameters: 	pixel
//return: 		void
//description:	restart the game state
//				initialize everything to what it would be like at the start of the game
void restartGame(Pixel *pixel)
{
	game.pauseDuration = 0;
	frog.startTime = time(NULL);
	frog.position = 400;
	frog.lane = 17;
	frog.direction = -1;
	frog.lives = 4;
	frog.moves = 200;
	frog.time = 150;
	frog.maxLane = 17;
	heartPack.valuePackDrawn = 0;
	heartPack.gotHeart = false;
	heartPack.heartCount = 0;
	game.finished = false;
	drawGameBoard(pixel);
}

//parameters: 	pixel
//return: 		void
//description:	goes to the main menu screen
//				waits for user to either start the game or quit the game
void goToMainMenu(Pixel *pixel)
{
	unsigned int *gpioPtr = getGPIOPtr();
	drawMainMenu(pixel);
	drawGameScreen(pixel);

	bool start = false;
	int buttonCode = 0;
	
	do
	{
		delayMicroseconds(120000);
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
		exit(-1);
		
	}
}    

//parameters: 	pixel, gpio pointer
//return: 		void
//description:	function performs the actions during a paused screen:
//				draw the game menu screen
//				calculate the seconds of game being paused
void pauseScreen(Pixel *pixel, unsigned int *gpioPtr)
{
	
	time_t pauseStart = time(NULL);
			
	drawGameMenu(pixel);
	drawGameMenuScreen(pixel);
	game.pause = true;
	int buttonCode =-1;
	bool restart = false;
	bool exitGame = false;
	do
	{
		delayMicroseconds(160000);
		buttonCode = getButtonPressed(gpioPtr);
		
		if (buttonCode == 3 || buttonCode == 8)
		{
			game.pause = false; //exit the loop
		}
		
		if (buttonCode == 4)//up
		{
			underline(pixel, 42, 356, 149, 90, 324, 225);
			restart = true;
		}
		else if (buttonCode == 5)//down
		{
			underline(pixel, 90, 324, 225, 42, 356, 149);
			exitGame = true;
		}

		
	}while (game.pause);
	
	time_t pauseEnd = time(NULL);
	game.pauseDuration = game.pauseDuration + (pauseEnd - pauseStart); //calculate TOTAl paused duration
	
	if (buttonCode != 3)
	{
		if(restart)
		{
			game.finished = true;
		}
		
		else if(exitGame){
			goToMainMenu(pixel);
			restartGame(pixel);
		}
	}

	
}

//Frog thread for the frog, to get button pressed from the controller
void *FrogThread(){
	unsigned int *gpioPtr = getGPIOPtr(); 
	while(1)
	{
		if (game.pause == false)
		{
			delayMicroseconds(120000);
			frog.direction = getDirectionFromCon(gpioPtr);
		}
		while(game.finished);
	}


}

//Image thread for the image rendering and draw the final output
void *ImageThread(){
	while(1){
		while(game.finished);
		unsigned int *gpioPtr = getGPIOPtr();  
		Pixel *pixel;
		pixel = malloc(sizeof(Pixel));
		bool collided = false;
		//~ int valuePackDrawn = 0;
		//~ bool gotHeart = false;
		//~ int heartCount = 0;
		heartPack.valuePackDrawn = 0;
		heartPack.gotHeart = false;
		heartPack.heartCount = 0;
		
		
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

		while(!game.finished){ 
			//~ valuePackDrawn = 0;
			//~ gotHeart = false;
			//heartCount = 0;
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
				drawObj(pixel, &car[i],  carLane[i], carDir[i]);
				
				if(logLane[i] == 2 && log[i] <= 180){
					log[i] = 562;
				}
				else if(logLane[i] == 3 && log[i] >= 563){
					log[i] = 166;
				}
				else if(logLane[i] == 4 && log[i] <= 165){
					log[i] = 562;
				}
				drawObj(pixel, &log[i],  logLane[i], logDir[i]);
				
				if(snakeLane[i] == 10 && snake[i] <= 191){
					snake[i] = 591;
				}	
				else if(snakeLane[i] == 11 && snake[i] >= 563){
					snake[i] = 191;
				}
				else if(snakeLane[i] == 12 && snake[i] <=191){
					snake[i] = 591;
				}
				drawObj(pixel, &snake[i], snakeLane[i], snakeDir[i]);
				
				if(tweedLane[i] == 8 && tweed[i] >= 563){
					tweed[i] = 208;
				}
				else if(tweedLane[i] == 7 && tweed[i] <= 208){
					tweed[i] = 562;
				}
				else if(tweedLane[i] == 6 && tweed[i] >= 563 ){
					tweed[i] = 208;
				}
				drawObj(pixel, &tweed[i], tweedLane[i], tweedDir[i]);
			
			}
		
			
			
			drawFrog(pixel);	
			for (int i = 0; i < 6; i++){
				collided = checkCollision(pixel, car[i], carLane[i]);
				if(collided){
					resetFrog(pixel);
				}
				collided = checkCollision(pixel, tweed[i], tweedLane[i]);
				if(collided){
					resetFrog(pixel);
				}
				
				collided = checkCollision(pixel, snake[i], snakeLane[i]);
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
			
			//calculate the seconds of game played
			time_t finish = time(NULL);
			frog.endTime = finish - frog.startTime - game.pauseDuration; 

			//indicator to draw value pack 30 seconds into the game
			if (frog.endTime == 30)
			{
				heartPack.valuePackDrawn ++;
			}
			
			//update screen
			updateScore(pixel);
			updateLives(pixel);
			updateSteps(pixel);
			updateTime(pixel);
			
			//draw the value pack
			if (heartPack.valuePackDrawn == 1)
			{
				drawHeart(pixel);
			}
			
			//check if heart is obtained if not yet obtained
			if (heartPack.heartCount == 0)
			{
				heartPack.gotHeart = isHeartObtained();
				if (heartPack.gotHeart)
				{
					cleanBackground(pixel, heartPack.startX,  REF_POINT + 35*heartPack.lane, heartPack.startX + 33 ,  REF_POINT + 35*heartPack.lane + 33);
					frog.lives ++;
					heartPack.heartCount = 1;
				}
			}
			
			//if player wants to pause the game
			if (frog.direction == PAUSE)
			{	
				pauseScreen(pixel, gpioPtr);
			}	
						
			//check if game is won
			if (isGameWon(pixel))
			{
				drawGameWon(pixel);
			}
			
			drawGameScreen(pixel);
		
		
		}
	}
}	


/* main function */
int main(){
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	
	
	goToMainMenu(pixel);


	
	srand((unsigned int)time(NULL));
	
	frog.startTime = time(NULL);
	
	drawGameBoard(pixel);
	drawGameScreen(pixel);
	
	//initialize game State
	frog.position = 400;
    frog.lane = 17;
    frog.direction = -1;
    frog.lives = 4;
	frog.moves = 200;
	frog.time = 150;
	frog.maxLane = 17;
	//create Image and frog threads
	pthread_t imageRenderThread;
	pthread_t frogThread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	int rc = pthread_create(&imageRenderThread, &attr, ImageThread, NULL);
	if(rc){
		exit(-1);
	}
	int rc2 = pthread_create(&frogThread, &attr, FrogThread, NULL);
	if(rc2)
	{
		exit(-1);
	}
	
	//if game is finished, restart the game
	while(1){ 
		if (game.finished)
		{	
			restartGame(pixel);
		}
	}
	

			/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	return 0;
}




