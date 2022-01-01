
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>
#include <math.h>

#include "SerialMC.h"

bool SerialMC::hasInst=0;

SerialMC::SerialMC()
{
	struct termios options;
	
	error=0;
	uart0_filestream=-1;
	lSpd=0;
	rSpd=0;
	
	if(hasInst)
	{
		std::cout << "multiple instances of the SerialMC class!\n";
		error=1;
	}
	else
	{
		uart0_filestream = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
		
		if (uart0_filestream<0)
		{
			std::cout << "uart failed to open\n";
			error=1;
		}
	}
	
	if(!error)
	{
		tcgetattr(uart0_filestream, &options);
		options.c_cflag=B9600 | CS8 | CLOCAL | CREAD;
		options.c_iflag=IGNPAR;
		options.c_oflag=0;
		options.c_lflag=0;
		tcflush(uart0_filestream, TCIFLUSH);
		tcsetattr(uart0_filestream, TCSANOW, &options);
		
		hasInst=1;
	}
	
	turnOff();
}

SerialMC::~SerialMC()
{
	turnOff();
	
	hasInst=0;
}

void SerialMC::turnOff()
{
	if (!error)
	{
		unsigned char txBuffer=0;
		
		if (write(uart0_filestream, &txBuffer, 1)<0)
			std::cout << "error sending data/n";
		else
			std::cout << "motors turned off" << "\n";
	}
	else
	{
		std::cout << "failed to turn off motors" << "\n";
	}
}

void SerialMC::set(double lSpd, double rSpd)
{
	if (!error && (this->lSpd!=lSpd || this->rSpd!=rSpd))
	{
		if (lSpd<-1)
			lSpd=-1;
		if (lSpd>1)
			lSpd=1;
		if (rSpd<-1)
			rSpd=-1;
		if (rSpd>1)
			rSpd=1;
		
		unsigned char txBuffer;
		
		txBuffer=64+lSpd*63;
		
		if (write(uart0_filestream, &txBuffer, 1)<0)
			std::cout << "error sending data/n";
		
		txBuffer=192+rSpd*63;
		
		if (write(uart0_filestream, &txBuffer, 1)<0)
			std::cout << "error sending data/n";
		
		this->lSpd=lSpd;
		this->rSpd=rSpd;
		
		std::cout << "motor speeds set to " << lSpd << ", " << rSpd << "\n";
	}
	else
    {
		std::cout << "failed to set motor speeds to " << lSpd << ", " << rSpd << "\n";
    }
}
