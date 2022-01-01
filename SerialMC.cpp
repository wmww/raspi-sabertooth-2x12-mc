
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
		
		//std::cout << "motor speeds set to " << lSpd << ", " << rSpd << "\n";
	}
	//else
		//std::cout << "failed to set motor speeds to " << lSpd << ", " << rSpd << "\n";
}


///for old Pololu motor controller

/*SerialMC::SerialMC(int resetPin)
{
	struct termios options;
	
	error=0;
	setsSinceReset=0;
	reset=0;
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
		
		reset=new GpioPin(resetPin);
		
		if (!reset || reset->getIfError())
		{
			std::cout << "motor controller reset pin failed\n";
			error=1;
		}
		//else
		//	reset->set(1);
		
		//configMC();
		
		hasInst=1;
	}
}

SerialMC::~SerialMC()
{
	turnOff();
	
	if (reset)
		delete reset;
	
	hasInst=0;
}

void SerialMC::turnOff()
{
	if (!error)
	{
		reset->set(0);
		lSpd=0;
		rSpd=0;
		setsSinceReset=0;
	}
}

void SerialMC::set(double lSpd, double rSpd)
{
	if (!error && (this->lSpd!=lSpd || this->rSpd!=rSpd))
	{
		if (!reset->getState()) //if we have been turned off
		{
			reset->set(1);
			usleep(100); //this is the motor needs this time before it receives serial input
		}
		
		if (setsSinceReset>MAX_SETS)
		{ //without this, the controller might become unresponsive
			std::cout << "motor controller has been set " << MAX_SETS << " times. resetting it...\n";
			reset->set(0);
			usleep(4); //documentation says 2 microseconds is sufficient
			reset->set(1);
			setsSinceReset=0;
		}
		
		if(lSpd<-1)
			lSpd=-1;
		if(lSpd>1)
			lSpd=1;
		if(rSpd<-1)
			rSpd=-1;
		if(rSpd>1)
			rSpd=1;
		
		unsigned char txBuffer[4];
		txBuffer[0]=128;
		txBuffer[1]=0;
		
		txBuffer[2]=6+(lSpd>0);
		txBuffer[3]=fabs(lSpd)*127;
		
		if (write(uart0_filestream, &txBuffer[0], 4)<0)
			std::cout << "error sending data/n";
		
		txBuffer[2]=4+(rSpd>0);
		txBuffer[3]=fabs(rSpd)*127;
		
		if (write(uart0_filestream, &txBuffer[0], 4)<0)
			std::cout << "error sending data/n";
		
		this->lSpd=lSpd;
		this->rSpd=rSpd;
		
		++setsSinceReset;
		
		//std::cout << "motor speeds set to " << lSpd << ", " << rSpd << "\n";
	}
	//else
		//std::cout << "failed to set motor speeds to " << lSpd << ", " << rSpd << "\n";
}

void SerialMC::configMC()
{
	if (!error)
	{
		reset->set(1);
		
		unsigned char txBuffer[3];
		txBuffer[0]=128;
		txBuffer[1]=2;
		txBuffer[2]=2;
		
		if (write(uart0_filestream, &txBuffer[0], 3)<0)
			std::cout << "error sending data/n";
		
		reset->set(0);
		
		std::cout << "motor controller configured\n";
	}
}

*/
