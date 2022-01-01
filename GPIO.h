#pragma once

class GpioPin
{

public:

	GpioPin(int pinNum);
	~GpioPin();
	
	bool getState() {return pinState;}
	int getPin() {return pin;}
	bool getIfError() {return pin==0;}

	void set(bool state);

private:

	int pin; //the pin number, if 0 then there has been an error and trying to set this pin will fail
	static volatile unsigned int *gpioPtr;
	bool pinState;
	void setup();
};


