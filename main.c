
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
        gpioPtr[10] = 1<<9;
        
    }
    
    //set pin 9
    else if (bit == 1) 
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

//function: print_Message
//parameters: button
//return: none
//functionality: Prints an appropriate message depending on which button was pressed
void print_Message(int button)
{
    char * string[] = {"B", "Y", "Select", "Start", "Joy-Pad UP", "Joy-Pad DOWN", "Joy-Pad LEFT", "Joy-Pad RIGHT", "A", "X", "L", "R", "?", "?", "?", "?", "?" };
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

//function: read_SNES
//parameters: gpioPtr
//return: code of the button pressed
//functionality: loop 16 times to check if a button is pressedon the SNES controller
int read_SNES(unsigned int *gpioPtr)
{

    int value = 0;
    for(int i = 0; i < 16; i++)
    {

        wait(6);

        //clear pin 11
        write_CLOCK(gpioPtr, 0);

        wait(6);

        value = read_DATA(gpioPtr);
        
	//return the button code value which indicates which button was pressed 
        if (value == 0 && i < 12)
        {
            return i;
        }
        
	//set pin 11
        write_CLOCK(gpioPtr, 1);
    }
    
    return -1;
    
}

int main()
{
    printf("Created by Parth Patel and Shailly Patel\n");
    
    
    // get gpio pointer
    unsigned int *gpioPtr = getGPIOPtr();  

    //button code to specify which button was pressed
    int buttonCode;
    
    //sameButton which keeps track of which button is being held to avoid printing it again.
    int sameButton = -2;
    
    printf("Press a button...\n");
    
    do
    {
        //intialize the pins
        init_GPIO(gpioPtr);
        
        //set pin 9
        write_LATCH(gpioPtr, 1);
        
        //delay 12 microseconds
        wait(12);
        
        //clear pin 9
        //offset is 0x28 -> 10 
        write_LATCH(gpioPtr, 0);

        //readSNES 
        buttonCode = read_SNES(gpioPtr);
	
	//when a button is pressed, buttonCode >= 0
        if(buttonCode >= 0)
        {
	    //set pin 11
            write_CLOCK(gpioPtr, 1);
	    
	    //when the button that was pressed is let go, buttonCode will be equal to -1 but sameButton will be equal to the previous button code
	    //in which case, the if condition is passed and asks the user to press a button
            if(buttonCode != sameButton)
            {
		//print the message displaying what message was printed 
                print_Message(buttonCode);
                printf("Press a button...\n");
                wait(180000); //This is to reduce the effects of the sensitvity of the controller
            }
	    
	    //set sameButton to the buttonCode of which button is being pressed
            sameButton = buttonCode;
        }
    
	//re-initialize sameButton if no button is pressed
        if (buttonCode == -1)
        {
            sameButton = -2;   
        }
        
    } while(1);
  
    return 0;
}

