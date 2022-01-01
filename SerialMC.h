#pragma once

#include "GPIO.h"

class SerialMC
{
public:
	SerialMC(); //reset pin is 18 on my 2.0 bot
	~SerialMC();
	
	void turnOff();
	void set(double lSpd, double rSpd); //speeds will be clamped between -1 and 1
	double getLSpd() {return lSpd;}
	double getRSpd() {return rSpd;}
	
private:
	static bool hasInst; //used to stop multiple instances of this class from interfering with each other
		//initalized to 0, checked, then set to 1 int the constructor
	int uart0_filestream;
	bool error;
	
	double lSpd, rSpd;
};

