 //Parth Patel
//30096473

//Shailly Patel
//30088139

//Feb 11, 2021
//Program Description: The program below interacts with the SNES controllers
//                     It waits for the user to press a button, and then
//                     displays the buttons the user pressed onto the screen.
//                     If user presses the start button, the program terminates.

#include <unistd.h>
#include <stdio.h>
#include "initGPIO.h"
#include <wiringPi.h>
#include <stdlib.h>

//DELETE THIS
//~ void initInput(int pin, unsigned int *gpio)
//~ {
    //~ *(gpio+(pin/10)) = (*(gpio+(pin/10)) & ~(0b111<<(pin%10*3))) | (0b000<<(pin%10*3));
    
//~ }

//~ void initOutput(int pin, unsigned int *gpio)
//~ {
    //~ *(gpio+(pin/10)) = (*(gpio+(pin/10)) & ~(0b111<<(pin%10*3))) | (0b001<<(pin%10*3));
    
//~ }

//set pin 10 to input
            //10%10 = 0 ... so line 0
            //gpioPtr[0] = (gpioPtr[0] & ~(0b111<<0)) | (0b000<<0);
            //initInput(10, gpioPtr);
            
            //set pin 9 to output
            //9/10 = 0 
            //9%10 = 9
            //gpioPtr[0] = (gpioPtr[0] & ~(0b111<<27)) | (0b001<<27);
            //initOutput(9, gpioPtr);
            
            
            //set pin 11 to output
            //11/10 = 1
            //11%10 = 1
            //gpioPtr[1] = (gpioPtr[1] & (0b111<<3)) | (0b001<<3);
            //initOutput(11, gpioPtr);



//function: init_GPIO
//parameters: gpioPtr
//return: none
//functionality: initializes the GPIO line for the SNES
//               set pin 10 to input, set pin 9, 11 to output
void init_GPIO(unsigned int *gpioPtr)
{
    //set pin 10 to input
    gpioPtr[0] = (gpioPtr[0] & ~(0b111<<0)) | (0b000<<0);
    
    
    //set pin 9 to output
    gpioPtr[0] = (gpioPtr[0] & ~(0b111<<27)) | (0b001<<27);
    
    
    //set pin 11 to output
    gpioPtr[1] = (gpioPtr[1] & (0b111<<3)) | (0b001<<3);
}

//function: write_LATCH
//parameters: gpioPtr, bit 
//return: none
//functionality: writes bit to the latch line 
//               bit specifies whether we clear or set pin 9
//               bit = 0 -> clear ... bit = 1 -> set
void write_LATCH(unsigned int *gpioPtr, int bit)
{
    //clear pin 9
    if (bit == 0)
    {
        //offset is 0x28 -> 10 
        gpioPtr[10] = 1<<9;
        
    }
    else if (bit == 1) //set pin 9
    {
        gpioPtr[7] = 1<<9;
    }
    
}

//function: write_CLOCK
//parameters: gpioPtr, bit 
//return: none
//functionality: writes bit to the clock line 
//               bit specifies whether we clear or set pin 11
//               bit = 0 -> clear ... bit = 1 -> set
void write_CLOCK(unsigned int *gpioPtr, int bit)
{
    //clear pin 11
    if (bit == 0)
    {
        gpioPtr[10] = 1<<11;
    }
    else if (bit == 1) //set pin 11
    {
        gpioPtr[7] = 1<<11;
        
    }
    
}

//function: read_DATA
//parameters: gpioPtr
//return: int
//        returns 0 if button is pressed
//        returns 1 if button is not pressed
//functionality: reads bit from data line
int read_DATA(unsigned int *gpioPtr)
{
    return gpioPtr[13] & (1<<10);
}

//function: wait
//parameters: waitTime
//return: none
//functionality: will wait waitTime microseconds 
void wait(int waitTime)
{
    return delayMicroseconds(waitTime);
}

void print_Message(int button)
{
    char * string[] = {"B", "Y", "Sl", "St", "Up", "Down", "Left", "Right", "A", "X", "L", "R", "?", "?", "?", "?", "?" };
    if(button == 3)
    {
        printf("Program is terminating...\n");
        exit(0);
    }
    else
    {
        printf("You have pressed %s\n", string[button]);
    }
}

int read_SNES(unsigned int *gpioPtr)
{

    int value = 0;
    //~ printf("buttonPressed : %d\n", *butPressed); 
    //~ if(*butPressed == 0){
    for(int i = 0; i < 16; i++)
    {

        //delayMicroseconds(6);
        wait(6);

        //clear pin 11
        //gpioPtr[10] = 1<<11;
        write_CLOCK(gpioPtr, 0);

        //delayMicroseconds(6);
        wait(6);


        //value = gpioPtr[13] & (1<<10);
        value = read_DATA(gpioPtr);
        

        if (value == 0 && i < 12)
        {
            return i;
        }
        //set pin 11
        //gpioPtr[7] = 1<<11;
        write_CLOCK(gpioPtr, 1);
    }
    //~ }
    
    return -1;
    
}



int main()
{
    printf("Created by Parth Patel and Shailly Patel\n");
    
    
    // get gpio pointer
    unsigned int *gpioPtr = getGPIOPtr();  
    //printf("Press a button...\n");
    int buttonCode;
    int sameButton = -2;
    //~ while (buttonTracker == 0)
    //~ {
       // buttonCode = 0;
       
       do
       {
       printf("Press a button...\n");
        do
        {
            
            init_GPIO(gpioPtr);
            
            
            //gpioPtr[7] = 1<<9;
            
            //set pin 9
            write_LATCH(gpioPtr, 1);
            
            
            //delay 12 microseconds
            //delayMicroseconds(12);
            wait(12);
            
            //clear pin 9
            //offset is 0x28 -> 10 
            //gpioPtr[10] = 1<<9;
            write_LATCH(gpioPtr, 0);

            
            //printf("butC: %d\n", buttonCode);
            
        
            //readSNES 
	    buttonCode = read_SNES(gpioPtr);
	    //~ printf("buttonCode %d\n", buttonCode);
            if(buttonCode >= 0)
            {
                write_CLOCK(gpioPtr, 1);
		if(buttonCode != sameButton){
		    print_Message(buttonCode);
		    printf("Press a button...\n");
		    //~ wait(180000);
		}
		sameButton = buttonCode;
            }
        
            if (buttonCode == -1){
		sameButton = -2;   
	    }
            //~ if(buttonCode >= 0)
            //~ {
                //~ printf("1\n");
                //~ //buttonTracker = 0;

                //~ break;
            //~ }
        
        }while(1);
        //~ printf("buttonCode2 %d\n", read_SNES(gpioPtr));
        //~ printf("%d\n", buttonTracker);
	//~ printf("HEllo\n");
        //~ buttonTracker = 0;
	//~ butCheck = read_SNES(gp)
        
        //~ printf("World\n");
        //~ printf("2\n");
        //~ printf("%d\n", buttonCode);
        //~ printf("%d\n", buttonTracker);
        
        }while(1);
        

                
    return 0;
}
