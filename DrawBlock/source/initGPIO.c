#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "initGPIO.h"

unsigned int *getGPIOPtr(void)
{
	int fdgpio = open("/dev/gpiomem", O_RDWR);
	
	if (fdgpio <0) {
		printf("unable to open");
	}
	
	unsigned int *gpioPtr = (unsigned int *)mmap(0,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fdgpio,0);
	
	return gpioPtr;
}
