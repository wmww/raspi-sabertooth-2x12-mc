#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "GPIO.h"

volatile unsigned int *GpioPin::gpioPtr=0;

GpioPin::GpioPin(int pinNum)
{
	if (!gpioPtr)
	{
		setup();
	}

	if ((pinNum>=2 && pinNum<=27) || pinNum==32) //2 through 27 are normal pin numbers, 32 apparently controls the camera LED
	{
		if (gpioPtr)
		{
			pin=pinNum;
			*(gpioPtr+((pin)/10)) &= ~(7<<(((pin)%10)*3)); //clear the correct bits, all that is needed to set up as input
			*(gpioPtr+((pin)/10)) |=  (1<<(((pin)%10)*3)); //set up for output
			//*(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3)) not sure what this does, something about ALT
		}
		else
			pin=0;
	}
	else
	{
		pin=0;
		std::cout << "invalid pin number " << pinNum << std::endl;
	}
	
	set(0);
}

GpioPin::~GpioPin()
{
	set(0);
}

void GpioPin::setup()
{
	void *gpio_map;
	int mem_fd;

	/* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      std::cout << "can't open /dev/mem.\nperhaps running without root privileges?" << std::endl;
      return;
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      4*1024,		//Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      0x20200000         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      std::cout << "mmap error " << gpio_map << std::endl;//errno also set!
      return;
   }

   // Always use volatile pointer!
   gpioPtr = ( unsigned *)gpio_map; //there is assumed to be an error unless gpioPtr is set
}

void GpioPin::set(bool state)
{
	pinState=state;
	
	if (pin)
	{
		if (state)
		{
			*(gpioPtr+7)=1<<pin;
		}
		else
		{
			*(gpioPtr+10)=1<<pin;
		}
	}
	else
		std::cout << "failed to set pin" << std::endl;

	//*(gpio+13)&(1<<g) //this somehow says if it is on if its an input
	//*(gpio+37) //something to do with pulling up and down
	//*(gpio+38) // Pull up/pull down clock
}

